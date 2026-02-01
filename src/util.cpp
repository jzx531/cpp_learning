// util.c
#include "util.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #define _POSIX_C_SOURCE 199309L
    #include <time.h>
    #include <unistd.h>
#endif

void sleep_milli(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);  // Windows API, 单位：毫秒
#else
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}