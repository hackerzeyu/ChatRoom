#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QCloseEvent>
#include <QTcpSocket>
#include <QMessageBox>
#include <QListWidget>
#include <QList>
#include "addfriend.h"
#include "application.h"
class PrivateChat;
#include "privatechat.h"
#include "addgroup.h"
#include "creategroup.h"
class GroupChat;
#include "groupchat.h"

struct ChatInfo
{
    PrivateChat* m_chat;
    QString name;
};

struct GroupInfo
{
    GroupChat* m_chat;
    QString groupName;
};

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QTcpSocket* socket,QString userName,QString friendList,QString groupList,QWidget *parent = nullptr);
    ~MainWidget();
    void initUI();
protected:
    void closeEvent(QCloseEvent* event) override;
public:
    //添加好友处理
    void add_friend_reply_handle(QJsonObject& obj);
    //接收到好友申请
    void recv_friend_handle(QJsonObject& obj);
    //好友添加成功
    void friend_agree_handle(QJsonObject& obj);
    //打开私聊窗口
    void private_chat_window(QListWidgetItem* item);
    //私聊
    void private_chat_reply_handle(QJsonObject& obj);
    //接收到私聊信息
    void recv_private_chat_handle(QJsonObject& obj);
    //打开群聊窗口
    void group_chat_window(QListWidgetItem* item);
    //好友下线
    void friend_offline_handle(QJsonObject& obj);
    //好友上线
    void friend_online_handle(QJsonObject& obj);
    //创建群聊
    void create_group_reply_handle(QJsonObject& obj);
    //添加群聊
    void add_group_reply_handle(QJsonObject& obj);
    //群聊
    void group_chat_reply_handle(QJsonObject& obj);
    //发送文件
    void send_file_reply_handle(QJsonObject& obj);
    void send_file_port_reply_handle(QJsonObject& obj);
    //接收文件
    void recv_file_reply_handle(QJsonObject& obj);
    void recv_file_port_reply_handle(QJsonObject& obj);
signals:
    void private_chat_offline(QJsonObject& obj);
    void private_chat_sender(QJsonObject& obj);
    void private_chat_receiver(QJsonObject& obj);
    void group_chat_receiver(QJsonObject& obj);
    void send_file_start(QJsonObject& obj);
    void sender_timeout(QJsonObject& obj);
    void receiver_timeout(QJsonObject& obj);
    void start_working();
private slots:
    void on_addFriend_clicked();
    void on_friendBtn_clicked();
    void on_groupBtn_clicked();
    void on_addGroup_clicked();

    void on_createGroup_clicked();

private:
    AddFriend* m_addFriend;
    Application* m_apply;
    AddGroup* m_addGroup;
    CreateGroup* m_createGroup;
private:
    Ui::MainWidget *ui;
    QTcpSocket* m_socket;
    QString m_userName;
    QString m_friendList;           //好友列表
    QString m_groupList;            //群列表
    QList<ChatInfo> m_chatInfo;     //私聊窗口
    QList<GroupInfo> m_groupInfo;   //群聊窗口
};

#endif // MAINWIDGET_H
