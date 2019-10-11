#ifndef _H_CONFIGURATOR_
#define _H_CONFIGURATOR_


#include <iostream>
#include <string>
#include <map>
#include <vector>

//配置获取
namespace COMM
{

using namespace std;
class CConfigurator
{
public:
    CConfigurator(const string&  initFileName);
    ~CConfigurator();
    
    //获取配置项的key-value
    int get_conf(map<string, string> &cfg);

private:
    string                  m_FileName;
    int                     whiteSpace[128] = {0};   //空白标识符

    //去掉前后的空格，返回去掉空格后的长度
    int trim(char* dst, int dst_len, char* src, int src_len);
    
};

} // namespace COMM




#endif //_H_CONFIGURATOR_