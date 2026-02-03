#ifndef FORMAT_HPP
#define FORMAT_HPP

#include "level.hpp"
#include "message.hpp"
#include <vector>
#include <sstream>
#include <ctime>
#include <cassert>

namespace LOG{
    //抽象基类
    class FormatItem
    {
        public:
            using ptr = std::shared_ptr<FormatItem>;
            virtual void format(std::ostream &out,const LogMsg &msg) = 0;
    };

    class MsgFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._payload;
        }
    };

    class LevelFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << LogLevel::tostring(msg._level);
        }
    };

    class TimeFormatItem : public FormatItem
    {
        private: std::string _time_fmt;//%H:%M:%S
        public: 
        TimeFormatItem(const std::string &fmt = "%H:%M:%S") : _time_fmt(fmt) {}

        void format(std::ostream &out, const LogMsg &msg) override
        {
            struct tm local_time;
             // 使用 localtime_r：将 now 转换为本地时间，结果存入 local_time
            // localtime_r 是线程安全的
            localtime_r(&msg._time, &local_time);
            char buffer[32];
            strftime(buffer, sizeof(buffer), _time_fmt.c_str(), &local_time);
            out << buffer;
        }
    };

    class  FileFormatItem : public FormatItem
    {
        public:
            void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._file;
        }
    }

    class LineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._line;
        }
    };

    class ThreadFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._tid;
        }
    };

    class LoggerFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMsg &msg) override
        {
            out << msg._logger;
        }
    };




}

#endif // FORMAT_HPP


