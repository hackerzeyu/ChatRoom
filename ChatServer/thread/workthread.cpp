#include "workthread.h"

WorkerThread::WorkerThread(ChatDB* db,int filePort):m_base(nullptr),m_chatdb(db),m_filePort(filePort)
{
}

void WorkerThread::start()
{
    m_base=event_base_new();
    //创建线程
    m_thread=std::thread(&WorkerThread::run,this);
    m_thread.detach();
}

void WorkerThread::run()
{
    //注册定时器事件,防止退出
    struct event timeout;
    struct timeval tv;
    event_assign(&timeout,m_base,-1,EV_PERSIST,timeout_cb,nullptr);
    evutil_timerclear(&tv);
    tv.tv_sec=1;
    event_add(&timeout,&tv);
    event_base_dispatch(m_base); 
    event_base_free(m_base);
    log_info("thread exit,addr=%x",this);
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::setbev(struct bufferevent* bev,void* arg)
{
    bufferevent_setcb(bev,readcb,nullptr,eventcb,arg);
    bufferevent_enable(bev,EV_READ);
}

void WorkerThread::timeout_cb(evutil_socket_t fd, short event, void *arg)
{
}

void WorkerThread::readcb(struct bufferevent* bev,void* arg)
{
    WorkerThread* thread=static_cast<WorkerThread*>(arg);
    //防止粘包
    int len=0;
    size_t ret=bufferevent_read(bev,(char*)&len,4);
    if(ret!=4)
    {
        log_error("read data len failed!");
        return;
    }
    //转为主机字节序
    len=ntohl(len);
    char buf[1024];
    int sum=0,count=0;
    std::ostringstream oss;
    while (sum<len)
    {
        memset(buf,0,sizeof(buf));
        count=bufferevent_read(bev,buf,sizeof(buf));
        sum+=count;
        oss<<buf;
    }
    string msg=oss.str();
    //解析json字符串
    Json::Value val;
    Json::Reader r;
    if(!r.parse(msg,val))
    {
        log_error("parse jsonstring failed!");
        return;
    }
    //处理读事件
    Handler handler(bev,thread->m_chatdb,thread->m_filePort);
    handler.readHandle(val);
}

void WorkerThread::eventcb(struct bufferevent* bev,short what,void* arg)
{
    if(what & BEV_EVENT_EOF)
    {
        log_debug("connection closed,connfd=%d",bufferevent_getfd(bev));
    }
    else if(what & BEV_EVENT_ERROR)
    {
        log_error("error on the connection:%s",strerror(errno));
    }
    bufferevent_free(bev);
}