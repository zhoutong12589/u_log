#include <iostream>
#include <list>
#include <thread>
#include <mutex>

#ifndef _H_QUEUE_T_
#define _H_QUEUE_T_

namespace COMM
{

using namespace std;



mutex g_queue_mutex;    

//队列模板
template<class T>
class CQueue_T
{
public:
    CQueue_T(){}
    ~CQueue_T(){}
    int put(T* t)
    {
        //提供线程互斥
        std::lock_guard<std::mutex> lock(g_queue_mutex);
        m_queue.push_back(t);
        return 0;
    };
    T* get()
    {
        std::lock_guard<std::mutex> lock(g_queue_mutex);
        T* t = m_queue.pop_front();
        return t;
    };
    int length()
    {
        return m_queue.size();
    };

private:
    list<T*> m_queue;
};


}

#endif //_H_QUEUE_T_