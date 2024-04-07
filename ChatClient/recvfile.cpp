#include "recvfile.h"

RecvFile::RecvFile(QJsonObject& obj,QObject* parent):QObject(parent)
{
    QString fileName=obj["filename"].toString();
    QStringList nameList=fileName.split('/');
    m_fileName=nameList.at(nameList.size()-1);
    m_length=obj["file_length"].toVariant().toLongLong();
    m_port=obj["port"].toInt();}

void RecvFile::working()
{
    m_file.setFileName(m_fileName);
    m_file.open(QIODevice::WriteOnly);
    m_socket=new QTcpSocket();
    connect(m_socket,&QTcpSocket::readyRead,this,&RecvFile::recv_file_slot);
    m_socket->connectToHost(QHostAddress(IP),m_port);
    if(!m_socket->waitForConnected(10000))
    {
        delete m_socket;
        emit recv_timeout();
    }
}

void RecvFile::recv_file_slot()
{
    QByteArray data=m_socket->readAll();
    int bytes=m_file.write(data);
    if(bytes<0)
        return;
    sum+=bytes;
    qDebug()<<sum;
    if(sum>=m_length)
    {
        m_file.close();
        m_socket->close();
        delete m_socket;
        emit recv_done();
    }
}
