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
    other.mysql_ = nullptr;
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
    if(!mysql_real_connect(
                mysql_,
                config.host.c_str(),
                config.user.c_str(),
                config.password.c_str(),
                config.dbname.c_str(),
                config.port,
                nullptr,
                CLIENT_MULTI_STATEMENTS
    )){
        error_msg_ =mysql_error(mysql_);
        return false;
    }
    //设置字符集
    if(config.charset/*.length()*/>0){
        if(mysql_set_character_set(mysql_,std::to_string(config.charset).c_str())!=0)
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

