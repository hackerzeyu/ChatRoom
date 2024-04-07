#ifndef APPLICATION_H
#define APPLICATION_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

//好友申请
namespace Ui {
class Application;
}

class Application : public QWidget
{
    Q_OBJECT

public:
    explicit Application(QTcpSocket* socket,QString sender,QString receiver,QWidget *parent = nullptr);
    ~Application();
    void initUI();
private slots:
    void on_agreeBtn_clicked();
    void on_refuseBtn_clicked();

private:
    Ui::Application *ui;
    QTcpSocket* m_socket;
    QString m_sender;       //好友申请发送者
    QString m_receiver;     //好友申请接收者
};

#endif // APPLICATION_H
