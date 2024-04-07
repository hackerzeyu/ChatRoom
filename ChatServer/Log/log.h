#pragma once
#include <iostream>
#include <ctime>
#include <cstdarg>
#include <string>
using std::string;
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include "../utility/Singleton.h"
#include <mutex>

#define log_debug(fmt,...) \
    Singleton<Log>::instance()->log(Log::DEBUG,__FILE__,__LINE__,fmt,##__VA_ARGS__)
#define log_info(fmt,...) \
    Singleton<Log>::instance()->log(Log::INFO,__FILE__,__LINE__,fmt,##__VA_ARGS__)
#define log_warn(fmt,...) \
    Singleton<Log>::instance()->log(Log::WARN,__FILE__,__LINE__,fmt,##__VA_ARGS__)
#define log_error(fmt,...) \
    Singleton<Log>::instance()->log(Log::ERROR,__FILE__,__LINE__,fmt,##__VA_ARGS__)
#define log_fatal(fmt,...) \
    Singleton<Log>::instance()->log(Log::FATAL,__FILE__,__LINE__,fmt,##__VA_ARGS__)
class Log
{
    SINGLETON(Log);
public:
    enum Level
    {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LEVEL_COUNT
    };
    //打开文件
    void open(const string& filename);
    //关闭文件
    void close();
    //打印日志
    void log(Level level,const char* file,int line,const char* format,...);
    //日志翻滚
    void rotate();
    //设置日志等级
    void setLevel(Level level);
    //设置日志最大长度
    void setMaxLen(int len);
    //设置打印控制台
    void setConsole(bool flag);
private:
    static const char* Log_Level[LEVEL_COUNT];  //日志等级对应字符串
private:
    std::mutex m_mutex;         //互斥锁
    std::ofstream m_ofs;        //文件流
    string m_filename;          //文件名
    int m_len = 0;              //文件长度
    Level m_level=DEBUG;        //日志等级
    int m_maxLen=1024;          //日志最大长度
    bool m_console= true;       //打印控制台选项
};


