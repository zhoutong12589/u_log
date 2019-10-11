
#include <iostream>
#include "logging.h"
#include "queue_T.h"
#include "configurator.h"
#include <unistd.h>

using namespace std;
using namespace COMM;

int main()
{
    cout<<"start"<<endl;
    CLogging::GetInstance()->init();

    CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_INFO, "%s" , "test1");
    CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_INFO, "%s" , "test2");

    int count = 1;
    while(1)
    {
        cout<<count<<endl;
        //CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_INFO, "%d" , count++);
        
        ILOG("%s- %d:%s", "333", 345, "677");
        sleep(1);
        
    }
    return 0;
}