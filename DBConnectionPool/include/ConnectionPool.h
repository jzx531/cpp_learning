#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <stdexcept>
#include <chrono>
#include <iostream>

#include "Connection.h"

//连接池配置
struct PoolConfig{
    int min_connections = 5;//最小连接数
    int max_connections = 20;//最大连接数
    int acquire_timeout = 3000;//获取连接超时时间
    int idle_timeout = 60000;//空闲连接超时时间
    int check_interval = 30000;//连接检测周期
};

//数据连接池
class ConnectionPool
{
    private:
        DBConfig db_config_;
        PoolConfig pool_config_;
        std::queue<std::unique_ptr<Connection>> idle_connections_;
        int total_connections_ = 0;
        std::mutex mtx_;
        std::condition_variable cond_var_;
        std::thread maintenance_thread_;
        std::atomic<bool> is_running_{false};

    public:
        //单例模式
        static ConnectionPool& getInstance();
        //初始化连接池
        bool init(const DBConfig& db_config,const PoolConfig& pool_config);

        //关闭连接池
        void close();

        //获取连接(返回智能指针，自动释放)
        std::unique_ptr<Connection, std::function<void(Connection*)>> getConnection();
        
        // 禁止拷贝
        ConnectionPool(const ConnectionPool&) = delete;
        ConnectionPool& operator=(const ConnectionPool&) = delete;

    private:
        ConnectionPool()=default;
        ~ConnectionPool();
        //创建连接
        std::unique_ptr<Connection> createConnection();
        //释放连接到连接池
        void releaseConnection(Connection *conn);
        //启动连接维护线程
        void startMaintenanceThread();
        //清理所有连接
        void clearConnections();

        // 动态调整配置
        void updateConfig(const PoolConfig& new_config);
        
};       


#endif


