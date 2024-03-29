#include <iostream>
#include "task_I.h"
#include "logging.h"
#include "log_worker.h"

#ifndef _H_LOG_TASK_
#define _H_LOG_TASK_

namespace COMM
{

using namespace std;

//继承自task，将每条日志包装为一个任务
class CLog_task : public CTask_I
{
public:
    CLog_task(char* msg);
    virtual ~CLog_task();
    

    virtual int run();
private:
    char* m_msg;
};


//方便日常使用的宏定义，pt标识是否打印到屏幕
#define FLOGT(pt, format, args...) do{ \
    char* msg = CLogging::GetInstance()->strformat(__FILE__, __LINE__, LOG_FATAL, format, args ); \
    if(pt){ cout<<msg; } \
    CLog_task* task = new CLog_task(msg); \
    CLog_worker::GetInstance()->put(task); \
}while(0); 

#define ELOGT(pt, format, args...) do{ \
    char* msg = CLogging::GetInstance()->strformat(__FILE__, __LINE__, LOG_ERROR, format, args ); \
    if(pt){ cout<<msg; } \
    CLog_task* task = new CLog_task(msg); \
    CLog_worker::GetInstance()->put(task); \
}while(0); 

#define WLOGT(pt, format, args...) do{ \
    char* msg = CLogging::GetInstance()->strformat(__FILE__, __LINE__, LOG_WARN, format, args ); \
    if(pt){ cout<<msg; } \
    CLog_task* task = new CLog_task(msg); \
    CLog_worker::GetInstance()->put(task); \
}while(0); 

#define ILOGT(pt, format, args...) do{ \
    char* msg = CLogging::GetInstance()->strformat(__FILE__, __LINE__, LOG_INFO, format, args ); \
    if(pt){ cout<<msg; } \
    CLog_task* task = new CLog_task(msg); \
    CLog_worker::GetInstance()->put(task); \
}while(0); 

#define DLOGT(pt, format, args...) do{ \
    char* msg = CLogging::GetInstance()->strformat(__FILE__, __LINE__, LOG_DEBUG, format, args ); \
    if(pt){ cout<<msg; } \
    CLog_task* task = new CLog_task(msg); \
    CLog_worker::GetInstance()->put(task); \
}while(0); 

#define NLOGT(pt, format, args...) do{ \
    char* msg = CLogging::GetInstance()->strformat(__FILE__, __LINE__, LOG_NONE, format, args ); \
    if(pt){ cout<<msg; } \
    CLog_task* task = new CLog_task(msg); \
    CLog_worker::GetInstance()->put(task); \
}while(0); 
} //namespace COMM

#endif //_H_LOG_TASK_



