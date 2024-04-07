#pragma once
#include <condition_variable>
#include <cstring>
#include <event.h>
#include <mutex>
#include <jsoncpp/json/json.h>
#include <thread>
#include "log.h"
#include "Handler.h"

class WorkerThread
{
public:
    WorkerThread(ChatDB* db,int filePort);
    ~WorkerThread();   
    void start();   //启动线程
    void run();     //运行函数
    event_base* get(){return m_base;}
    void setbev(struct bufferevent*,void* arg);
    //读回调
    static void readcb(struct bufferevent* bev,void* arg);
    //异常回调
    static void eventcb(struct bufferevent* bev,short what,void* arg);
    //定时器事件
    static void timeout_cb(evutil_socket_t fd, short event, void *arg);
private:
    ChatDB* m_chatdb;                  //数据库实例
    std::thread m_thread;
    struct event_base* m_base;
    int m_filePort;                    //文件端口
};