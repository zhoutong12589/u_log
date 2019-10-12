#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "task_I.h"
#include "queue_T.h"

#ifndef _H_WORKER_
#define _H_WORKER_

namespace COMM
{

using namespace std;

//运行线程的基类，如果想打造一个缓冲队列，则继承该类，可以重写run，来达到特殊的目的
//添加的任务都继承自CTask_I，在构造函数中实现数据的拷贝，实现run来实际执行
class CWorker
{
public:
    CWorker();
    virtual ~CWorker();

    void start(int thrnum = 1);
    void stop();

    int put(CTask_I* t);
private:
    virtual void run();

    bool               m_run_flag;
    CQueue_T<CTask_I>  m_queue;
    vector<thread*>    m_vth;  
    
};


} // namespace COMM

#endif //_H_WORKER_
