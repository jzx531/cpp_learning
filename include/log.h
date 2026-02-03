#ifndef __MY_LOG__
#define __MY_LOG__
#include "logger.hpp"

namespace LOG
{
    // 1.提供获取指定日志器的全局接口（避免用户自己操作单例对象）
    Logger::ptr getLogger(const std::string &name)
    {
        return LOG::LoggerManager::getInstance().getLogger(name);
    }
    Logger::ptr rootLogger()
    {
        return LOG::LoggerManager::getInstance().rootLogger();
    }

    // 2.实用宏函数对日志器的接口进行代理（代理模式）
    #define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define warn(fmt, ...) warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define error(fmt, ...) error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

    // 3.提供宏函数，直接通过默认日志器进行日志的标准输出打印
    #define DEBUG(fmt, ...) LOG::rootLogger()->debug(fmt, ##__VA_ARGS__)
    #define INFO(fmt, ...) LOG::rootLogger()->info(fmt, ##__VA_ARGS__)
    #define WARN(fmt, ...) LOG::rootLogger()->warn(fmt, ##__VA_ARGS__)
    #define ERROR(fmt, ...) LOG::rootLogger()->error(fmt, ##__VA_ARGS__)
    #define FATAL(fmt, ...) LOG::rootLogger()->fatal(fmt, ##__VA_ARGS__)

}
#endif
