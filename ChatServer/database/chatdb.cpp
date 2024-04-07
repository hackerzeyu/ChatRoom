#include "chatdb.h"

ChatDB::ChatDB(string host,string username,string pwd,string db):
    m_host(host),m_username(username),m_pwd(pwd),m_db(db)
{
}

void ChatDB::connect()
{
    m_sql=mysql_init(nullptr);
    if(m_sql==nullptr)
    {
        log_fatal("mysql_init failed");
        exit(1);
    }
    m_sql=mysql_real_connect(m_sql,m_host.c_str(),m_username.c_str(),m_pwd.c_str(),m_db.c_str(),0,nullptr,0);
    if(m_sql==nullptr)
    {
        log_fatal("mysql connect failed,errmsg=%s",mysql_error(m_sql));
        exit(1);
    }
    //设置编码为utf8
    mysql_set_character_set(m_sql,"utf8");
}

void ChatDB::disconnect()
{
    mysql_close(m_sql);
}


void ChatDB::init_table()
{
    connect();
    const char* user_sql="create table if not exists tb_user("\
        "username varchar(30) comment '账号',password varchar(30) comment '密码',"\
        "friendlist varchar(4096) comment '好友列表',grouplist varchar(4096) comment '群列表')"\
        "charset utf8;";
    if(mysql_query(m_sql,user_sql)!=0)
    {
        log_fatal("create table tb_user failed,errmsg=%s",mysql_error(m_sql));
        exit(1);
    }
    const char* group_sql="create table if not exists tb_group("\
        "groupname varchar(30) comment '群名',groupowner varchar(30) comment '群主',"\
        "groupmember varchar(4096) comment '群成员') charset utf8;";
    if(mysql_query(m_sql,group_sql)!=0)
    {
        log_fatal("create table tb_group failed,errmsg=%s",mysql_error(m_sql));
        exit(1);
    }
    disconnect();
}

bool ChatDB::user_exist(const string& name)
{
    char sql[128]={0};
    snprintf(sql,sizeof(sql),"select * from tb_user where username='%s';",name.c_str());
    std::unique_lock<std::mutex> lck(m_mutex);
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return true;
    }
    MYSQL_RES* res=mysql_store_result(m_sql);
    if(res==nullptr)
    {
        log_error("store result failed,errmsg=%s",mysql_error(m_sql));
        return true;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row==nullptr)
    {
        mysql_free_result(res);
        return false;
    }
    mysql_free_result(res);
    return true;
}

bool ChatDB::login_correct(const string& name,const string& pwd)
{
    char sql[128]={0};
    snprintf(sql,sizeof(sql),"select password from tb_user where username='%s';",name.c_str());
    std::unique_lock<std::mutex> lck(m_mutex);
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return false;
    }
    MYSQL_RES* res=mysql_store_result(m_sql);
    if(res==nullptr)
    {
        log_error("store result failed,errmsg=%s",mysql_error(m_sql));
        return false;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row==nullptr)
    {
        log_error("fetch row failed,errmsg=%s",mysql_error(m_sql));
        mysql_free_result(res);
        return false;
    }
    if(row[0]!=pwd)
    {
        mysql_free_result(res);
        return false;
    }
    mysql_free_result(res);
    return true;
}

void ChatDB::get_friend_group(const string& name,string& friend_list,string& group_list)
{
    friend_list="";
    group_list="";
    char sql[128]="";
    snprintf(sql,sizeof(sql),"select friendlist,grouplist from tb_user where username='%s';",name.c_str());
    std::unique_lock<std::mutex> lck(m_mutex);
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    MYSQL_RES* res=mysql_store_result(m_sql);
    if(res==nullptr)
    {
        log_error("store result failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row==nullptr)
    {
        log_error("fetch row failed,errmsg=%s",mysql_error(m_sql));
        mysql_free_result(res);
        return;
    }
    if(row[0])
        friend_list=row[0];
    if(row[1])
        group_list=row[1];
    mysql_free_result(res);
}

void ChatDB::user_register(Json::Value& val)
{
    string username=val["username"].asString();
    string password=val["password"].asString();
    char sql[256]="";
    snprintf(sql,sizeof(sql),"insert into tb_user (username,password) values('%s','%s');",username.c_str(),password.c_str());
    std::unique_lock<std::mutex> lck(m_mutex);
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("insert failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
}

bool ChatDB::is_friend(const string& name,const string& friendName)
{
    char sql[128]={0};
    snprintf(sql,sizeof(sql),"select friendlist from tb_user where username='%s';",name.c_str());
    std::unique_lock<std::mutex> lck(m_mutex);
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return true;
    }
    MYSQL_RES* res=mysql_store_result(m_sql);
    if(res==nullptr)
    {
        log_error("store result failed,errmsg=%s",mysql_error(m_sql));
        return true;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row==nullptr)
    {
        log_error("fetch row failed,errmsg=%s",mysql_error(m_sql));
        mysql_free_result(res);
        return true;
    }
    if(row[0]==nullptr)
    {
        mysql_free_result(res);
        return false;
    }
    string friendList=row[0];    
    int first=0,last=0;
    while(true)
    {
        last=friendList.find('|',first);
        if(last==string::npos)
        {
            if(friendList.substr(first)==friendName)
                return true;
            break;
        }
        if(friendList.substr(first,last-first)==friendName)
            return true;
        first=last+1;
    }
    mysql_free_result(res);
    return false;
}

void ChatDB::add_friend(const string& name,const string& friendName)
{
    char sql[128]={0};
    snprintf(sql,sizeof(sql),"select friendlist from tb_user where username='%s';",name.c_str());
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    MYSQL_RES* res=mysql_store_result(m_sql);
    if(res==nullptr)
    {
        log_error("store result failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row==nullptr)
    {
        log_error("fetch row failed,errmsg=%s",mysql_error(m_sql));
        mysql_free_result(res);
        return;
    }
    string friendList="";
    if(row[0])
    {
        friendList=row[0];
        friendList.append("|");
        friendList.append(friendName);
    }
    else
        friendList.append(friendName);
    snprintf(sql,sizeof(sql),"update tb_user set friendlist='%s' where username='%s';",friendList.c_str(),name.c_str());
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("update failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    mysql_free_result(res);
}

bool ChatDB::group_exist(const string& groupname)    
{
    char sql[128]={0};
    snprintf(sql,sizeof(sql),"select * from tb_group where groupname='%s';",groupname.c_str());
    std::unique_lock<std::mutex> lck(m_mutex);
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return true;
    }
    MYSQL_RES* res=mysql_store_result(m_sql);
    if(res==nullptr)
    {
        log_error("store result failed,errmsg=%s",mysql_error(m_sql));
        return true;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row==nullptr)
    {
        mysql_free_result(res);
        return false;
    }
    mysql_free_result(res);
    return true;
}

void ChatDB::create_group(const string& name,const string& groupname)
{
    char sql[128]={0};
    snprintf(sql,sizeof(sql),"insert into tb_group (groupname,groupowner) values('%s','%s');",groupname.c_str(),name.c_str());
    std::unique_lock<std::mutex> lck(m_mutex);
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("insert failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
}

void ChatDB::add_group(const string& name,const string& groupname)
{
    char sql[128]={0};
    snprintf(sql,sizeof(sql),"select groupmember from tb_group where groupname='%s';",groupname.c_str());
    std::unique_lock<std::mutex> lck(m_mutex);
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    MYSQL_RES* res=mysql_store_result(m_sql);
    if(res==nullptr)
    {
        log_error("store result failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row==nullptr)
    {
        log_error("fetch row failed,errmsg=%s",mysql_error(m_sql));
        mysql_free_result(res);
        return;
    }
    string groupMember;
    if(!row[0])
    {
        groupMember=name;
    }
    else
    {
        groupMember=row[0];
        groupMember+='|';
        groupMember+=name;
    }
    mysql_free_result(res);
    memset(sql,0,sizeof(sql));
    snprintf(sql,sizeof(sql),"update tb_group set groupmember='%s' where groupname='%s';",groupMember.c_str(),groupname.c_str());
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    memset(sql,0,sizeof(sql));
    snprintf(sql,sizeof(sql),"select grouplist from tb_user where username='%s';",name.c_str());
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    res=mysql_store_result(m_sql);
    if(res==nullptr)
    {
        log_error("store result failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    row=mysql_fetch_row(res);
    if(row==nullptr)
    {
        log_error("fetch row failed,errmsg=%s",mysql_error(m_sql));
        mysql_free_result(res);
        return;
    }
    string groupList="";
    if(!row[0])
    {
        groupList=groupname;
    }
    else
    {
        groupList=row[0];
        groupList+='|';
        groupList+=groupname;
    }
    mysql_free_result(res);
    memset(sql,0,sizeof(sql));
    snprintf(sql,sizeof(sql),"update tb_user set grouplist='%s' where username='%s';",groupList.c_str(),name.c_str());
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("update failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
}

void ChatDB::get_group_member(const string& groupname,string& groupmember)
{
    char sql[128]={0};
    snprintf(sql,sizeof(sql),"select groupmember from tb_group where groupname='%s';",groupname.c_str());
    std::unique_lock<std::mutex> lck(m_mutex);
    if(mysql_query(m_sql,sql)!=0)
    {
        log_error("select failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    MYSQL_RES* res=mysql_store_result(m_sql);
    if(res==nullptr)
    {
        log_error("store result failed,errmsg=%s",mysql_error(m_sql));
        return;
    }
    MYSQL_ROW row=mysql_fetch_row(res);
    if(row==nullptr)
    {
        log_error("fetch row failed,errmsg=%s",mysql_error(m_sql));
        mysql_free_result(res);
        return;
    }
    if(!row[0])
        groupmember="";
    else
        groupmember=row[0];
    mysql_free_result(res);
}

ChatDB::~ChatDB()
{
}                              