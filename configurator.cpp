#include "configurator.h"    
#include <stdio.h>
#include <string.h>
#include <unistd.h>

namespace COMM
{

CConfigurator::CConfigurator(const string&  initFileName)
{
    m_FileName = initFileName;

    //设置空格标识
    int pos = int(' ');
    whiteSpace[pos] = 1;
    pos = int('\t');
    whiteSpace[pos] = 1;
    pos = int('\r');
    whiteSpace[pos] = 1;
    pos = int('\n');
    whiteSpace[pos] = 1;
}

CConfigurator::~CConfigurator()
{

}
    
//
int CConfigurator::get_conf(map<string, string> &cfg)
{
    //读取配置文件的参数
    FILE *fp;
    fp = fopen(m_FileName.c_str(), "r");
    if (fp == NULL)
    {
        printf("File %s does not exist\n", m_FileName.c_str());
        return -1;
    }

    //现阶段默认的一行的长度不能超过256，每行都是完整的key = value
    //只能在注释中出现中文，否则可能出现越界的错误
    size_t sz = 256;
    char* line = NULL;
    char  command[256];
    char  key[256];
    char  value[256];
    int   count = 1;
    //每次读取一行，以\n为换行符，最长256
    int len = getline(&line, &sz, fp);
    while (len != -1) 
    {
        //1、找到#的位置，即认为后面的都是注释
        //只能在注释中出现中文，这也是为什么先判断注释
        //如果直接给中文去前后的空白，可能出现问题
        int npos = -1;
        for(int n = 0; n < strlen(line); ++n)
        {
            if(line[n] == '#')
            {
                npos = n;
                break;
            }
        }
        if(-1 != npos) //找到了#，并给出位置
        {
            line[npos] = '\0';
            //如果开头就是#，则表明整行都是注释
            if(0 == strlen(line))
            {
                count++;
                len = getline(&line, &sz, fp);
                continue;
            }
        }

        //2、去掉空行
        int nl = trim(command, 256, line, len);
        if(0 == nl)
        {
            //读取到空行，读取下一行
            count++;
            len = getline(&line, &sz, fp);
            continue;
        }

        //3、commad中的应该都是格式正确
        npos = -1;
        for(int n = 0; n < strlen(command); ++n)
        {
            if(command[n] == '=')
            {
                npos = n;
                break;
            }
        }
        if(-1 == npos)
        {
            //没有找到=，格式不正确
            printf("config file format error line %d\n", count);
            fclose(fp);
            return -1;
        }
        else
        {
            //找到正确的配置项
            int nk = trim(key, 256, command, npos);
            if(nk <= 0)
            {
                printf("config file format error line %d, no key\n", count);
                fclose(fp);
                return -1;
            }
            int nv = trim(value, 256, command + npos + 1, strlen(command) - npos - 1);
            if(nv <= 0)
            {
                printf("config file format error line %d, no value\n", count);
                fclose(fp);
                return -1;
            }

            //认为key和value已经是正确的了
            //printf("%s = %s\n", key, value);
            cfg.insert(make_pair(key, value));

        }
        
        len = getline(&line, &sz, fp);
        count++;
    }
    free(line);

    fclose(fp);
    return 0;
}

int CConfigurator::trim(char* dst, int dst_len, char* src, int src_len) 
{
    unsigned int pos = 0;
    //去掉前面的空白
    int start = 0;
    for(;start < src_len; ++start)
    {
        pos = int(src[start]);
        if(whiteSpace[pos] != 1)
        {
            break;
        }
    }

    //去掉后面的空白
    int end = src_len - 1;
    for(;(end >= 0) && (end >= start); --end)
    {
        pos = (unsigned int)(src[end]);
        if(whiteSpace[pos] != 1)
        {
            break;
        }
    }
    strncpy(dst, src + start, end - start + 1);
    dst[end - start + 1] = '\0';

    return strlen(dst);
}

}

/*


CConfigurator::CConfigurator()
{
}
CConfigurator::CConfigurator(const std::string&  initFileName)
{   
    m_initFileName = initFileName;
}

CConfigurator::~CConfigurator()
{

}

int CConfigurator::init()
{
    //给定初始化参数
    int ret = set_default();
    if(-1 == ret)
    {
        return -1;
    }
    //读取配置文件
    if(m_initFileName.empty())
    {
        return 1;
    }

    FILE *fp;
    fp = fopen(m_initFileName.c_str(), "r");
    if (fp == NULL)
    {
        printf("File %s does not exist\n", m_initFileName.c_str());
        exit(-1);
    }    

    //现阶段默认的一行的长度不能超过256，每行都是完整的key = value
    //只能在注释中出现中文，否则可能出现越界的错误
    size_t sz = 256;
    char* line = NULL;
    char  command[256];
    char  key[256];
    char  value[256];
    int   count = 1;
    //每次读取一行，以\n为换行符，最长256
    int len = getline(&line, &sz, fp);
    while (len != -1) 
    {
        //1、找到#的位置，即认为后面的都是注释
        //只能在注释中出现中文，这也是为什么先判断注释
        //如果直接给中文去前后的空白，可能出现问题
        int npos = -1;
        for(int n = 0; n < strlen(line); ++n)
        {
            if(line[n] == '#')
            {
                npos = n;
                break;
            }
        }
        if(-1 != npos) //找到了#，并给出位置
        {
            line[npos] = '\0';
            //如果开头就是#，则表明整行都是注释
            if(0 == strlen(line))
            {
                count++;
                len = getline(&line, &sz, fp);
                continue;
            }
        }

        //2、去掉空行
        int nl = trim(command, 256, line, len);
        if(0 == nl)
        {
            //读取到空行，读取下一行
            count++;
            len = getline(&line, &sz, fp);
            continue;
        }

        //3、commad中的应该都是格式正确
        npos = -1;
        for(int n = 0; n < strlen(command); ++n)
        {
            if(command[n] == '=')
            {
                npos = n;
                break;
            }
        }
        if(-1 == npos)
        {
            //没有找到=，格式不正确
            printf("config file format error line %d\n", count);
            fclose(fp);
            return -1;
        }
        else
        {
            //找到正确的配置项
            int nk = trim(key, 256, command, npos);
            if(nk <= 0)
            {
                printf("config file format error line %d, no key\n", count);
                fclose(fp);
                return -1;
            }
            int nv = trim(value, 256, command + npos + 1, strlen(command) - npos - 1);
            if(nv <= 0)
            {
                printf("config file format error line %d, no value\n", count);
                fclose(fp);
                return -1;
            }

            //认为key和value已经是正确的了
            //printf("%s = %s\n", key, value);

            //确认配置名称是否正确，默认的配置项都是存在的
            auto search = m_conf.find(key);
            if (search != m_conf.end()) 
            {
                search->second = value;
            } 
            else 
            {
                printf("config file unknow key:%s line:%d\n", key, count);
                fclose(fp);
                return -1;
            }
        }
        
        len = getline(&line, &sz, fp);
        count++;
    }
    free(line);

    return 0;
}


int CConfigurator::get_format(char* format)
{
    strcpy(format, m_format);

    return 0;
}

std::vector<int> CConfigurator::get_vec()
{
    return m_vec;
}

int CConfigurator::trim(char* dst, int dst_len, char* src, int src_len) 
{
    unsigned int pos = 0;
    //去掉前面的空白
    int start = 0;
    for(;start < src_len; ++start)
    {
        pos = int(src[start]);
        if(whiteSpace[pos] != 1)
        {
            break;
        }
    }

    //去掉后面的空白
    int end = src_len - 1;
    for(;(end >= 0) && (end >= start); --end)
    {
        pos = (unsigned int)(src[end]);
        if(whiteSpace[pos] != 1)
        {
            break;
        }
    }
    strncpy(dst, src + start, end - start + 1);
    dst[end - start + 1] = '\0';

    return strlen(dst);
}


int CConfigurator::set_default()
{
    //设置空格标识
    int pos = int(' ');
    whiteSpace[pos] = 1;
    pos = int('\t');
    whiteSpace[pos] = 1;
    pos = int('\r');
    whiteSpace[pos] = 1;
    pos = int('\n');
    whiteSpace[pos] = 1;

    //加载默认的配置参数
    char aucPathBuf[512] = {0};
	if( readlink("/proc/self/exe", aucPathBuf,1024) <=0 )
	{
			return -1;
	}
    char *name = basename(aucPathBuf);

    char strname[256];
    sprintf(strname, "%s.log", name);
    //1 默认文件名为程序名+.log
    m_conf.insert(std::make_pair(keys[0], strname));
    
    //2 默认路径为当前路径.
    m_conf.insert(std::make_pair(keys[1], "."));

    //3 默认文件数量为24
    m_conf.insert(std::make_pair(keys[2], "24"));

    //4 默认单个文件大小为10m，只有在LOG_SIZE模式下，也就是按照文件大小进行切割的模式下，该参数才有效
    m_conf.insert(std::make_pair(keys[3], "10485760"));  //1024*1024*10 10MB

    //5 默认的日志格式
    m_conf.insert(std::make_pair(keys[4], "[%s - %s - %12s:%4n] %s"));

    //6 默认日志类型，LOG_HOURS
    m_conf.insert(std::make_pair(keys[5], "1"));

    //7 默认日志级别
    m_conf.insert(std::make_pair(keys[6], "300"));
    return 0;
}

int CConfigurator::convert_format(std::vector<int>& vec, char* spf)
{
    auto fd = m_conf.find(keys[4]);
    if(fd != m_conf.end())
    {
        std::string format = fd->second;
        int n_format = 0;  // 配置格式的字符位置索引
        int n_spf = 0;     // sprintf使用的格式化字符串索引
        while(n_format < format.length())
        {
            //寻找定界符%，在这个符号之后的应该都是内容定义符号
            if('%' == format[n_format])
            {
                n_format++;
                char c = format[n_format++];
                switch(c)
                {
                case 'c':
                    //print("c");
                    strncpy(spf + n_spf, "%s", 2);
                    n_spf += 2;

                    vec.push_back(0);
                    break;
                case 'd':
                    //print("d");
                    strncpy(spf + n_spf, "%s", 2);
                    n_spf += 2;

                    vec.push_back(1);
                    break;
                case 'p':
                    //print("p");
                    strncpy(spf + n_spf, "%5s", 3);
                    n_spf += 3;

                    vec.push_back(2);
                    break;
                case 'f':
                    //print("f");
                    strncpy(spf + n_spf, "%16s", 4);
                    n_spf += 4;

                    vec.push_back(3);
                    break;
                case 'n':
                    //print("n");
                    strncpy(spf + n_spf, "%4d", 3);
                    n_spf += 3;

                    vec.push_back(4);
                    break;
                case '%':
                    //print("%");
                    strncpy(spf + n_spf, "%%", 2);
                    n_spf += 2;
                    break;
                default:
                    printf("logformat error unknow key word:%c\n", c);
                    return -1;
                }
            }
            else
            {
                spf[n_spf++] = format[n_format++];
            }
            
        }

    }
    else
    {
        return -1;
    }
    
    return 0;
}
*/
