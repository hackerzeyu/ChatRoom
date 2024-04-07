#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include <QCloseEvent>
#include <QMessageBox>
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QFileDialog>
#include <QProgressBar>
struct ChatInfo;
class MainWidget;
#include "mainwidget.h"
#include "sendthread.h"

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QTcpSocket* socket,QString userName,QString chatName,MainWidget* mainWidget,QList<ChatInfo>* chatInfo,QWidget *parent = nullptr);
    ~PrivateChat();
    QString getChatName(){return m_chatName;}
    void closeEvent(QCloseEvent* event) override;
private slots:
    void on_sendBtn_clicked();
    void send_slot(QJsonObject& obj);
    void recv_slot(QJsonObject& obj);
    void offline_slot(QJsonObject& obj);
    void on_fileBtn_clicked();
    void send_file_slot(QJsonObject& obj);
    void progress_slot(int value,QJsonObject obj);
    void sender_timeout_slot(QJsonObject& obj);
    void receiver_timeout_slot(QJsonObject& obj);
private:
    Ui::PrivateChat *ui;
    QString m_userName;             //用户名
    QString m_chatName;             //聊天名
    QTcpSocket* m_socket;
    MainWidget* m_mainWidget;
    QList<ChatInfo>* m_chatInfo;   //聊天窗口
    SendThread* m_sendThread;      //发送线程
};

#endif // PRIVATECHAT_H
