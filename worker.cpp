#include "worker.h"
#include <unistd.h>

namespace COMM
{

using namespace std;

CWorker::CWorker()
{
    m_run_flag = false;
}
CWorker::~CWorker()
{

}

void CWorker::start(int thrnum)
{
    m_run_flag = true;
    for(int n = 0; n < thrnum; ++n)
    {
        thread *thr = new thread(std::bind(&CWorker::run, this));
        m_vth.push_back(thr);
    }
    
}
void CWorker::stop()
{
    m_run_flag = false;
    int num = m_vth.size();
    for(int n = num - 1; n >= 0; --n)
    {
        thread *thr = m_vth[n];
        m_vth.pop_back();

        thr->join();
        delete thr;
    }
}

int CWorker::put(CTask_I* t)
{
    m_queue.put(t);
    return 0;
}

void CWorker::run()
{
    while(m_run_flag)
    {
        CTask_I* t = m_queue.get();
        if(nullptr == t)
        {
            sleep(1);
        }
        else
        {
            int ret = t->run();
            if(1 == ret)
            {
                m_queue.put(t);
            }
        }
        
    }
}



} // namespace COMM

