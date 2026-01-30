#include "Connection.h"

void Connection::updateLastActiveTime()
{
    auto now = std::chrono::system_clock::now();
    last_active_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

Connection::Connection():mysql_(mysql_init(nullptr)){};

Connection::~Connection()
{
    if(this->mysql_)
    {
        mysql_close(this->mysql_);
        this->mysql_ = nullptr;
    }
}

Connection::Connection(Connection && other) noexcept:mysql_(other.mysql_)
{
    if(this != &other)
    {
        mysql_ = other.mysql_;
        other.mysql_ =nullptr;
    }
}

Connection& Connection::operator=(Connection &&other)noexcept
{
    if(this != &other)
    {
        mysql_ = other.mysql_;
        other.mysql_ =nullptr;
    }
    return *this;
}

bool Connection::connect(const DBConfig& config)
{
    if(!mysql_) return false;
    //连接数据库

    // if(!mysql_real_connect(
    //             mysql_,
    //             // config.host.c_str(),
    //             // config.user.c_str(),
    //             // config.password.c_str(),
    //             // config.dbname.c_str(),
    //             // config.port,
    //             // nullptr,
    //             nullptr,        // host: 使用 Unix socket（localhost）
    //             "root",         // user
    //             "",             // password: auth_socket 不需要密码
    //             "mydb",         // ✅ 指定数据库名！
    //             0,              // port: 0 表示默认（socket 模式忽略）
    //             nullptr,        // unix_socket: 默认路径
    //             CLIENT_MULTI_STATEMENTS
    // ))
    printf("connecting");
    if (mysql_real_connect(
            mysql_,
            nullptr,        // host: 使用 Unix socket（localhost）
            "appuser",         // user
            "app123",             // password: auth_socket 不需要密码
            config.dbname.c_str(),         // ✅ 指定数据库名！
            0,              // port: 0 表示默认（socket 模式忽略）
            nullptr,        // unix_socket: 默认路径
            CLIENT_MULTI_STATEMENTS               // client_flag
        ) == nullptr){
        printf("fail create connection");
        error_msg_ =mysql_error(mysql_);
        return false;
    }
    //设置字符集
    if(config.charset.length()>0){
        if(mysql_set_character_set(mysql_,/*std::to_string(config.charset).c_str()*/config.charset.c_str())!=0)
        {
            error_msg_ = mysql_error(mysql_);
            return false;
        }
    }

    //更新最后活动时间
    updateLastActiveTime();
    return true;
}

//执行SQL(INSERT/UPDATE/DELETE)
bool Connection::execute(const std::string & sql)
{
    if(!mysql_){
        error_msg_ ="Connection is invalid";
        return false;
    }
    int ret = mysql_real_query(mysql_,sql.c_str(),sql.size());
    if(ret !=0){
        error_msg_ = mysql_error(mysql_);
        return false;
    }
    updateLastActiveTime();
    return true;
}

//执行查询(SELECT),返回结果集
MYSQL_RES* Connection::query(const std::string &sql)
{
    if(!mysql_){
        error_msg_ = "Connection is invalid";
        return nullptr;
    }

    int ret = mysql_real_query(mysql_,sql.c_str(),sql.size());
    if(ret != 0){
        error_msg_ = mysql_error(mysql_);
        return nullptr;
    }

    updateLastActiveTime();
    return mysql_store_result(mysql_);
}


//连接重置
void Connection::reset(){
    if(!mysql_)return;
    mysql_rollback(mysql_);
    //清除结果
    mysql_free_result(mysql_store_result(mysql_));
    mysql_reset_connection(mysql_);
    updateLastActiveTime();
}


std::string Connection::getError() const{
    return error_msg_;
}

uint64_t Connection::getLastActiveTime()const{
    return last_active_time_;
}

// 检测连接是否有效（通过执行ping命令）
bool Connection::isAlive() {
    if (!mysql_) return false;
    // mysql_ping在连接有效时返回0，无效时尝试重连（若配置了自动重连）
    return mysql_ping(mysql_) == 0;
}

MYSQL * Connection::getMysql()
{
    return mysql_;
}