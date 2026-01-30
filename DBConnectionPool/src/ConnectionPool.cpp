#include "ConnectionPool.h"

ConnectionPool& ConnectionPool::getInstance(){
    static ConnectionPool instance;
    return instance;
}

//初始化连接池
bool ConnectionPool::init(const DBConfig& db_config,const PoolConfig& pool_config)
{
    std::lock_guard<std::mutex> lock(mtx_);
    //如果正在运行，则代表已经初始化完成，防止重复初始化
    if(is_running_) return false;

    this->db_config_ = db_config;
    this->pool_config_ = pool_config;

    if(pool_config.min_connections<0||
        pool_config.max_connections<=0||
        pool_config.min_connections>pool_config.max_connections)
    {
        throw std::invalid_argument("Invalid pool configuration");
    }

    //按最小连接数创建连接
    for(int i=0;i<pool_config.min_connections;++i)
    {
        std::unique_ptr<Connection> conn = createConnection();
        if(!conn){
            std::cerr<<"Failed to create initial connections"<<(conn? conn->getError():"Unknown error")<<std::endl;
            //出现创建失败则清除所有当前创建的连接
            clearConnections();
            return false;
        }
        idle_connections_.push(std::move(conn));
    }

    total_connections_ = pool_config.min_connections;
    is_running_ = true;

    //启动连接维护线程
    startMaintenanceThread();
    return true;
}

//关闭连接池
void ConnectionPool::close()
{
    std::lock_guard<std::mutex> lock(mtx_);
    //如果没运行就直接返回
    if(!is_running_) return;
    //通知维护线程退出
    cond_var_.notify_all();
    if(maintenance_thread_.joinable()){
        maintenance_thread_.join();
    }
    clearConnections();
}

//获取连接(返回智能指针，自动释放)
std::unique_ptr<Connection, std::function<void(Connection*)>> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mtx_);
    auto start_time = std::chrono::steady_clock::now();
    //此时没有多余连接并且总链接已经大于了最大连接数
    while (idle_connections_.empty() && total_connections_ >= pool_config_.max_connections)
    {
        //计划剩余等待时间
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time
        ).count();
        int remaining = pool_config_.acquire_timeout-elapsed;
        if(remaining<=0)
        {
            throw std::runtime_error("Time acquiring connection");
        }

         // 等待指定时间或被唤醒
        if (cond_var_.wait_for(lock, std::chrono::milliseconds(remaining)) == std::cv_status::timeout) {
            throw std::runtime_error("Timeout acquiring connection");
        }

    }
    //如果没有空连接，且未达连接上限，则可以继续创建新线程
    if(idle_connections_.empty() && total_connections_ < pool_config_.max_connections)
    {
         std::unique_ptr<Connection> conn = createConnection();
            if (!conn) {
                throw std::runtime_error("Failed to create new connection: " + conn->getError());
            }
            total_connections_++;
            // 返回连接，自定义释放器（归还到池）
            return {conn.release(), [this](Connection* c) { releaseConnection(c); }};
    }
    //从空闲队列取一个连接
    std::unique_ptr<Connection> conn = std::move(idle_connections_.front());
    idle_connections_.pop();
    //检查连接是否有效，无效则重建
    if(!conn->isAlive()){
        std::cerr<<"Connection is invalid"<<std::endl;
        conn = createConnection();
        if(!conn){
            throw std::runtime_error("Failed to create connection");
        }
        
    }
    // 返回连接，自定义释放器
    return {conn.release(), [this](Connection* c) { releaseConnection(c); }};
}

ConnectionPool::~ConnectionPool()
{
    close();
}


// 创建新连接
std::unique_ptr<Connection>ConnectionPool:: createConnection() {
    std::unique_ptr<Connection> conn(new Connection());
    if (!conn->connect(db_config_)) {
        return nullptr;
    }
    return conn;
}

void ConnectionPool::releaseConnection(Connection *conn)
{
    if(!conn){
        return;
    }
    std::lock_guard<std::mutex>lock(mtx_);
    if(!is_running_){
        delete conn;
        total_connections_--;
        return;
    }
    conn->reset();
    idle_connections_.push(std::unique_ptr<Connection>(conn));
    cond_var_.notify_one();
}

void ConnectionPool::startMaintenanceThread()
{
    maintenance_thread_=std::thread([this](){
        while(is_running_)
        {
            //等待检测周期
            std::this_thread::sleep_for(std::chrono::milliseconds(pool_config_.check_interval));
            std::lock_guard<std::mutex> lock(mtx_);
            if(!is_running_) break;

            //回收超时的空闲连接
            int excess = idle_connections_.size() - pool_config_.min_connections;
            
            if (excess > 0) {
                // 遍历空闲队列，移除超时连接
                std::queue<std::unique_ptr<Connection>> temp;
                while (!idle_connections_.empty()) {
                    auto conn = std::move(idle_connections_.front());
                    idle_connections_.pop();

                    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()
                    ).count();

                    // 若连接空闲超时且仍有多余，则销毁
                    if (excess > 0 && (now - conn->getLastActiveTime() > pool_config_.idle_timeout)) {
                        delete conn.release();
                        total_connections_--;
                        excess--;
                    } else {
                        temp.push(std::move(conn));
                    }
                }
                //替换队列
                idle_connections_=std::move(temp);

            }
            

            //检测无效连接并替换
            std::queue<std::unique_ptr<Connection>> valid_conns;
            while (!idle_connections_.empty()) {
                auto conn = std::move(idle_connections_.front());
                idle_connections_.pop();
                if(!conn->isAlive())
                {
                    std::cerr<<"Detect invalid connection, recreate it" << std::endl;
                    auto new_conn = createConnection();
                    if(new_conn){
                        valid_conns.push(std::move(new_conn));
                    }else{
                        std::cerr<<"failed to create new_conn"<<std::endl;
                        total_connections_--;
                    }
                }
            }
            idle_connections_=std::move(valid_conns);
            while(total_connections_<pool_config_.min_connections)
            {
                auto new_conn = createConnection();
                if(new_conn){
                    idle_connections_.push(std::move(new_conn));
                    total_connections_++;
                }else{
                    std::cerr << "Failed to补充 connection to reach min_connections" << std::endl;
                    break;
                }
            }
        }
    });
}


// 清理所有连接
void ConnectionPool::clearConnections() {
    while (!idle_connections_.empty()) {
        idle_connections_.pop();
    }
    total_connections_ = 0;
}

// 动态调整配置
void ConnectionPool::updateConfig(const PoolConfig& new_config) {
    std::lock_guard<std::mutex> lock(mtx_);
    // 校验新配置
    if (new_config.min_connections < 0 || 
        new_config.max_connections <= 0 || 
        new_config.min_connections > new_config.max_connections) {
        throw std::invalid_argument("Invalid new pool configuration");
    }
    pool_config_ = new_config;
    // 若新的最小连接数大于当前总连接数，补充连接
    while (total_connections_ < pool_config_.min_connections) {
        // 补充连接逻辑...
    }
}

