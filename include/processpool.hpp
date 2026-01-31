#ifndef PROCESS_POOL_H
#define PROCESS_POOL_H

#include<iostream>
#include<unistd.h>
#include<vector>
#include<sys/wait.h>
#include<sys/types.h>
#include<cstdlib>
#include<time.h>
#include"task.hpp"
#define EXIT_FAILURE 1
#define missionnum 4

class channel
{
    public:
     int _processid;
     int _write_fd;
     channel(int &processid,int &write_fd)
     :_processid(processid)
     ,_write_fd(write_fd)
     {}
};

void mission_load();
void start_mission();
void process_control();
void process_clean();
void processpool_init();

#endif
