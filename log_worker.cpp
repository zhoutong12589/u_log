#include "log_worker.h"


namespace COMM
{

CLog_worker* CLog_worker::g_log_worker = new CLog_worker;

CLog_worker::CLog_worker()
{

}

CLog_worker::~CLog_worker()
{
    
}


CLog_worker* CLog_worker::GetInstance()
{
    return g_log_worker;
}

void CLog_worker::open()
{
    start(1);
}

}

