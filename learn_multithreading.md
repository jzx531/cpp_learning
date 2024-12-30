# c++ multithreading

[TOC]
```c++
typedef NTSTATUS(NTAPI* PNtQuerySystemInformation)(
    IN HANDLE ProcessHandle,
    IN PROCESSINFOCLASS ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength OPTIONAL
);
```
定义了一个hook函数指针，用来获取系统信息。IN为宏定义控制编译,PROCESSINFOCLASS为枚举类型，