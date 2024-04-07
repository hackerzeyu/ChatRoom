#ifndef RECVFILE_H
#define RECVFILE_H

#include <QObject>
#include <QMessageBox>
#include <QFile>
#include <QHostAddress>
#include <QTcpSocket>
#include <QJsonObject>

#define IP "101.132.163.255"

class RecvFile:public QObject
{
    Q_OBJECT
public:
    RecvFile(QJsonObject& obj,QObject* parent=nullptr);
    void working();
    void recv_file_slot();
signals:
    void recv_timeout();
    void recv_done();
private:
    QString m_fileName;
    long long m_length;
    int m_port;
    QTcpSocket* m_socket;
    QFile m_file;
    long long sum=0;
};

#endif // RECVFILE_H
