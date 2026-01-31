#ifndef TASK_H
#define TASK_H

typedef void (*mission)() ;

#include <iostream>
#include<unistd.h>
#include<vector>
#include<sys/wait.h>
#include<sys/types.h>
#include<cstdlib>
#include<time.h>

void task1();
void task2();
void task3();
void task4();


#endif

