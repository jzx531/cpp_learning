#ifndef __M_LOGGER_H__
#define __M_LOGGER_H__
#include "util.hpp"
#include "level.hpp"
#include "format.hpp"
#include "sink.hpp"
#include "looper.hpp"
#include <cstdarg>
#include <atomic>
#include <thread>
#include <mutex>
#include <unordered_map>

namespace LOG
{
    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string &logger_name,
               LogLevel::value level,
               Formatter::ptr &formatter,
               std::vector<LogSink::ptr> &sinks) : 
               _logger_name(logger_name),
               _limit_level(level),
               _formatter(formatter),
               _sinks(sinks.begin(), sinks.end())
        {}

        const std::string& name(){ return _logger_name; }
        void debug(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象， 进行日志格式化，最终落地
            // 判断当前的日志是否达到了输出等级
            if (LogLevel::value::DEBUG < _limit_level)
            {
                return;
            }

            // 对fmt格式化字符串和不定参数进行字符串组织， 得到的日志消息字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == 1)
            {
                std::cout << "vasprintf failed\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::DEBUG, file, line, res);
            free(res);
        }
        void info(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象， 进行日志格式化，最终落地
            if (LogLevel::value::INFO < _limit_level)
            {
                return;
            }

            // 对fmt格式化字符串和不定参数进行字符串组织， 得到的日志消息字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == 1)
            {
                std::cout << "vasprintf failed\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::INFO, file, line, res);
            free(res);
        }
        void warn(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象， 进行日志格式化，最终落地
            if (LogLevel::value::WARN < _limit_level)
            {
                return;
            }
            // 对fmt格式化字符串和不定参数进行字符串组织， 得到的日志消息字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == 1)
            {
                std::cout << "vasprintf failed\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::WARN, file, line, res);
            free(res);
        }
        void error(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象， 进行日志格式化，最终落地
            if (LogLevel::value::ERROR < _limit_level)
            {
                return;
            }
            // 对fmt格式化字符串和不定参数进行字符串组织， 得到的日志消息字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == 1)
            {
                std::cout << "vasprintf failed\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::ERROR, file, line, res);
            free(res);
        }
        void fatal(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 通过传入的参数构造出一个日志消息对象， 进行日志格式化，最终落地
            if (LogLevel::value::FATAL < _limit_level)
            {
                return;
            }
            // 对fmt格式化字符串和不定参数进行字符串组织， 得到的日志消息字符串
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == 1)
            {
                std::cout << "vasprintf failed\n";
                return;
            }
            va_end(ap);
            serialize(LogLevel::value::FATAL, file, line, res);
            free(res);
        }

    protected:
        void serialize(LogLevel::value level, const std::string &file, size_t line, char *str)
        {
            // 构造LogMsg对象
            LogMsg msg(level, line, file, _logger_name, str);
            // 通过格式化工具对LogMsg进行格式化， 得到格式化后的日志字符串
            std::stringstream ss;
            _formatter->format(ss, msg);
            // 对日志进行落地
            log(ss.str().c_str(), ss.str().size());
        }
        virtual void log(const char *data, size_t len) = 0;

    protected:
        std::mutex _mutex;
        std::string _logger_name;                  // 日志器名称
        std::atomic<LogLevel::value> _limit_level; // 限制输出等级
        Formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };

    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string &logger_name,
                   LogLevel::value level,
                   LOG::Formatter::ptr &formatter,
                   std::vector<LogSink::ptr> &sinks) 
                   : Logger(logger_name, level, formatter, sinks)
        {}

    protected:
        void log(const char *data, size_t len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_sinks.empty())
                return;
            for (auto &sink : _sinks)
            {
                sink->log(data, len);
            }
        }
    };

    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(const std::string &logger_name,
                    LogLevel::value level,
                    LOG::Formatter::ptr &formatter,
                    std::vector<LogSink::ptr> &sinks,
                    AsyncType looper_type)
            : Logger(logger_name, level, formatter, sinks),
              _looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::realLog, this, std::placeholders::_1), looper_type))
        {}
        // 将数据写入缓冲区 
        void log(const char *data, size_t len)
        {
            _looper->push(data, len);
        }
        // 设计一个实际落地函数
        void realLog(Buffer &buf)
        {
            if (_sinks.empty())
                return;
            for (auto &sink : _sinks)
            {
                sink->log(buf.begin(), buf.readAbleSize());
            }
        }

    private:
        AsyncLooper::ptr _looper; // 异步工作器
    };

    // 1.抽象一个日志器建造者类（完成日志器所需零部件的构建 & 日志器的构建）
    //  1.设置日志器类型
    //  2.将不同类型的日志器的创建放到同一个日志器建造者类中完成
    enum class LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC
    };

    class LoggerBuilder
    {
    public:
        LoggerBuilder() : _logger_type(LoggerType::LOGGER_SYNC),
                          _limit_level(LogLevel::value::DEBUG),
                          _looper_type(AsyncType::ASYNC_SAFE)
        {}
        void buildLoggerType(LoggerType type) { _logger_type = type; }
        void buildEnableUnSafeAsync() { _looper_type = AsyncType::ASYNC_UNSAFE; }
        void buildLoggerName(const std::string &name) { _logger_name = name; }
        void buildLoggerLevel(LogLevel::value level) { _limit_level = level; }
        void buildFormatter(const std::string &pattern)
        {
            _formatter = std::make_shared<Formatter>(pattern);
        }
        template <typename SinkType, typename... Args>
        void buildSink(Args &&...args) // 由用户自己决定落地方式
        {
            LogSink::ptr psink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }
        virtual Logger::ptr build() = 0;
    protected:
        AsyncType _looper_type;
        LoggerType _logger_type;
        std::string _logger_name;
        std::atomic<LogLevel::value> _limit_level;
        Formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };

    /*2.派生出具体的建造者类---局部日志器的建造者 & 全局日志器的建造者*/
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(_logger_name.empty() == false);
            if (_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }
            if (_sinks.empty())
            {
                buildSink<StdOutSink>();
            }
            if (_logger_type == LoggerType::LOGGER_ASYNC)
            {
                return std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _looper_type);
            }
            return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
        }
    };

    class LoggerManager
    {
    public:
        static LoggerManager& getInstance()
        {
            // c++11之后，静态局部变量，编译器在编译的层面实现了线程安全
            // 当静态局部变量在没有构造完成之前，其他线程进入就会阻塞
            static LoggerManager eton;
            return eton;
        }
        void addLogger(Logger::ptr &logger)
        {
            if(hasLogger(logger->name())) return;
            std::unique_lock<std::mutex> lock(_mutex);
            _loggers.insert(std::make_pair(logger->name(), logger));
        }
        bool hasLogger(const std::string name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if(it == _loggers.end())
            {
                return false;
            }
            return true;
        }
        Logger::ptr getLogger(const std::string name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _loggers.find(name);
            if(it == _loggers.end())
            {
                return Logger::ptr();
            }
            return it->second;
        }
        Logger::ptr rootLogger()
        {
            return _root_logger;
        }
    private:
        // 构造函数私有化
        LoggerManager()
        {
            std::unique_ptr<LOG::LoggerBuilder> builder(new LOG::LocalLoggerBuilder());
            builder->buildLoggerName("root");
            _root_logger = builder->build();
            _loggers.insert(std::make_pair("root", _root_logger));
        }
    private:
        std::mutex _mutex;
        Logger::ptr _root_logger; // 默认日志器
        std::unordered_map<std::string, Logger::ptr> _loggers; // 日志器数组;
    };

    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(_logger_name.empty() == false);
            if (_formatter.get() == nullptr)
            {
                _formatter = std::make_shared<Formatter>();
            }

            if (_sinks.empty())
            {
                buildSink<StdOutSink>();
            }

            Logger::ptr logger;
            if (_logger_type == LoggerType::LOGGER_ASYNC)
            {
                logger = std::make_shared<AsyncLogger>(_logger_name, _limit_level, _formatter, _sinks, _looper_type);
            }
            else
            {
                logger = std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks);
            }
            LoggerManager::getInstance().addLogger(logger);
            return logger;
        }
    };
}

#endif

