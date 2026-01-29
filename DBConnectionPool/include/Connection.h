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

        void upadateLastActiveTime();
    public:
        Connection();


#endif // CONNECTION_H
