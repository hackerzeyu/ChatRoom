#pragma once
#include <cstring>
#include <event.h>
#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>
#include <mutex>
#include "log.h"

class ChatDB
{
public:
    ChatDB(string host,string username,string pwd,string db);
    void connect();                                                                 //连接数据库
    void disconnect();                                                              //数据库断开连接
    void init_table();                                                              //初始化表
    bool user_exist(const string& name);                                            //判断用户是否存在
    bool login_correct(const string& name,const string& pwd);                       //登录校验
    void get_friend_group(const string& name,string&friend_list,string& group_list);//获取好友和群聊列表
    void user_register(Json::Value& val);                                           //注册
    bool is_friend(const string& name,const string& friendName);                    //判断是否是好友
    void add_friend(const string& name,const string& friendName);                   //添加好友
    bool group_exist(const string& groupname);                                      //群是否存在
    void create_group(const string& name,const string& groupname);                  //创建群聊
    void add_group(const string& name,const string& groupname);                     //添加群聊
    void get_group_member(const string& groupname,string& groupmember);             //获取群成员         
    ~ChatDB();                              
private:
    MYSQL* m_sql;
    string m_host;
    string m_username;
    string m_pwd;
    string m_db;       //数据库名
    std::mutex m_mutex;
};