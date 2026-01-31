#include "processpool.hpp"
#include "task.hpp"

const int processnum=10;
std::vector<mission> missionarray;
std::vector<channel> channelarray;

void mission_load()
{
        missionarray.push_back(task1);
        missionarray.push_back(task2);
        missionarray.push_back(task3);
        missionarray.push_back(task4);
}

void processpool_init()
{
    for (size_t i = 0; i < processnum; i++)
    {
       int pipefd[2];
       int n = pipe(pipefd);
       if(n<0)
       {
        //创建管道失败
          perror("pipe fail");
          exit(EXIT_FAILURE);
       }
       int id=fork();
       if(id<0){
        //fork 失败
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        exit(EXIT_FAILURE);
       }
       //子进程
       if(id==0)
       {
         close(pipefd[1]);//关闭子线程写
         dup2(pipefd[0],0);//将子进程的标准输入（stdin，文件描述符 0）重定向到管道的读端（pipefd[0]），使得子进程后续通过标准输入读取数据时，实际上是从父进程写入的管道中读取。
         start_mission();
         exit(0);
       }
       //父进程
       close(pipefd[0]);//关闭父进程读
       channelarray.push_back(channel(id,pipefd[1]));//将子进程和其readfd保存进channel
    }
    
}

void start_mission()
{
    while(true)
    {
        int status;
        int n = read(0,&status,sizeof(int));//读取执行指令到status
        if(n==sizeof(int))
        {
            if(status>=0&&status<missionnum)
            {
                std::cout<<"子进程"<<getpid()<<" 成功获取到任务码"<<status<<std::endl;
                missionarray[status]();
            }
        }else if(n==0)
        {
            break;
        }else if(n<0)
        {
            //读取失败
            perror("read");
            exit(EXIT_FAILURE);
        }
    }
}

void process_control()
{
    srand((unsigned int)time(NULL));
    for(int i=0;i<100;++i)
    {
        int cmd = rand()%missionnum;
        int n=write(channelarray[i%processnum]._write_fd,&cmd,sizeof(int));
        if(n < 0){
            perror("write");
            exit(EXIT_FAILURE);
        }
        std::cout<<"father process send to"<<channelarray[i%processnum]._processid<<std::endl;
    }
}

void process_clean()
{
    for(int i=channelarray.size()-1;i>=0;i--)
    {
        close(channelarray[i]._write_fd);
        int statues;
        int n=waitpid(channelarray[i]._processid,&statues,0);
        if(n<0)
        {
            perror("waitpid");
        }else
        {
            std::cout<<"子进程"<<channelarray[i]._processid<<"等待成功"<<std::endl;
        }
    }
}