#include <iostream>
#include "worker.h"

#ifndef _H_LOG_WORKER_
#define _H_LOG_WORKER_

namespace COMM
{

//log的处理队列
class CLog_worker : public CWorker
{
public:
    ~CLog_worker();
    static CLog_worker* GetInstance();
    void open();

private:
    CLog_worker();
    static CLog_worker*        g_log_worker;
};


} //namespace COMM



#endif //_H_LOG_WORKER_
