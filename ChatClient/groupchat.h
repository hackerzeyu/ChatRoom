#ifndef GROUPCHAT_H
#define GROUPCHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include "QCloseEvent"
struct GroupInfo;
class MainWidget;
#include "mainwidget.h"

namespace Ui {
class GroupChat;
}

class GroupChat : public QWidget
{
    Q_OBJECT

public:
    explicit GroupChat(QTcpSocket* socket,QString userName,QString groupName,MainWidget* mainWidget,QList<GroupInfo>* groupInfo,QWidget *parent = nullptr);
    ~GroupChat();
    void closeEvent(QCloseEvent* event) override;

private slots:
    void on_sendBtn_clicked();
    void group_chat_slot(QJsonObject& obj);
private:
    Ui::GroupChat *ui;
    QTcpSocket* m_socket;
    QString m_userName;
    QString m_groupName;
    QList<GroupInfo>* m_groupInfo;
    MainWidget* m_mainWidget;
};

#endif // GROUPCHAT_H
