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
