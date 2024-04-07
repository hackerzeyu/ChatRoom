#include "privatechat.h"
#include "ui_privatechat.h"
#include "tcp.h"

PrivateChat::PrivateChat(QTcpSocket* socket,QString userName,QString chatName,MainWidget* mainWidget,QList<ChatInfo>* chatInfo,QWidget *parent) :
    QWidget(parent),ui(new Ui::PrivateChat),m_userName(userName),m_chatName(chatName),m_socket(socket),m_mainWidget(mainWidget),
    m_chatInfo(chatInfo)
{
    ui->setupUi(this);
    this->setWindowTitle(m_chatName);
    this->setWindowIcon(QIcon(":/image/qq.jpg"));
    ui->inputEdit->setPlaceholderText("输入发送消息(最大1kb)");
    ui->progressBar->setValue(0);
    //处理MainWidget发送的信号
    connect(m_mainWidget,&MainWidget::private_chat_offline,this,&PrivateChat::offline_slot);
    connect(m_mainWidget,&MainWidget::private_chat_sender,this,&PrivateChat::send_slot);
    connect(m_mainWidget,&MainWidget::private_chat_receiver,this,&PrivateChat::recv_slot);
    connect(m_mainWidget,&MainWidget::send_file_start,this,&PrivateChat::send_file_slot);
    connect(m_mainWidget,&MainWidget::sender_timeout,this,&PrivateChat::sender_timeout_slot);
    connect(m_mainWidget,&MainWidget::receiver_timeout,this,&PrivateChat::receiver_timeout_slot);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::on_sendBtn_clicked()
{
    QString text=ui->inputEdit->toPlainText();
    if(text.isEmpty())
    {
        QMessageBox::information(this,"提示","发送消息不能为空!");
        return;
    }
    if(text.length()>1024)
    {
        QMessageBox::information(this,"提示","一次发送不能超过1kb大小");
        return;
    }
    QJsonObject obj;
    obj["cmd"]="private_chat";
    obj["username"]=m_userName;
    obj["chatname"]=m_chatName;
    obj["text"]=text;
    QJsonDocument doc(obj);
    QByteArray json=doc.toJson();
    ::sendBuf(m_socket,json);
}

void PrivateChat::send_slot(QJsonObject& obj)
{
    if(obj["cmd"].toString()=="private_chat_reply")
    {
        QString user=obj["username"].toString();
        QString chatname=obj["chatname"].toString();
        if(m_userName==user && m_chatName==chatname)
        {
            QString text=ui->inputEdit->toPlainText();
            QString info=QString("你:%2").arg(text);
            ui->chatEdit->append(info);
            ui->inputEdit->clear();
        }
    }
}

void PrivateChat::recv_slot(QJsonObject &obj)
{
    if(obj["cmd"].toString()=="recv_private_chat")
    {
        QString username=obj["username"].toString();
        QString chatname=obj["chatname"].toString();
        if(m_userName==chatname && m_chatName==username)
        {
            if(this->isMinimized())
            {
                this->showNormal();
            }
            this->activateWindow();
            QString text=obj["text"].toString();
            QString info=QString("%1:%2").arg(username).arg(text);
            ui->chatEdit->append(info);
        }
    }
}

void PrivateChat::offline_slot(QJsonObject &obj)
{
    if(obj["cmd"].toString()=="private_chat_reply" || obj["cmd"].toString()=="send_file_reply")
    {
        QString username=obj["username"].toString();
        QString chatname=obj["chatname"].toString();
        if(m_userName==username && m_chatName==chatname)
        {
            QString info=QString("%1不在线").arg(chatname);
            QMessageBox::information(this,"提示",info);
        }
    }
}

void PrivateChat::closeEvent(QCloseEvent* event)
{
    //删除聊天框对象
    for(auto it=m_chatInfo->begin();it!=m_chatInfo->end();it++)
    {
        if(it->name==m_userName && it->m_chat->getChatName()==m_chatName)
        {
            m_chatInfo->erase(it);
            break;
        }
    }
    event->accept();
}

void PrivateChat::on_fileBtn_clicked()
{
    QString fileName=QFileDialog::getOpenFileName(this,"文件",QCoreApplication::applicationDirPath());
    if(fileName.isEmpty())
    {
        return;
    }
    QJsonObject obj;
    obj["cmd"]="send_file";
    obj["username"]=m_userName;
    obj["chatname"]=m_chatName;
    obj["filename"]=fileName;
    QFile file;
    file.setFileName(fileName);
    long long length=file.size();
    obj["file_length"]=length;
    QByteArray json=QJsonDocument(obj).toJson();
    ::sendBuf(m_socket,json);
}

void PrivateChat::send_file_slot(QJsonObject &obj)
{
    QString userName=obj["username"].toString();
    QString chatName=obj["chatname"].toString();
    if(m_userName==userName && m_chatName==chatName)
    {
        ui->progressBar->setValue(0);
        //启动发送线程
        m_sendThread=new SendThread(obj);
        m_sendThread->start();
        connect(m_sendThread,&SendThread::progress_signal,this,&PrivateChat::progress_slot);
        connect(m_sendThread,&SendThread::send_end,this,[=](){
            QMessageBox::information(this,"提示","对方接收完毕!");
        });
    }
}

void PrivateChat::progress_slot(int value, QJsonObject obj)
{
    QString userName=obj["username"].toString();
    QString chatName=obj["chatname"].toString();
    if(userName==m_userName && m_chatName==chatName)
    {
        ui->progressBar->setValue(value);
    }
}

void PrivateChat::sender_timeout_slot(QJsonObject &obj)
{
    QString userName=obj["sender"].toString();
    QString chatName=obj["receiver"].toString();
    if(m_userName==userName && m_chatName==chatName)
    {
        QMessageBox::information(this,"提示","连接超时!");
    }
}

void PrivateChat::receiver_timeout_slot(QJsonObject &obj)
{
    QString userName=obj["sender"].toString();
    QString chatName=obj["receiver"].toString();
    if(m_userName==userName && m_chatName==chatName)
    {
        QMessageBox::information(this,"提示","接收方连接超时!");
    }
}





