#include <cstdio>

#define LOG(fmt, ...) \
    std::fprintf(stderr, "[%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

int main() {
    LOG("日志消息");               // 无额外参数
    LOG("x = %d, name = %s", 42, "Alice"); // 有额外参数
    return 0;
}