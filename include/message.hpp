#ifndef MESSAGE_HPP
#define MESSAGE_HPP

/*
定义日志消息类：
1. 日志的输出时间
2. 日志等级
3. 源文件名称
4. 源代码行号
5. 线程ID
6. 日志主体消息
7. 日志器名称
*/

#include <iostream>
#include <string>
#include <thread>
#include "util.hpp"
#include "level.hpp"

namespace LOG
{
    struct LogMsg
    {
        time_t _time;//日志输出时间
        LogLevel::value _level;//日志等级
        std::string _file;//源文件名称
        size_t _line;//源代码行号
        std::thread::id _tid;//线程ID
        std::string _logger;//日志器名称
        std::string _payload;//日志主体消息

        LogMsg(LogLevel::value level,
            size_t line,
            const std::string file,
            const std::string logger,
            const std::string msg
        ) : 
        _time(util::Date::getTime()),
        _level(level),
        _line(line),
        _tid(std::this_thread::get_id()), // get_id 获取当前线程的ID
        _file(file),
        _logger(logger),
        _payload(msg) {}
    };
    
}


#endif// MESSAGE_HPP


