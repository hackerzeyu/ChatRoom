#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QWidget>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>

//添加好友
namespace Ui {
class AddFriend;
}

class AddFriend : public QWidget
{
    Q_OBJECT

public:
    explicit AddFriend(QTcpSocket* socket,QString userName,QWidget *parent = nullptr);
    ~AddFriend();

private slots:
    void on_sureBtn_clicked();
    void on_cancelBtn_clicked();

private:
    Ui::AddFriend *ui;
    QString m_userName;
    QTcpSocket* m_socket;
};

#endif // ADDFRIEND_H
