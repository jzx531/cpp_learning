#include "logger.hpp"

void log_test()
{
    LOG::Logger::ptr logger = LOG::LoggerManager::getInstance().getLogger("async_logger");
    logger->debug(__FILE__, __LINE__, "%s", "测试日志");
    logger->info(__FILE__, __LINE__, "%s", "测试日志");
    logger->warn(__FILE__, __LINE__, "%s", "测试日志");
    logger->error(__FILE__, __LINE__, "%s", "测试日志");
    logger->fatal(__FILE__, __LINE__, "%s", "测试日志");

    size_t count = 0;
    while(count < 300000)
    {   
        logger->fatal(__FILE__, __LINE__, "测试日志-%d", count++);
    }
}
int main()
{   
    std::unique_ptr<LOG::LoggerBuilder> builder(new LOG::GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger");
    builder->buildLoggerLevel(LOG::LogLevel::value::WARN);
    builder->buildFormatter("[%c]%m%n");
    builder->buildLoggerType(LOG::LoggerType::LOGGER_ASYNC);
    builder->buildEnableUnSafeAsync();
    builder->buildSink<LOG::FileSink>("./logfile/async.log");
    builder->buildSink<LOG::StdOutSink>();
    builder->build();
    log_test();
    return 0;
}