// connect_db.cpp
#include <iostream>
#include <mysql/mysql.h>

int main() {
    // 初始化 MySQL 连接
    MYSQL* conn = mysql_init(nullptr);
    if (!conn) {
        std::cerr << "❌ mysql_init() failed\n";
        return 1;
    }

    // 设置字符集为 utf8mb4（防止中文乱码）
    if (mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8mb4")) {
        std::cerr << "⚠️ Warning: Failed to set charset\n";
    }

    // 连接数据库：显式指定数据库名为 "mydb"
    if (mysql_real_connect(
            conn,
            nullptr,        // host: 使用 Unix socket（localhost）
            "appuser",         // user
            "app123",             // password: auth_socket 不需要密码
            "mydb",         // ✅ 指定数据库名！
            0,              // port: 0 表示默认（socket 模式忽略）
            nullptr,        // unix_socket: 默认路径
            CLIENT_MULTI_STATEMENTS               // client_flag
        ) == nullptr) {

        std::cerr << "❌ Connection failed: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return 1;
    }

    std::cout << "✅ Connected successfully!\n";
    std::cout << "Server version: " << mysql_get_server_info(conn) << "\n\n";

    // 查询 MySQL 版本（可选）
    if (mysql_query(conn, "SELECT VERSION()")) {
        std::cerr << "Query error: " << mysql_error(conn) << std::endl;
    } else {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res && mysql_num_rows(res) > 0) {
            MYSQL_ROW row = mysql_fetch_row(res);
            std::cout << "MySQL VERSION(): " << (row[0] ? row[0] : "NULL") << "\n\n";
            mysql_free_result(res);
        }
    }

    // 查询 students 表
    std::cout << "📚 学生信息列表:\n";
    if (mysql_query(conn, "SELECT name, student_id FROM students")) {
        std::cerr << "❌ Query students failed: " << mysql_error(conn) << std::endl;
    } else {
        MYSQL_RES* res = mysql_store_result(conn);
        if (res) {
            if (mysql_num_rows(res) == 0) {
                std::cout << "   (暂无数据)\n";
            } else {
                while (MYSQL_ROW row = mysql_fetch_row(res)) {
                    // 安全处理 NULL 值
                    const char* name = row[0] ? row[0] : "(null)";
                    const char* sid  = row[1] ? row[1] : "(null)";
                    std::cout << "   姓名: " << name << ", 学号: " << sid << '\n';
                }
            }
            mysql_free_result(res);
        }
    }

    mysql_close(conn);
    std::cout << "\n👋 Connection closed.\n";
    return 0;
}