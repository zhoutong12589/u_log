
#include <iostream>
#include "logging.h"
#include "queue_T.h"
#include "configurator.h"
#include "task_I.h"
#include "worker.h"
#include <unistd.h>
#include "log_worker.h"
#include "log_task.h"
#include <stdio.h>

using namespace std;
using namespace COMM;


class wite_log : public CTask_I
{
public:
    wite_log(){

    }
    ~wite_log(){

    }

    virtual int run()
    {
        cout<<"wite_log run"<<endl;
        sleep(1);

        return 0;
    }
};


int main()
{
    cout<<"start"<<endl;

    //初始化日志
    CLogging::GetInstance()->init();

    //最直接的模式写日志
    CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_INFO, "%d %s", 12, "444");
    //使用宏定义写
    ILOG("%d %s", 12, "444");
    WLOG("%08d", 4654);
    //开启日志的队列
    CLog_worker::GetInstance()->open();
    //获取日志字符串
    char* msg = CLogging::GetInstance()->strformat(__FILE__, __LINE__, LOG_INFO, "%d %s", 12, "444");
    cout<<msg<<endl;
    //向队列中写入

    int count  = 0;
    while(1)
    {
        cout<<"111"<<endl;
        ILOGT("%d", count++);
        sleep(1);
    }
    

    sleep(1);
    return 0;
}