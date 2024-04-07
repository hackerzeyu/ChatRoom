#include "Handler.h"

Handler::Handler(struct bufferevent* bev,ChatDB* db,int filePort):m_bev(bev),m_chatdb(db),m_filePort(filePort)
{
}

void Handler::sendBuf(struct bufferevent* bev,const string& reply)
{
    int len=reply.length();
    int bigLen=::htonl(len);
    char* buf=new char[len+4];
    memcpy(buf,&bigLen,4);
    memcpy(buf+4,reply.c_str(),len);
    if(bufferevent_write(bev,buf,len+4)<0)
    {
        log_error("bufferevent_write failed,errmsg=%s",strerror(errno));
    }
    delete[] buf;
}

void Handler::readHandle(Json::Value& val)
{
    //处理读事件
    string cmd=val["cmd"].asString();
    if(cmd=="register")
    {
        register_handle(val);
    }
    else if(cmd=="login")
    {
        login_handle(val);
    }
    else if(cmd=="add_friend")
    {
        add_friend_handle(val);
    }
    else if(cmd=="send_friend")
    {
        send_friend_handle(val);
    }
    else if(cmd=="private_chat")
    {
        private_chat_handle(val);
    }
    else if(cmd=="user_offline")
    {
        user_offline_handle(val);
    }
    else if(cmd=="user_online")
    {
        user_online_handle(val);
    }
    else if(cmd=="create_group")
    {
        create_group_handle(val);
    }
    else if(cmd=="add_group")
    {
        add_group_handle(val);
    }
    else if(cmd=="group_chat")
    {
        group_chat_handle(val);
    }
    else if(cmd=="send_file")
    {
        send_file_handle(val);
    }
}

void Handler::login_handle(Json::Value& val)
{
    Json::Value v;
    v["cmd"]="login_reply";
    string username=val["username"].asString();
    auto info=Singleton<ChatInfo>::instance();
    //用户已经上线
    if(info->user_exist(username))
    {
        v["result"]="user_login";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    m_chatdb->connect();
    //用户不存在
    if(!m_chatdb->user_exist(username))
    {
        m_chatdb->disconnect();
        v["result"]="login_fail";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    string password=val["password"].asString();
    if(!m_chatdb->login_correct(username,password))
    {
        m_chatdb->disconnect();
        v["result"]="login_fail";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    string friendlist,grouplist;
    m_chatdb->get_friend_group(username,friendlist,grouplist);
    m_chatdb->disconnect();
    v["result"]="login_success";
    v["friend_list"]=friendlist;
    v["group_list"]=grouplist;
    string reply=Json::FastWriter().write(v);
    sendBuf(m_bev,reply);
    //将登录用户添加到在线用户队列中
    info->addUser(username,m_bev);
}        

void Handler::register_handle(Json::Value& val)
{
    Json::Value v;
    v["cmd"]="register_reply";
    m_chatdb->connect();
    string username=val["username"].asString();
    //数据库已经存在该用户名
    if(m_chatdb->user_exist(username))
    {
        m_chatdb->disconnect();
        v["result"]="user_exist";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }  
    //注册
    m_chatdb->user_register(val);
    m_chatdb->disconnect();
    v["result"]="register_success";
    string reply=Json::FastWriter().write(v);
    sendBuf(m_bev,reply);
}

void Handler::add_friend_handle(Json::Value& val)
{
    Json::Value v;
    string friendName=val["friendname"].asString();
    v["friendname"]=friendName;
    m_chatdb->connect();
    //该用户不存在
    if(!m_chatdb->user_exist(friendName))
    {
        m_chatdb->disconnect();
        v["cmd"]="add_friend_reply";
        v["result"]="user_not_exist";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    auto info=Singleton<ChatInfo>::instance();
    //用户不在线
    if(!info->user_exist(friendName))
    {
        m_chatdb->disconnect();
        v["cmd"]="add_friend_reply";
        v["friendname"]=friendName;
        v["result"]="friend_offline";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    string userName=val["username"].asString();
    //已经是好友了
    if(m_chatdb->is_friend(userName,friendName))
    {
        m_chatdb->disconnect();
        v["cmd"]="add_friend_reply";
        v["friendname"]=friendName;
        v["result"]="friend_already";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    m_chatdb->disconnect();
    //向对方发送好友申请
    v["cmd"]="recv_friend";
    v["sender"]=userName;
    string reply=Json::FastWriter().write(v);
    sendBuf(info->getbev(friendName),reply);
}

void Handler::send_friend_handle(Json::Value& val)
{
    string friendName=val["friendname"].asString();
    string result=val["result"].asString();
    Json::Value v;
    v["cmd"]="add_friend_reply";
    v["friendname"]=friendName;
    //对方拒绝添加好友
    if(result=="disagree")
    {
        v["result"]="friend_refuse";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    string username=val["username"].asString();
    m_chatdb->connect();
    m_chatdb->add_friend(username,friendName);
    m_chatdb->add_friend(friendName,username);
    m_chatdb->disconnect();
    v["username"]=username;
    v["result"]="add_friend_success";
    string reply=Json::FastWriter().write(v);
    auto info=Singleton<ChatInfo>::instance();
    sendBuf(info->getbev(username),reply);
    Json::Value v2;
    v2["cmd"]="friend_agree";
    v2["username"]=username;
    reply=Json::FastWriter().write(v2);
    sendBuf(m_bev,reply);
}

void Handler::private_chat_handle(Json::Value& val)
{
    string username=val["username"].asString();
    string chatname=val["chatname"].asString();
    Json::Value v;
    v["cmd"]="private_chat_reply";
    v["username"]=username;
    v["chatname"]=chatname;
    auto info=Singleton<ChatInfo>::instance();
    //用户不在线
    if(!info->user_exist(chatname))
    {
        v["result"]="chat_offline";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    v["result"]="send_success";
    string reply=Json::FastWriter().write(v);
    sendBuf(m_bev,reply);
    v.clear();
    string text=val["text"].asString();
    v["cmd"]="recv_private_chat";
    v["text"]=text;
    v["username"]=username;
    v["chatname"]=chatname;
    reply=Json::FastWriter().write(v);
    sendBuf(info->getbev(chatname),reply);
}

void Handler::user_offline_handle(Json::Value& val)
{
    string userName=val["username"].asString();
    string friendList,groupList;
    m_chatdb->connect();
    m_chatdb->get_friend_group(userName,friendList,groupList);
    m_chatdb->disconnect();
    //通知好友下线
    std::vector<string> friVec;
    split(friVec,friendList);
    noticeOffline(userName,friVec);
    auto info=Singleton<ChatInfo>::instance();
    //用户下线,从列表中删除在线用户
    info->userOffline(userName);
}

void Handler::split(std::vector<string>& vec,const string& str)
{
    int first=0,last=0;
    string s="";
    while(true)
    {
        last=str.find('|',first);
        if(last==string::npos)
        {
            //无好友的情况下可能字符串为空
            if(first==0)
                break;
            s=str.substr(first);
            vec.push_back(s);
            break;
        }
        s=str.substr(first,last-first);
        vec.push_back(s);
        first=last+1;
    }
}

void Handler::user_online_handle(Json::Value& val)
{
    string userName=val["username"].asString();
    string friendList,groupList;
    m_chatdb->connect();
    m_chatdb->get_friend_group(userName,friendList,groupList);
    m_chatdb->disconnect();
    std::vector<string> friVec;
    split(friVec,friendList);
    noticeOnline(userName,friVec);
}

void Handler::noticeOffline(const string& name,const std::vector<string>& friVec)
{
    auto info=Singleton<ChatInfo>::instance();
    int n=friVec.size();
    for(int i=0;i<n;i++)
    {
        //若好友在线
        if(info->user_exist(friVec[i]))
        {
            Json::Value v;
            v["cmd"]="friend_offline";
            v["friendname"]=name;
            string reply=Json::FastWriter().write(v);
            sendBuf(info->getbev(friVec[i]),reply);
        }
    }
}

void Handler::noticeOnline(const string& name,const std::vector<string>& friVec)
{
    auto info=Singleton<ChatInfo>::instance();
    int n=friVec.size();
    for(int i=0;i<n;i++)
    {
        //若好友在线
        if(info->user_exist(friVec[i]))
        {
            Json::Value v;
            v["cmd"]="friend_online";
            v["friendname"]=name;
            string reply=Json::FastWriter().write(v);
            sendBuf(info->getbev(friVec[i]),reply);
        }
    }
}

void Handler::create_group_handle(Json::Value& val)
{
    string groupName=val["groupname"].asString();
    Json::Value v;
    v["cmd"]="create_group_reply";
    v["groupname"]=groupName;
    m_chatdb->connect();
    //群名已经存在
    if(m_chatdb->group_exist(groupName))
    {
        m_chatdb->disconnect();
        v["result"]="group_exist";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    string userName=val["owner"].asString();
    m_chatdb->create_group(userName,groupName);
    m_chatdb->add_group(userName,groupName);
    m_chatdb->disconnect();
    v["result"]="create_success";
    string reply=Json::FastWriter().write(v);
    sendBuf(m_bev,reply);
}

void Handler::add_group_handle(Json::Value& val)
{
    string groupName=val["groupname"].asString();
    string username=val["username"].asString();
    string friendList,groupList;
    Json::Value v;
    v["cmd"]="add_group_reply";
    m_chatdb->connect();
    if(!m_chatdb->group_exist(groupName))
    {
        m_chatdb->disconnect();
        v["result"]="group_not_exist";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    m_chatdb->get_friend_group(username,friendList,groupList);
    int first=0,last=0;
    string s;
    int flag=0;
    while(true)
    {
        last=groupList.find('|',first);
        if(last==string::npos)
        {
            s=groupList.substr(first);
            if(s==groupName)
            {
                flag=1;
            }
            break;
        }
        s=groupList.substr(first,last-first);
        if(s==groupName)
        {
            flag=1;
            break;
        }
        first=last+1;
    }
    if(flag)
    {
        m_chatdb->disconnect();
        v["result"]="user_in_group";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    m_chatdb->add_group(username,groupName);
    m_chatdb->disconnect();
    v["groupname"]=groupName;
    v["result"]="add_group_success";
    string reply=Json::FastWriter().write(v);
    sendBuf(m_bev,reply);
}

void Handler::group_chat_handle(Json::Value & val)
{
    string userName=val["username"].asString();
    string groupName=val["groupname"].asString();
    string text=val["text"].asString();
    string groupMember;
    m_chatdb->connect();
    m_chatdb->get_group_member(groupName,groupMember);
    m_chatdb->disconnect();
    std::vector<string> member;
    split(member,groupMember);
    Json::Value v;
    v["cmd"]="group_chat_reply";
    v["groupname"]=groupName;
    v["text"]=text;
    v["username"]=userName;
    string reply=Json::FastWriter().write(v);
    auto info=Singleton<ChatInfo>::instance();
    for(int i=0;i<member.size();i++)
    {
        if(info->user_exist(member[i]) && member[i]!=userName)
        {
            sendBuf(info->getbev(member[i]),reply);
        }
    }
}

void Handler::send_file_handle(Json::Value& val)
{
    string userName=val["username"].asString();
    string chatName=val["chatname"].asString();
    Json::Value v;
    auto info=Singleton<ChatInfo>::instance();
    //对方不在线
    if(!info->user_exist(chatName))
    {
        v["cmd"]="send_file_reply";
        v["username"]=userName;
        v["chatname"]=chatName;
        v["result"]="user_offline";
        string reply=Json::FastWriter().write(v);
        sendBuf(m_bev,reply);
        return;
    }
    int* sfd=new int(0);
    int* cfd=new int(0);
    std::thread t(&Handler::file_transfer_handle,this,sfd,cfd,val["file_length"].asInt64());
    t.detach();
    v["cmd"]="send_file_port_reply";
    v["username"]=userName;
    v["chatname"]=chatName;
    v["filename"]=val["filename"];
    v["file_length"]=val["file_length"];
    v["port"]=m_filePort;
    string reply=Json::FastWriter().write(v);
    sendBuf(m_bev,reply);
    int count=0;
    while(sfd==0)
    {
        count++;
        usleep(100000);
        if(count==100)
        {
            pthread_cancel(t.native_handle());
            v.clear();
            v["cmd"]="send_file_reply";
            v["sender"]=userName;
            v["receiver"]=chatName;
            v["result"]="send_timeout";
            reply=Json::FastWriter().write(v);
            sendBuf(m_bev,reply);
            return;
        }
    }
    v.clear();
    v["cmd"]="recv_file_port_reply";
    v["username"]=userName;
    v["chatname"]=chatName;
    v["filename"]=val["filename"];
    v["file_length"]=val["file_length"];
    v["port"]=m_filePort;
    reply=Json::FastWriter().write(v);
    sendBuf(info->getbev(chatName),reply);
    while(cfd==0)
    {
        count++;
        usleep(100000);
        if(count==100)
        {
            pthread_cancel(t.native_handle());
            v.clear();
            v["cmd"]="recv_file_reply";
            v["sender"]=userName;
            v["receiver"]=chatName;
            v["result"]="recv_timeout";
            reply=Json::FastWriter().write(v);
            sendBuf(m_bev,reply);
            return;
        }
    }
}

void Handler::file_transfer_handle(int* sfd,int* cfd,long long length)
{
    //绑定文件服务器
    int fd=::socket(AF_INET,SOCK_STREAM,0);
    if(fd<0)
    {
        log_error("socket create error,errmsg=%s",strerror(errno));
        return;
    }
    int opt=1;
    int ret=::setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(ret<0)
    {
        log_error("set reuseaddr error,errmsg=%s",strerror(errno));
        return;
    }
    //缓冲区10k
    int buff_size=1024*1024;
    ::setsockopt(fd,SOL_SOCKET,SO_SNDBUF,&buff_size,sizeof(buff_size));
    ::setsockopt(fd,SOL_SOCKET,SO_RCVBUF,&buff_size,sizeof(buff_size));
    struct sockaddr_in fileserv;
    fileserv.sin_family=AF_INET;
    fileserv.sin_addr.s_addr=inet_addr("172.29.235.134");
    fileserv.sin_port=htons(m_filePort);
    ret=bind(fd,reinterpret_cast<sockaddr*>(&fileserv),sizeof(fileserv));
    if(ret<0)
    {
        log_error("socket bind error,errmsg=%s",strerror(errno));
        return;
    }
    ret=listen(fd,128);
    if(ret<0)
    {
        log_error("socket listen error,errmsg=%s",strerror(errno));
        return;
    }
    *sfd=::accept(fd,nullptr,nullptr);
    *cfd=::accept(fd,nullptr,nullptr);
    
    long long sum=0;
    int len;
    char buf[1024*1024]={0};
    while(true)
    {
        memset(buf,0,sizeof(buf));
        len=::recv(*sfd,buf,sizeof(buf),0);
        if(len<0)
        {
            log_error("recv failed,errmsg=%s",strerror(errno));
            break;
        }
        else if(len==0)
        {
            log_debug("sender close.");
            break;
        }
        len=::send(*cfd,buf,len,0);
        if(len<0)
        {
            log_error("send failed,errmsg=%s",strerror(errno));
            break;
        }
        else if(len==0)
        {
            log_debug("receiver close.");
            break;
        }
        else
        {
            sum+=len;
            log_debug("sum=%d,length=%d",sum,length);
            if(sum>=length)
                break;
        }
    }
    log_debug("file end");
    close(*sfd);
    close(*cfd);
    close(fd);
    delete sfd;
    delete cfd;
}