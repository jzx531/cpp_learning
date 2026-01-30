#include "DBUtil.h"

// 绑定连接到当前线程（事务开始时）
void bindConnectionToThread(std::unique_ptr<Connection, std::function<void(Connection*)>>& conn) {
    static thread_local std::unique_ptr<Connection, std::function<void(Connection*)>> thread_conn;
    thread_conn = std::move(conn);
}

// 从线程获取绑定的连接（事务中）
std::unique_ptr<Connection, std::function<void(Connection*)>>& getThreadBoundConnection() {
    static thread_local std::unique_ptr<Connection, std::function<void(Connection*)>> thread_conn;
    if (!thread_conn) {
        thread_conn = ConnectionPool::getInstance().getConnection();
    }
    return thread_conn;
}


// 初始化连接池
void initPool() {
    DBConfig db_cfg;
    db_cfg.host ="";
    db_cfg.port = 0;
    db_cfg.user = "root";
    db_cfg.password = "";
    db_cfg.dbname = "mydb";
    db_cfg.charset = "utf8mb4";

    PoolConfig pool_cfg;
    pool_cfg.min_connections = 5;
    pool_cfg.max_connections = 20;
    pool_cfg.acquire_timeout = 3000;
    pool_cfg.idle_timeout = 60000;
    pool_cfg.check_interval = 30000;

    if (!ConnectionPool::getInstance().init(db_cfg, pool_cfg)) {
        throw std::runtime_error("Failed to initialize connection pool");
    }
}

// 执行查询示例
void queryExample() {
    try {
        // 获取连接（自动释放）
        auto conn = ConnectionPool::getInstance().getConnection();
        
        // 执行查询
        MYSQL_RES* res = conn->query("SELECT id, name FROM users LIMIT 10");
        if (!res) {
            std::cerr << "Query failed: " << conn->getError() << std::endl;
            return;
        }

        // 处理结果集
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr) {
            unsigned long* lengths = mysql_fetch_lengths(res);
            std::cout << "id: " << row[0] << ", name: " << row[1] << std::endl;
        }

        // 释放结果集
        mysql_free_result(res);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// 执行更新示例
void updateExample() {
    try {
        auto conn = ConnectionPool::getInstance().getConnection();
        bool success = conn->execute("UPDATE users SET age = age + 1 WHERE id = 1");
        if (success) {
            std::cout << "Update success, affected rows: " << mysql_affected_rows(conn->getMysql()) << std::endl;
        } else {
            std::cerr << "Update failed: " << conn->getError() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
