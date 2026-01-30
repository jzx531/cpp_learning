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
    }
}






