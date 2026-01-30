#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <chrono>
#include <memory>
#include <mysql/mysql.h>

struct DBConfig{
    std::string host;
    uint16_t port;
    std::string user;
    std::string password;
    std::string dbname;
    int charset;
};

class Connection{
    private:
        MYSQL *  mysql_=nullptr;
        std::string error_msg_;
        uint64_t last_active_time_=0;

        void updateLastActiveTime();
    public:
        Connection();
        ~Connection();

        //禁止拷贝
        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        //移动构造函数
        Connection(Connection && other) noexcept;
        Connection& operator=(Connection && other) noexcept;

        bool connect(const DBConfig& config);

        bool execute(const std::string & sql);

        MYSQL_RES * query(const std::string& sql);

        void reset();

        std::string getError() const;

        uint64_t getLastActiveTime() const;

};


#endif // CONNECTION_H
