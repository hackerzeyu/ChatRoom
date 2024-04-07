#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "tcp.h"
#include "recvfile.h"
#include <QThread>

MainWidget::MainWidget(QTcpSocket* socket,QString userName,QString friendList,QString groupList,QWidget *parent) :
    QWidget(parent),ui(new Ui::MainWidget),m_socket(socket),m_userName(userName),m_friendList(friendList),
    m_groupList(groupList)
{
    ui->setupUi(this);
    initUI();
    connect(m_socket,&QTcpSocket::readyRead,this,[=](){
        QByteArray data=::recvBuf(m_socket);
        QJsonDocument doc=QJsonDocument::fromJson(data);
        QJsonObject obj=doc.object();
        QString cmd=obj["cmd"].toString();
        if(cmd=="add_friend_reply")
        {
            add_friend_reply_handle(obj);
        }
        else if(cmd=="recv_friend")
        {
            recv_friend_handle(obj);
        }
        else if(cmd=="friend_agree")
        {
            friend_agree_handle(obj);
        }
        else if(cmd=="private_chat_reply")
        {
            private_chat_reply_handle(obj);
        }
        else if(cmd=="recv_private_chat")
        {
            recv_private_chat_handle(obj);
        }
        else if(cmd=="friend_online")
        {
            friend_online_handle(obj);
        }
        else if(cmd=="friend_offline")
        {
            friend_offline_handle(obj);
        }
        else if(cmd=="create_group_reply")
        {
            create_group_reply_handle(obj);
        }
        else if(cmd=="add_group_reply")
        {
            add_group_reply_handle(obj);
        }
        else if(cmd=="group_chat_reply")
        {
            group_chat_reply_handle(obj);
        }
        else if(cmd=="send_file_reply")
        {
            send_file_reply_handle(obj);
        }
        else if(cmd=="recv_file_reply")
        {
            recv_file_reply_handle(obj);
        }
        else if(cmd=="send_file_port_reply")
        {
            send_file_port_reply_handle(obj);
        }
        else if(cmd=="recv_file_port_reply")
        {
            recv_file_port_reply_handle(obj);
        }
    });

    connect(ui->friendList,&QListWidget::itemDoubleClicked,this,&MainWidget::private_chat_window);
    connect(ui->groupList,&QListWidget::itemDoubleClicked,this,&MainWidget::group_chat_window);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::initUI()
{
    this->setWindowTitle(m_userName);
    this->setWindowIcon(QIcon(":/image/qq.jpg"));
    ui->stackWidget->setCurrentIndex(0);
    QStringList friendList=m_friendList.split('|');
    QStringList groupList=m_groupList.split('|');
    for(int i=0;i<friendList.size();i++)
    {
        if(friendList[i].isEmpty())
            continue;
        ui->friendList->addItem(friendList[i]);
    }
    for(int i=0;i<groupList.size();i++)
    {
        if(groupList[i].isEmpty())
            continue;
        ui->groupList->addItem(groupList[i]);
    }
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    QJsonObject obj;
    obj["cmd"]="user_offline";
    obj["username"]=m_userName;
    QJsonDocument doc(obj);
    QByteArray json=doc.toJson();
    ::sendBuf(m_socket,json);
    m_socket->flush();
    event->accept();
}

void MainWidget::add_friend_reply_handle(QJsonObject& obj)
{
    QString friendName=obj["friendname"].toString();
    QString result=obj["result"].toString();
    QString info;
    if(result=="add_friend_success")
    {
        info=QString("添加好友%1成功!").arg(friendName);
        ui->friendList->addItem(friendName);
    }
    else if(result=="friend_already")
    {
        info=QString("%1已经是好友了!").arg(friendName);
    }
    else if(result=="user_not_exist")
    {
        info=QString("不存在用户名为%1的账户!").arg(friendName);
    }
    else if(result=="friend_offline")
    {
        info=QString("用户%1不在线").arg(friendName);
    }
    else
    {
        info=QString("%1拒绝了你的好友申请!").arg(friendName);
    }
    QMessageBox::information(this,"提示",info);
}

void MainWidget::recv_friend_handle(QJsonObject& obj)
{
    QString sender=obj["sender"].toString();
    m_apply=new Application(m_socket,sender,m_userName);
    m_apply->show();
}

void MainWidget::friend_agree_handle(QJsonObject &obj)
{
    QString username=obj["username"].toString();
    ui->friendList->addItem(username);
}

void MainWidget::private_chat_window(QListWidgetItem *item)
{
    QString chatName=item->text();
    for(int i=0;i<m_chatInfo.size();i++)
    {
        if(m_chatInfo.at(i).m_chat->getChatName()==chatName)
        {
            PrivateChat* chat=m_chatInfo.at(i).m_chat;
            if(chat->isMinimized())
                chat->showNormal();
            chat->activateWindow();
            //不允许重复创建同一对象的聊天窗口
            return;
        }
    }
    PrivateChat* priChat=new PrivateChat(m_socket,m_userName,chatName,this,&m_chatInfo);
    priChat->show();
    ChatInfo info{priChat,m_userName};
    m_chatInfo.push_back(info);
}

void MainWidget::private_chat_reply_handle(QJsonObject &obj)
{
    if(obj["result"].toString()=="chat_offline")
    {
        emit private_chat_offline(obj);
    }
    else
    {
        emit private_chat_sender(obj);
    }
}

void MainWidget::recv_private_chat_handle(QJsonObject &obj)
{
    QString username=obj["username"].toString();
    QString chatname=obj["chatname"].toString();
    bool flag=0;
    for(int i=0;i<m_chatInfo.size();i++)
    {
        if(m_chatInfo.at(i).name==chatname)
        {
            flag=true;
            break;
        }
    }
    if(!flag)
    {
        PrivateChat* priChat=new PrivateChat(m_socket,chatname,username,this,&m_chatInfo);
        ChatInfo info{priChat,chatname};
        m_chatInfo.push_back(info);
        priChat->show();
    }
    emit private_chat_receiver(obj);
}

void MainWidget::group_chat_window(QListWidgetItem *item)
{
    QString groupName=item->text();
    for(int i=0;i<m_groupInfo.size();i++)
    {
        if(m_groupInfo.at(i).groupName==groupName)
        {
            GroupChat* m_chat=m_groupInfo.at(i).m_chat;
            if(m_chat->isMinimized())
            {
                m_chat->showNormal();
            }
            m_chat->isActiveWindow();
            //不允许重复创建同一对象的群聊窗口
            return;
        }
    }
    GroupChat* groupChat=new GroupChat(m_socket,m_userName,groupName,this,&m_groupInfo);
    groupChat->show();
    GroupInfo groupInfo{groupChat,groupName};
    m_groupInfo.push_back(groupInfo);
}

void MainWidget::friend_offline_handle(QJsonObject &obj)
{
    QString friendName=obj["friendname"].toString();
    QString info=QString("%1下线了").arg(friendName);
    QMessageBox::information(this,"提示",info);
}

void MainWidget::friend_online_handle(QJsonObject &obj)
{
    QString friendName=obj["friendname"].toString();
    QString info=QString("%1上线了").arg(friendName);
    QMessageBox::information(this,"提示",info);
}

void MainWidget::create_group_reply_handle(QJsonObject &obj)
{
    QString groupName=obj["groupname"].toString();
    QString result=obj["result"].toString();
    if(result=="group_exist")
    {
        QString info=QString("群名%1已经存在!").arg(groupName);
        QMessageBox::information(this,"提示",info);
    }
    else
    {
        QMessageBox::information(this,"提示","创建成功!");
        ui->groupList->addItem(groupName);
    }
}

void MainWidget::add_group_reply_handle(QJsonObject &obj)
{
    QString result=obj["result"].toString();
    if(result=="user_in_group")
    {
        QMessageBox::information(this,"提示","您已在该群聊中!");
    }
    else if(result=="add_group_success")
    {
        QMessageBox::information(this,"提示","添加成功!");
        ui->groupList->addItem(obj["groupname"].toString());
    }
    else if(result=="group_not_exist")
    {
        QMessageBox::information(this,"提示","群名不存在!");
    }
}

void MainWidget::group_chat_reply_handle(QJsonObject &obj)
{
    QString groupName=obj["groupname"].toString();
    int flag=1;
    for(int i=0;i<m_groupInfo.size();i++)
    {
        if(m_groupInfo.at(i).groupName==groupName)
        {
            flag=0;
            break;
        }
    }
    if(flag)
    {
        GroupChat* groupChat=new GroupChat(m_socket,m_userName,groupName,this,&m_groupInfo);
        groupChat->show();
        GroupInfo groupInfo{groupChat,groupName};
        m_groupInfo.push_back(groupInfo);
    }
    emit group_chat_receiver(obj);
}

void MainWidget::send_file_reply_handle(QJsonObject &obj)
{
    QString result=obj["result"].toString();
    if(result=="user_offline")
    {
        emit private_chat_offline(obj);
    }
    else
    {
        //超时
        emit sender_timeout(obj);
    }
}

void MainWidget::send_file_port_reply_handle(QJsonObject &obj)
{
    emit send_file_start(obj);
}

void MainWidget::recv_file_reply_handle(QJsonObject &obj)
{
    emit receiver_timeout(obj);
}

void MainWidget::recv_file_port_reply_handle(QJsonObject &obj)
{
    //启动接收线程
    QThread* recvThread=new QThread();
    RecvFile* recvFile=new RecvFile(obj);
    recvFile->moveToThread(recvThread);
    recvThread->start();
    connect(this,&MainWidget::start_working,recvFile,&RecvFile::working);
    emit start_working();
    connect(recvFile,&RecvFile::recv_timeout,this,[=](){
        QMessageBox::warning(this,"提示","接收客户端连接超时!");
    });
    connect(recvFile,&RecvFile::recv_done,this,[=](){
        QMessageBox::information(this,"提示","文件接收完成!");
        recvThread->quit();
        recvThread->wait();
        recvThread->deleteLater();
        recvFile->deleteLater();
    });
}

void MainWidget::on_addFriend_clicked()
{
    m_addFriend=new AddFriend(m_socket,m_userName);
    m_addFriend->show();
}


void MainWidget::on_friendBtn_clicked()
{
    ui->stackWidget->setCurrentIndex(0);
}


void MainWidget::on_groupBtn_clicked()
{
    ui->stackWidget->setCurrentIndex(1);
}


void MainWidget::on_addGroup_clicked()
{
    m_addGroup=new AddGroup(m_socket,m_userName);
    m_addGroup->show();
}


void MainWidget::on_createGroup_clicked()
{
    m_createGroup=new CreateGroup(m_socket,m_userName);
    m_createGroup->show();
}

