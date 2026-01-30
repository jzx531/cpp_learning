#include "ConnectionPool.h"
#include "DBUtil.h"
int main() {
    
    try {
        initPool();
        
        // 多线程测试
        std::vector<std::thread> threads;
        for (int i = 0; i < 30; ++i) {
            threads.emplace_back(queryExample);
        }
        for (auto& t : threads) {
            t.join();
        }

        updateExample();
    } catch (const std::exception& e) {
        std::cerr << "Main error: " << e.what() << std::endl;
    }

    // 关闭连接池（程序退出时）
    ConnectionPool::getInstance().close();
    return 0;
    
   std::cout << "🧪 Testing Connection class directly...\n";

    // 配置数据库连接参数（与 connect_db.cpp 一致）
    DBConfig config;
    config.host = "";           // 表示 localhost (Unix socket)
    config.port = 0;            // socket 模式忽略端口
    config.user = "appuser";
    config.password = "app123";
    config.dbname = "mydb";
    config.charset = "utf8mb4";

    // 创建 Connection 对象
    Connection conn;

    // 尝试连接
    std::cout << "🔌 Connecting to database...\n";
    if (!conn.connect(config)) {
        std::cerr << "❌ Connection failed: " << conn.getError() << "\n";
        return 1;
    }

    std::cout << "✅ Connected successfully!\n";

    // 执行一个简单查询
    std::cout << "🔍 Executing query: SELECT 'Hello from C++ Connection class!' AS msg;\n";
    MYSQL_RES* res = conn.query("SELECT 'Hello from C++ Connection class!' AS msg");
    if (!res) {
        std::cerr << "❌ Query failed: " << conn.getError() << "\n";
        return 1;
    }

    // 获取结果
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row && row[0]) {
        std::cout << "💬 Result: " << row[0] << "\n";
    }
    mysql_free_result(res);

    // 执行更新（无实际表也可测试语法）
    std::cout << "📝 Trying a dummy update (safe on non-existent table):\n";
    bool ok = conn.execute("CREATE TABLE IF NOT EXISTS test_table (id INT)");
    if (ok) {
        std::cout << "✅ CREATE TABLE succeeded.\n";
    } else {
        std::cerr << "⚠️ CREATE TABLE failed (might be OK): " << conn.getError() << "\n";
    }

    std::cout << "👋 Test completed successfully!\n";
    return 0;
}