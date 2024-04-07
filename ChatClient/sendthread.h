#ifndef SENDTHREAD_H
#define SENDTHREAD_H

#include <QThread>
#include <QFile>
#include <QJsonObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>

#define IP "101.132.163.255"

class SendThread:public QThread
{
    Q_OBJECT
public:
    SendThread(QJsonObject& obj,QObject* parent=nullptr);
protected:
    void run() override;
    void send_success_slot();
signals:
    void progress_signal(int value,QJsonObject obj);
    void send_end();
private:
    QJsonObject m_obj;
    QString m_userName;
    QString m_fileName;
    long long m_length;
    int m_port;
    QTcpSocket* m_socket;
};

#endif // SENDTHREAD_H
