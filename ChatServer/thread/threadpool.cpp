#include "threadpool.h"

void ThreadPool::run(int num,ChatDB* db,int filePort)
{
    if(num<=0)
    {
        log_fatal("thread num<=0");
        exit(1);
    }
    m_num=num;
    for(int i=0;i<num;i++)
    {
        WorkerThread* thread=new WorkerThread(db,filePort);
        //启动线程
        thread->start();
        m_pool.push_back(thread);
        log_info("create a thread:%x",thread);
    }
}

void ThreadPool::put(int cfd)  
{
    WorkerThread* thread=get();
    struct bufferevent* bev=bufferevent_socket_new(thread->get(),cfd,BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        log_error("error constructing bev,connfd=%d",cfd);
        return;
    }
    thread->setbev(bev,thread);
}

WorkerThread* ThreadPool::get()
{
    if(m_num>0)
    {
        int index=m_index;
        m_index=(m_index+1)%m_num;
        return m_pool[index];
    }
    return nullptr;
}

