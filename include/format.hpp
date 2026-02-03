#ifndef FORMAT_HPP
#define FORMAT_HPP

#include "level.hpp"
#include "message.hpp"
#include <vector>
#include <sstream>
#include <ctime>
#include <cassert>
#include <memory>

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
            out << LogLevel::toString(msg._level);
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
    };

    class NLineFormatItem : public FormatItem
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

    class TabFormatItem: public FormatItem
    {
        public:
            void format(std::ostream &out, const LogMsg &msg) override
            {
                out<<'\n';
            }
    };

    class OtherFormatItem: public FormatItem
    {
        private:
            std::string _str;
        public:
            OtherFormatItem(const std::string &str):_str(str){}
            void format(std::ostream &out,const LogMsg&msg) override{
                out<<_str;
            }
    };

    /*
        %d 表示日期，包含子格式 {%H:%M:%S}
        %t 表示线程ID
        %c 表示日志器名称
        %f 表示源码文件名
        %l 表示源码行号
        %p 表示日志级别
        %m 表示主体消息
        %n 表示换行
    */
    class Formatter
    {
    private:
        std::string _pattern; // 格式化规则字符串
        std::vector<FormatItem::ptr> _items;
    public:
        using ptr = std::shared_ptr<Formatter>;
         // 时间{年-月-日 时:分:秒}缩进 线程ID 缩进 [日志级别] 缩进 [日志名称] 缩进 文件名:行号 缩进 消息换行
        Formatter(const std::string &pattern ="[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n"):_pattern(pattern)
        {
            assert(parsePattern());
        }

        //对msg进行格式化
        void format(std::ostream &out, const LogMsg &msg)
        {
            for (auto &item : _items)
            {
                item->format(out, msg);
            }
        }
        std::string format(const LogMsg &msg)
        {
            std::stringstream ss;
            format(ss,msg);
            return ss.str();
        }

    private:
        //解析格式化字符串
        bool parsePattern()
        {
            std::vector<std::pair<std::string, std::string>> fmt_order;
            size_t pos = 0;
            std::string key, val;
            while(pos < _pattern.size())
            {
                if (_pattern[pos] != '%')
                {
                    val.push_back(_pattern[pos++]);
                    continue;
                }
                // 转义的 %%
                if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%')
                {
                    val.push_back('%');
                    pos += 2;
                    continue;
                }
                // 遇到 % 且不是转义 → 开始解析格式符
                // 如果之前累积了普通文本（val 非空），先作为一个“纯文本项”存入 fmt_order（key 为空字符串表示普通文本）。
                if (val.empty() == false)
                {
                    fmt_order.push_back(std::make_pair("", val));
                    val.clear();
                }
                // 跳过 %，检查是否越界（如字符串以 % 结尾是非法的）。
                pos += 1;
                if (pos == _pattern.size())
                {
                    std::cout << "%之后没有格式化字符\n";
                    return false;
                }
                // 读取格式符（如 'd', 'l' 等），存入 key（虽然是单字符，但用 string 存）。
                key = _pattern[pos];
                pos += 1;
                // 格式符后跟 {...} 参数？
                 if (pos < _pattern.size() && _pattern[pos] == '{')
                {
                    pos += 1;
                    while (pos < _pattern.size() && _pattern[pos] != '}')
                    {
                        val.push_back(_pattern[pos++]);
                    }

                    if (pos == _pattern.size())
                    {
                        std::cout << "子规则{}匹配出错\n";
                        return false;
                    }

                    pos += 1;
                }

                fmt_order.push_back(std::make_pair(key, val));
                key.clear();
                val.clear();
            }
            for (auto &it : fmt_order)
            {
                _items.push_back(createItem(it.first, it.second));
            }
            return true;
        }

        // 根据不同的格式化字符创建不同得格式化子项对象
        FormatItem::ptr createItem(const std::string &key, const std::string &val)
        {
            if (key == "d")
                return std::make_shared<TimeFormatItem>(val);
            if (key == "t")
                return std::make_shared<ThreadFormatItem>();
            if (key == "c")
                return std::make_shared<LoggerFormatItem>();
            if (key == "f")
                return std::make_shared<FileFormatItem>();
            if (key == "l")
                return std::make_shared<NLineFormatItem>();
            if (key == "p")
                return std::make_shared<LevelFormatItem>();
            if (key == "T")
                return std::make_shared<TabFormatItem>();
            if (key == "m")
                return std::make_shared<MsgFormatItem>();
            if (key == "n")
                return std::make_shared<NLineFormatItem>();
            if (key == "")
                return std::make_shared<OtherFormatItem>(val);
            std::cout << "没有对应的格式化字符串：%" << key << std::endl;
            abort();//错误，程序停止
        }
    };

}

#endif // FORMAT_HPP


