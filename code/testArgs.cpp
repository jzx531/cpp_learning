#include <stdio.h>
#include <stdarg.h>

void printNum(int count, ...)
{
    va_list ap;
    va_start(ap, count); // 初始化ap指针，使其指向不定参数列表的第一个参数
    for(int i = 0; i < count; i++)
    {
        int num = va_arg(ap, int); // 从不定参数列表中抽取int类型的参数
        printf("%d ", num);
    }
    printf("\n");
    va_end(ap); // 将ap置空
}

int main()
{
    printNum(5, 1, 2, 3, 4, 5);
    return 0;
}