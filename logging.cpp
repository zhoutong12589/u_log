#include "logging.h"
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/time.h>
#include <libgen.h>
#include <sys/types.h>
#include <dirent.h>
#include <thread>
#include <mutex>
#include <unistd.h>

namespace COMM
{
//字符串化
#define STR1(R)  #R

CLogging* CLogging::g_logging = new CLogging;      //初始化static变量
const int CLogging::MSG_SZ = 512;                  //目前允许的一条日志的最大长度
static mutex g_log_mutex;                          // 线程互斥锁

char* STR_LEVEL(CLogging_level l){
    if(l == LOG_FATAL) 
        return (char*)"FATAL";
    else if(l == LOG_ERROR) 
        return  (char*)"ERROR"; 
    else if(l == LOG_WARN) 
        return  (char*)"WARN"; 
    else if(l == LOG_INFO) 
        return  (char*)"INFO";
    else if(l == LOG_DEBUG) 
        return  (char*)"DEBUG"; 
    else if(l == LOG_NONE) 
        return  (char*)"NONE"; 
    else 
        return  (char*)"N";
};

CLogging::CLogging()
{
    m_file = nullptr;
    m_currentsize = 0;
    m_jduge_flag = false;



}

CLogging::~CLogging()
{
    if(nullptr != m_file)
    {
        fclose(m_file);
    }
}

CLogging* CLogging::GetInstance()
{
    return g_logging;
}

//初始化配置文件
int CLogging::init()
{
    //给定默认配置
    m_level = LOG_INFO;
    strcpy(m_path, ".");

    //获取程序名
    char nameBuf[512] = {0};
    if( readlink("/proc/self/exe", nameBuf,512) <=0 )
    {
            return -1;
    }
    char *name = basename(nameBuf);
    sprintf(m_name, "%s.log", name);
    
    m_mode = SPLIT_HOURS;
    m_size = 10*1024*1024;
    m_filenum = 16;

    //根据配置的m_path获取文件夹下有哪些文件
    get_info();
    
    return 0;
}

//设置日志级别
int CLogging::set_level(CLogging_level level)
{
    m_level = level;
    return 0;
}

int CLogging::set_filenum(int num)
{
    m_filenum = num;
    clean(num);
    return 0;
}

//设置日志路径
int CLogging::set_logpath(const char* path)
{
    strcpy(m_path, path);
    get_info();
    return 0;
}

//设置日志路径
int CLogging::set_logname(const char* name)
{
    strcpy(m_name, name);
    get_info();
    return 0;
}

//设置切分模式，默认为按小时分割
int CLogging::set_split_mode(CLogging_split_mode mode, int size)
{
    m_mode = mode;
    if(size <= 1024*1024)
    {
        m_size = 1024* 1024;
    }
    else
    {
        m_size = size;
    }
    
    return 0;
}

void CLogging::write(const char* file, int line, CLogging_level level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    
    write(file, line, level, format, args);

    va_end(args);
}

//非线程安全的写日志
void CLogging::write(const char* file, int line, CLogging_level level, const char* format, va_list args)
{
    //1、获取当前时间
    struct tm ts;
    suseconds_t usec;
    int ret = get_time(ts, usec);
    if(0 != ret)
    {
        return;
    }
    //2、创建文件
    ret = create_file(ts);  
    if(0 != ret)
    {
        return;
    }
    //3、日志内容写入字符串
    char msg[MSG_SZ];
    vsnprintf(msg, MSG_SZ, format, args);


    //4、获取文件名
    char file_name[128];
    strcpy(file_name, file);
    char* name = basename(file_name);
    
    //5、格式化写入文件
    ret = fprintf(m_file, "[%4d-%02d-%02d %02d:%02d:%02d,%03d - %s:%04d - %5s] %s\n", 
                        ts.tm_year + 1900, ts.tm_mon + 1, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec,
                        int(usec / 1000), name, line, STR_LEVEL(level), msg);
    if (ret < 0)
    {
        cout<<"CLogging::write vfprintf error="<<errno<<endl;
        return;
    }
    fflush(m_file);
    
    //6、根据累计写入大小
    m_currentsize += ret;

}

//线程安全的写日志
void CLogging::write_s(const char* file, int line, CLogging_level level, const char* format, ...)
{
    //提供线程互斥
    std::lock_guard<std::mutex> lock(g_log_mutex);
    va_list args;

    va_start(args, format);
    write(file, line, level, format, args);
    va_end(args);
    
}

void CLogging::write_str(const char* msg)
{
    //1、获取当前时间
    struct tm ts;
    suseconds_t usec;
    int ret = get_time(ts, usec);
    if(0 != ret)
    {
        return;
    }
    //2、创建文件
    ret = create_file(ts);  
    if(0 != ret)
    {
        return;
    }

    ret = fwrite(msg, 1, strlen(msg), m_file);
    delete msg;
    if(ret < 0)
    {
        cout<<"CLogging::write_str fwrite error="<<errno<<endl;
        return;
    }
    m_currentsize += ret;
    fflush(m_file);
}

//格式化字符串
char* CLogging::strformat(const char* file, int line, CLogging_level level, const char* format, ...)
{
    //1、获取当前时间
    struct tm ts;
    suseconds_t usec;
    int ret = get_time(ts, usec);
    if(0 != ret)
    {
        return nullptr;
    }

    //2、获取文件名
    char file_name[128];
    strcpy(file_name, file);
    char* name = basename(file_name);

    //3、将日志的头部信息写入字符串
    char* msg = new char[MSG_SZ];
    int len1 = snprintf(msg, MSG_SZ, "[%4d-%02d-%02d %02d:%02d:%02d,%03d - %s:%04d - %5s] ", 
                        ts.tm_year + 1900, ts.tm_mon + 1, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec,
                        int(usec / 1000), name, line, STR_LEVEL(level) );
    if(len1 < 0)
    {
        delete msg;
        return nullptr;
    }
    
    //4、将日志内容拼接到后面
    va_list args;

    va_start(args, format);
    int len2 = vsnprintf(msg + len1, MSG_SZ - ret, format, args);
    va_end(args);

    if(len2 < 0)
    {
        delete msg;
        return nullptr;
    }
    
    int len = len1 + len2;
    msg[len] = '\n';
    msg[len + 1] = '\0';
    return msg;
}

bool CLogging::judge_mode(int hour, int mins)
{
    bool flag = false;
    if(SPLIT_HOURS == m_mode)
    {
        //按小时分割，每个整数小时的开始进行新的日志文件的创建
        if(0 == mins)
        {
            m_currentsize = 0;
            flag = true;
        }
    }
    else if(SPLIT_DAY == m_mode)
    {
        if(0 == hour)
        {
            m_currentsize = 0;
            flag = true;
        }
    }
    else if(SPLIT_SIZE == m_mode)
    {
        if(m_currentsize >= m_size)
        {
            //重置文件大小累加器
            m_currentsize = 0;
            flag = true;
        }
    }
    
    //判断是否真的需要更新，只有在不需要更新的情况下才将m_jduge_flag置为false
    if(!flag)
    {
        m_jduge_flag = false;
    }

    //只有在需要更新的flag状态改变过后才确认
    if(!m_jduge_flag && flag)
    {
        m_jduge_flag = true;
        return true;
    }
    else
    {
        return false;
    }
    
}

void CLogging::clean(unsigned int num)
{
    while(m_vfiles.size() > 0)
    {
        if(m_vfiles.size() > num)
        {
            //保存文件数量大于最大保存数量+1
            //获取第一个文件
            string f = m_vfiles.front();
            m_vfiles.pop_front();
            
            char file_name[512];
            int len = strlen(m_path);
            if('/' == m_path[len - 1])
            {
                sprintf(file_name, "%s%s", m_path, f.c_str());
            }
            else
            {
                sprintf(file_name, "%s/%s", m_path, f.c_str());
            }
            remove(file_name);
        }
        else
        {
            break;
        }
    }

    
}

int CLogging::get_info()
{
    //首先清除历史数据
    m_vfiles.clear();

    //读取文件夹的结构
    DIR * dir;
    struct dirent * ptr;

    //获取文件夹下文件名集合
    dir = opendir(m_path); //打开一个目录
    while((ptr = readdir(dir)) != NULL) //循环读取目录数据
    {
        //只有以m_name开头的文件才会放入m_vfiles
        int res = strncmp(m_name, ptr->d_name, strlen(m_name));
        if(0 == res)
        {
            //格式化全路径
            char file_name[256];
            int len = strlen(m_path);
            if('/' == m_path[len - 1])
            {
                sprintf(file_name, "%s%s", m_path, ptr->d_name);
            }
            else
            {
                sprintf(file_name, "%s/%s", m_path, ptr->d_name);
            }
            m_vfiles.push_back(file_name);
        }
    }
    closedir(dir);//关闭目录指针
    
    //对文件进行排序，好确认哪个文件时最近生成的
    m_vfiles.sort();

    //在获取文件后对文件进行数量限制和清理，此处-1是因为马上要创建一个新的文件
    clean(m_filenum);

    return 0;
}


int CLogging::create_file(const struct tm ts)
{
    //根据文件模式检查是否写新文件
    if(judge_mode(ts.tm_hour, ts.tm_min))
    {
        //关闭文件句柄，下次写入新的文件
        fclose(m_file);
        m_file = nullptr;
        //清理过期文件
        clean(m_filenum);
    }

    if(nullptr == m_file)
    {
        //创建文件名需要添加日期后缀
        char last_name[64];
        sprintf(last_name, "%4d_%02d_%02d_%02d_%02d_%02d", ts.tm_year + 1900, ts.tm_mon + 1, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec);

        char file_name[512];
        int len = strlen(m_path);
        if('/' == m_path[len - 1])
        {
            sprintf(file_name, "%s%s.%s", m_path, m_name, last_name);
        }
        else
        {
            sprintf(file_name, "%s/%s.%s", m_path, m_name, last_name);
        }
        //将文件放入m_vfiles，以记录目前保留了多少文件
        m_vfiles.push_back(file_name);

        m_file = fopen(file_name, "a");
        if(NULL == m_file)
        {
            cout<<"CLogging::write fopen error="<<errno<<endl;
            return -1;
        }
    }

    return 0;
}

int CLogging::get_time(struct tm &ts, suseconds_t &usec)
{
    //尝试两次获取，如果两次都不成功，则标识失败
    struct timeval tv;
    int ret= gettimeofday(&tv, NULL);
    if(0 != ret)
    {
        ret= gettimeofday(&tv, NULL);
        if(0 != ret)
        {
            cout<<"CLogging::write gettimeofday1 error="<<errno<<endl;
            return -1; 
        }
        
    }
    //struct tm ts;
    localtime_r(&tv.tv_sec, &ts);
    usec = tv.tv_usec;
    return 0;
}


}
