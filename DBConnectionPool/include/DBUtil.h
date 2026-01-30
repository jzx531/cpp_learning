#ifndef DBUTIL_H_
#define DBUTIL_H_

#include "Connection.h"
#include "ConnectionPool.h"

// 绑定连接到当前线程（事务开始时）
void bindConnectionToThread(std::unique_ptr<Connection, std::function<void(Connection*)>>& conn);

// 从线程获取绑定的连接（事务中）
std::unique_ptr<Connection, std::function<void(Connection*)>>& getThreadBoundConnection() ;
// 执行更新示例
void updateExample();
// 执行查询示例
void queryExample();
// 初始化连接池
void initPool();

#endif

