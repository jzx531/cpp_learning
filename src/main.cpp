#include "log.h"
void log_test()
{
    DEBUG("%s", "测试日志");
    INFO("%s", "测试日志");
    WARN("%s", "测试日志");
    ERROR("%s", "测试日志");
    FATAL("%s", "测试日志");

    size_t count = 0;
    while(count < 300000)
    {
        WARN("测试日志-%d", count++);
    }
}
int main()
{
	log_test();
	return 0;
}