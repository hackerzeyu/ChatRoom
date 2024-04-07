#include "log.h"

const char* Log::Log_Level[LEVEL_COUNT]={
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL"
};

void Log::open(const string& filename)
{
    m_ofs.open(filename.c_str(),std::ios::app);
    if(m_ofs.fail())
        throw std::logic_error(filename+"open failed!");
    m_filename=filename;
    //获取当前文件长度
    m_ofs.seekp(0,std::ios::end);
    m_len=m_ofs.tellp();
}

void Log::log(Level level,const char* file,int line,const char* format,...)
{
    //文件未打开
    if(m_ofs.fail())
        throw std::logic_error("no file is opened!");
    //日志等级过低不打印
    if(m_level>level)
        return;
    std::ostringstream oss;
    char timeTemp[32]={0};
    time_t ticks=time(nullptr);
    struct tm tm;
    localtime_r(&ticks,&tm);
    strftime(timeTemp,sizeof(timeTemp),"%Y-%m-%d_%H-%M-%S",&tm);
    oss<<timeTemp;

    const char* fmt=" [%s] %s:%d ";
    int len = snprintf(nullptr,0,fmt,Log_Level[level],file,line);
    if(len>0)
    {
        char* str=new char[len+1];
        snprintf(str,len+1,fmt,Log_Level[level],file,line);
        str[len]='\0';
        oss<<str;
        delete[] str;
    }

    va_list arg_ptr;
    va_start(arg_ptr,format);
    len=vsnprintf(nullptr,0,format,arg_ptr);
    va_end(arg_ptr);
    if(len>0)
    {
        char* str=new char[len+1];
        va_start(arg_ptr,format);
        vsnprintf(str,len+1,format,arg_ptr);
        va_end(arg_ptr);
        str[len]='\0';
        oss<<str;
        delete[] str;
    }
    oss<<'\n';
    std::unique_lock<std::mutex> lck(m_mutex);
    m_ofs<<oss.str();
    m_ofs.flush();
    if(m_console)
        std::cout<<oss.str();
    m_len+=oss.str().length();
    if(m_len>=m_maxLen)
    {
        rotate();
    }
}

void Log::rotate()
{
    close();
    //防止日志打印过快
    sleep(1);
    char timeTemp[32]={0};
    time_t ticks=time(nullptr);
    struct tm tm;
    localtime_r(&ticks,&tm);
    strftime(timeTemp,sizeof(timeTemp),".%Y-%m-%d_%H-%M-%S",&tm);
    string newName=m_filename+timeTemp;
    //文件重命名
    if(rename(m_filename.c_str(),newName.c_str())!=0)
        throw std::logic_error("rename filename failed!");
    //日志翻滚
    open(m_filename);
}

void Log::close()
{
    if(m_ofs.is_open())
        m_ofs.close();
}

void Log::setLevel(Level level)
{
    m_level=level;
}

void Log::setMaxLen(int len)
{
    m_maxLen=len;
}

void Log::setConsole(bool flag)
{
    m_console=flag;
}



