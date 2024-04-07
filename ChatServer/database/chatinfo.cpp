#include "chatinfo.h"

void ChatInfo::addUser(const string& name,struct bufferevent* bev)
{
    std::lock_guard<std::mutex> lg(m_userMutex);
    m_userMap.insert(std::make_pair(name,bev));
}

bool ChatInfo::user_exist(const string& name)
{
    std::lock_guard<std::mutex> lg(m_userMutex);
    return m_userMap.find(name)!=m_userMap.end();
}

void ChatInfo::userOffline(const string& name)
{
    std::lock_guard<std::mutex> lg(m_userMutex);
    m_userMap.erase(name);
}

