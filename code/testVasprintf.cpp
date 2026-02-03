#include <stdio.h>
#include <stdarg.h>
#include <cstdlib>

void myprintf(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    char* res;
    int ret = vasprintf(&res, format, ap);
    if(ret != -1)
    {
        printf(res);
        free(res); // res指向动态开辟的空间，需要手动释放
    }
    va_end(ap);
}

//vasprintf是c库函数，通过可变参数创建一个格式化的字符串，并将其存储在动态分配的内存中，其使用方法与printf类似
// 但其不会将结果打印到标准输出中而是存储在字符数组指针中

int main()
{
    myprintf("%s-%d\n", "huaxiangyun", 100);
    return 0;
}

