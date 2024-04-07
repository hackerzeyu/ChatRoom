#pragma once
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <event.h>
#include <event2/listener.h>
#include <jsoncpp/json/json.h>
#include <string>
using std::string;
#include <sys/socket.h>
#include <netinet/in.h>
#include "Singleton.h"
#include "ini_file.h"
#include "log.h"
#include "threadpool.h"
#include "chatdb.h"

class Server
{
    SINGLETON(Server);
public:
    void init();
    void run();
    //监听回调
    static void listencb(struct evconnlistener*,evutil_socket_t,struct sockaddr*,int socklen,void*);
private:
    string m_ip;                   
    int m_port;
    struct event_base* m_base;          //事件集合
    struct evconnlistener* m_listener;  //链接侦听器
    ChatDB* m_database;                 //数据库实例
};