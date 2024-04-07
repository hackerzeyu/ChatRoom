#include "groupchat.h"
#include "ui_groupchat.h"
#include "tcp.h"

GroupChat::GroupChat(QTcpSocket* socket,QString userName,QString groupName,MainWidget* mainWidget,QList<GroupInfo>* groupInfo,QWidget *parent) :
    QWidget(parent),ui(new Ui::GroupChat),m_socket(socket),m_userName(userName),m_groupName(groupName),m_groupInfo(groupInfo),
    m_mainWidget(mainWidget)
{
    ui->setupUi(this);
    this->setWindowTitle(m_groupName);
    this->setWindowIcon(QIcon(":/image/qq.jpg"));
    ui->inputEdit->setPlaceholderText("输入发送消息(最大1kb)");
    connect(m_mainWidget,&MainWidget::group_chat_receiver,this,&GroupChat::group_chat_slot);
}

GroupChat::~GroupChat()
{
    delete ui;
}

void GroupChat::on_sendBtn_clicked()
{
    QString text=ui->inputEdit->toPlainText();
    if(text.isEmpty())
    {
        QMessageBox::information(this,"提示","发送消息不能为空!");
        return;
    }
    if(text.size()>1024)
    {
        QMessageBox::information(this,"提示","一次发送消息不能超过1kb!");
        return;
    }
    ui->inputEdit->clear();
    QString info=QString("你:%1").arg(text);
    ui->chatEdit->append(info);
    QJsonObject obj;
    obj["cmd"]="group_chat";
    obj["username"]=m_userName;
    obj["groupname"]=m_groupName;
    obj["text"]=text;
    QByteArray json=QJsonDocument(obj).toJson();
    ::sendBuf(m_socket,json);
}

void GroupChat::group_chat_slot(QJsonObject& obj)
{
    QString userName=obj["username"].toString();
    if(obj["groupname"].toString()==m_groupName)
    {
        if(this->isMinimized())
        {
            this->showNormal();
        }
        this->activateWindow();
        QString text=QString("%1:%2").arg(userName).arg(obj["text"].toString());
        ui->chatEdit->append(text);
    }
}


void GroupChat::closeEvent(QCloseEvent* event)
{
    for(auto it=m_groupInfo->begin();it!=m_groupInfo->end();it++)
    {
        if(it->groupName==m_groupName)
        {
            m_groupInfo->erase(it);
            break;
        }
    }
    event->accept();
}
