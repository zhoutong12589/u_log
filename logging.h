#include <iostream>
#include <stdio.h>
#include <list>
#include <string>
#include <assert.h>

#ifndef _H_LOGGING_
#define _H_LOGGING_

namespace COMM
{

using namespace std;

//日志级别
enum CLogging_level
{
    LOG_FATAL = 1,      //关键错误，写日志并调用assert
    LOG_ERROR = 100,    //错误，一般是需要人工处理的错误才用这个级别，但是程序可以继续运行
    LOG_WARN = 200,     //警告，一般错误，不影响程序运行，但是认为是外部引起的异常，不需要处理，需要注意
    LOG_INFO = 300,     //一般信息
    LOG_DEBUG = 400,    //调试信息
    LOG_NONE = 500      //最低级别，一般不使用
};
//字符串化日志级别
char* STR_LEVEL(CLogging_level l);


//日志的文件切分模式
enum CLogging_split_mode
{
    SPLIT_HOURS,
    SPLIT_DAY,
    SPLIT_SIZE
};

class CLogging
{
public:
    ~CLogging();
    static CLogging* GetInstance();

    //初始化配置文件，必须调用，可以不给配置文件，使用默认参数，之后才可以通过set函数来改变各个参数
    int init(const char *cfg_file = nullptr);

    //设置日志级别，可随时改变
    int set_level(CLogging_level level = LOG_INFO);

    //设置日志的最大保存数量
    int set_filenum(int num = 16);

    //设置日志路径，只允许在初始化时调用
    int set_logpath(const char* path = (char*)".");

    //设置日志路径，只允许在初始化时调用
    int set_logname(const char* name = (char*)".");

    //设置切分模式，默认为按小时分割，此时切分而文件大小参数即使设置了也无效
    //在设置了切分模式为SPLIT_SIZE时，size参数才有效，默认10M
    //只允许在初始化时调用
    int set_split_mode(CLogging_split_mode mode = SPLIT_HOURS, int size = 10*1024*1024);

    //非线程安全的写日志，每次启动都创建新的文件
    //目前每次都有fflush，对于效率的影响暂时未知
    void write(const char* file, int line, CLogging_level level, const char* format, ...);

    //线程安全的写日志
    void write_s(const char* file, int line, CLogging_level level, const char* format, ...);

private:
    CLogging();
    
    //根据日志模式判断日志是否需要更新
    bool judge_mode(int hour, int mins);
    //清理过期文件，num指定最大数量，在启动时要检查的数量为最大数量-1，因为马上要创建新的文件
    void clean(unsigned int num);
    //在初始化时获取配置的文件夹下的日志文件的数量和最新文件的大小
    int get_info();

    static const int MSG_SZ;
    
    static CLogging*        g_logging;
    CLogging_level          m_level;           //日志级别
    char                    m_path[256];       //日志路径
    char                    m_name[256];       //日志文件名
    CLogging_split_mode     m_mode;            //日志的切分模式
    int                     m_size;            //在使用SPLIT_SIZE模式时，文件的大小
    unsigned int            m_filenum;         //日志文件的保存数量
    bool                    m_jduge_flag;      //文件更新标识，只有在false时，才能更新文件
    list<string>            m_vfiles;          //目前保存的文件名，用于在删除最开始的文件时快速超找文件名
    
    FILE*                   m_file;            //日志文件的句柄
    int                     m_currentsize;        //目前文件的大小，根据初始时获取的大小和写入大小累加获得
};

//方便使用的宏定义
#define FLOG(format, args...) do{ \
    CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_FATAL, format, args ); \
    assert(0); \
}while(0); 

#define ELOG(format, args...) do{ \
    CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_ERROR, format, args ); \
}while(0); 

#define WLOG(format, args...) do{ \
    CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_WARN, format, args ); \
}while(0); 

#define ILOG(format, args...) do{ \
    CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_INFO, format, args ); \
}while(0); 

#define DLOG(format, args...) do{ \
    CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_DEBUG, format, args ); \
}while(0); 

#define NLOG(format, args...) do{ \
    CLogging::GetInstance()->write(__FILE__, __LINE__, LOG_NONE, format, args ); \
}while(0); 

} // namespace COMM




#endif //_H_LOGGING_
