#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <event.h>
#include <netinet/in.h>
#include <string>
using std::string;
#include <sys/socket.h>
#include <vector>
#include <jsoncpp/json/json.h>
#include <thread>
#include "chatdb.h"
#include "chatinfo.h"

//业务处理
class Handler
{
public:
    Handler(struct bufferevent* bev,ChatDB* db,int fileport);
    void sendBuf(struct bufferevent* bev,const string& reply);                //发送数据,防止半包粘包
    void readHandle(Json::Value& val);
    void login_handle(Json::Value& val);            
    void register_handle(Json::Value& val);
    void add_friend_handle(Json::Value& val);
    void add_group_handle(Json::Value& val);
    void send_friend_handle(Json::Value& val);
    void private_chat_handle(Json::Value& val);
    void user_online_handle(Json::Value& val);
    void user_offline_handle(Json::Value& val);
    void create_group_handle(Json::Value& val);
    void group_chat_handle(Json::Value & val);
    void send_file_handle(Json::Value& val);
    void file_transfer_handle(int* sfd,int* cfd,long long length);
    void noticeOffline(const string& name,const std::vector<string>& friVec); //通知好友下线
    void noticeOnline(const string& name,const std::vector<string>& friVec);  //通知好友上线
private:
    void split(std::vector<string>& vec,const string& str);
private:
    struct bufferevent* m_bev;
    ChatDB* m_chatdb;
    int m_filePort;             //文件端口
};