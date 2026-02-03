#ifndef __M_SINK_H__
#define __M_SINK_H__

#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

namespace LOG {

// 前向声明（如果需要）
class LogSink;

// 日志落地器的智能指针类型
using LogSinkPtr = std::shared_ptr<LogSink>;

// 抽象日志落地器基类
class LogSink {
public:
    using ptr = std::shared_ptr<LogSink>;
    virtual ~LogSink() = default;

    // 纯虚函数：将日志数据写入目标位置
    virtual void log(const char* data, size_t len) = 0;
};

// ==================== 具体 Sink 实现 ====================

// 标准输出 Sink
class StdOutSink : public LogSink {
public:
    void log(const char* data, size_t len) override {
        std::cout.write(data, len);
        std::cout.flush();
    }
};

// 标准错误输出 Sink
class StdErrSink : public LogSink {
public:
    void log(const char* data, size_t len) override {
        std::cerr.write(data, len);
        std::cerr.flush();
    }
};

// 文件输出 Sink
class FileSink : public LogSink {
public:
    explicit FileSink(const std::string& filepath)
        : _filepath(filepath), _ofs(_filepath, std::ios::app) {
        if (!_ofs.is_open()) {
            // 可选：记录错误或抛异常
            std::cerr << "Failed to open log file: " << _filepath << std::endl;
        }
    }

    void log(const char* data, size_t len) override {
        std::lock_guard<std::mutex> lock(_mutex); // 多线程安全
        if (_ofs.is_open()) {
            _ofs.write(data, len);
            _ofs.flush();
        }
    }

private:
    std::string _filepath;
    std::ofstream _ofs;
    std::mutex _mutex; // 保证多线程写入安全
};

// ==================== Sink 工厂类 ====================

class SinkFactory {
public:
    template<typename SinkType, typename... Args>
    static LogSink::ptr create(Args&&... args) {
        return std::make_shared<SinkType>(std::forward<Args>(args)...);
    }
};

} // namespace LOG

#endif // __M_SINK_H__

