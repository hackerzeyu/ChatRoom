#include "sendthread.h"

SendThread::SendThread(QJsonObject& obj,QObject* parent):QThread(parent),m_obj(obj)
{
    m_fileName=obj["filename"].toString();
    m_length=obj["file_length"].toVariant().toLongLong();
    m_port=obj["port"].toInt();
}

void SendThread::run()
{
    m_socket=new QTcpSocket();
    m_socket->connectToHost(QHostAddress(IP),m_port);
    if(!m_socket->waitForConnected(10000))
    {
        delete m_socket;
        this->quit();
    }
    else
    {
        QFile file(m_fileName);
        if(!file.open(QIODevice::ReadOnly))
        {
            m_socket->close();
            delete m_socket;
            this->quit();
        }
        long long sum=0;
        while(true)
        {
            QByteArray data=file.read(1024*1024);
            int bytes=m_socket->write(data);
            if(bytes<=0)
            {
                break;
            }
            sum+=bytes;
            m_socket->flush();
            //降低cpu负荷
            sleep(2);
            emit progress_signal(sum*100/m_length,m_obj);
            if(sum>=m_length)
                break;
        }
        if(m_socket->waitForDisconnected(-1))
        {
            qDebug()<<"send close";
            file.close();
            m_socket->close();
            delete m_socket;
            emit send_end();
            this->quit();
        }
    }
}
