#include "Server.h"

void Server::init()
{
    //读取配置文件
    IniFile ini;
    ini.load("../config/server.ini");
    m_ip=(string)ini["server"]["ip"];
    m_port=ini["server"]["port"];
    int filePort=ini["server"]["fileport"];
    int threads=ini["server"]["threads"];
    int level=ini["log"]["log_level"];
    int size=ini["log"]["log_size"];
    string username=ini["database"]["username"];
    string password=ini["database"]["password"];
    string database_name=ini["database"]["database_name"];
    //启动日志
    auto logger=Singleton<Log>::instance();
    logger->open("../server.log");
    logger->setLevel(static_cast<Log::Level>(level));
    logger->setMaxLen(size);
    //启动数据库,初始化表结构
    string host="localhost";
    m_database=new ChatDB(host,username,password,database_name);
    m_database->init_table();
    //创建监听集合
    m_base=event_base_new();
    if(!m_base)
    {
        log_fatal("could not initialize libevent!");
        exit(1);
    }
    //启动线程池
    Singleton<ThreadPool>::instance()->run(threads,m_database,filePort);
}

void Server::run()
{
    this->init();
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(m_port);
    serv_addr.sin_addr.s_addr=inet_addr(m_ip.c_str());
    //创建链接侦听器
    m_listener=evconnlistener_new_bind(m_base,listencb,reinterpret_cast<void*>(m_base),LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
        -1,reinterpret_cast<struct sockaddr*>(&serv_addr),sizeof(serv_addr));
    if(!m_listener)
    {
        log_fatal("could not create evconnlistener,errmsg=%s",strerror(errno));
        exit(1);
    }
    //开始监听
    log_info("server is running...");
    event_base_dispatch(m_base);
    evconnlistener_free(m_listener);
    event_base_free(m_base);
    log_info("server quit successfully!");
}

void Server::listencb(struct evconnlistener*,evutil_socket_t cfd,struct sockaddr*,int socklen,void* arg)
{
    log_debug("new connection:connfd=%d",cfd);
    //主线程不处理任务,将任务放置到子线程处理
    Singleton<ThreadPool>::instance()->put(cfd);   
}


