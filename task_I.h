#include <iostream>
#include <thread>
#include <mutex>


#ifndef _H_TASK_I_
#define _H_TASK_I_

namespace COMM
{

using namespace std;

//任务接口，I结尾标识为接口
class CTask_I
{
public:
    CTask_I()
    {
    }
    virtual ~CTask_I()
    {

    }
    //返回值为1时，标识再次加入执行
    virtual int run() = 0;
};


} // namespace COMM

#endif //_H_TASK_I_
