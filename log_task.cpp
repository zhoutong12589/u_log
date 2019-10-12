#include "log_task.h"


namespace COMM
{

CLog_task::CLog_task(char* msg)
{
    m_msg = msg;
}

CLog_task::~CLog_task()
{

}
    

int CLog_task::run()
{
    if(nullptr != m_msg)
    {
        CLogging::GetInstance()->write_str(m_msg);

        return 0;
    }
    else
    {
        return -1;
    }
    
}


}

