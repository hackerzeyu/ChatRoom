#pragma once
#include <event.h>
#include <list>
#include <mutex>
#include <string>
using std::string;
#include <unordered_map>
#include <vector>
#include "Singleton.h"

class ChatInfo
{
    SINGLETON(ChatInfo);
public:
    void addUser(const string& name,struct bufferevent* bev);           //添加用户到列表
    bool user_exist(const string& name);                                //判断用户存在于列表中
    bufferevent* getbev(const string& name){return m_userMap[name];}    //获取在线用户fd
    void userOffline(const string& name);                               //用户下线删除bev
private:
    std::unordered_map<string,bufferevent*> m_userMap;                  //在线用户列表
    std::mutex m_userMutex;                                             //用户列表互斥锁
};