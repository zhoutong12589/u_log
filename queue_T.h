#include <iostream>
#include <list>
#include <thread>
#include <mutex>

#ifndef _H_QUEUE_T_
#define _H_QUEUE_T_

namespace COMM
{

using namespace std;



static mutex g_queue_mutex;    

//队列模板
template<class T>
class CQueue_T
{
public:
    CQueue_T()
    {
        m_max_len = 1024;
    }
    ~CQueue_T(){}
    int put(T* t)
    {
        //提供线程互斥
        std::lock_guard<std::mutex> lock(g_queue_mutex);
        if(m_queue.size() < m_max_len)
        {
            m_queue.push_back(t);
            return 0;
        }
        else
        {
            return -1;
        }
    };
    T* get()
    {
        std::lock_guard<std::mutex> lock(g_queue_mutex);
        if(m_queue.size() > 0)
        {
            T* t = m_queue.front();
            m_queue.pop_front();
            return t;
        }
        else
        {
            return nullptr;
        }
        
    };
    int length()
    {
        std::lock_guard<std::mutex> lock(g_queue_mutex);
        return m_queue.size();
    };
    
    int set_max_len(int max = 1024)
    {
        m_max_len = max;

        return 0;
    }
private:
    list<T*> m_queue;
    int m_max_len;        //队列最大长度
};


}

#endif //_H_QUEUE_T_