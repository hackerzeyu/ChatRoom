#include "tcp.h"
#include <iostream>

void sendBuf(QTcpSocket* socket,QByteArray& json)
{
    //防止粘包
    int len=json.size();
    char* buf=new char[len+4];
    //数据转大端
    int bigLen=::htonl(len);
    memcpy(buf,&bigLen,4);
    memcpy(buf+4,json.data(),len);
    socket->write(buf,len+4);
    qDebug()<<buf+4;
    delete[] buf;
}

QByteArray recvBuf(QTcpSocket* socket)
{
    QByteArray json;
    int len=0;
    int ret=socket->read((char*)&len,4);
    if(ret!=4)
        return "";
    //数据转小端
    len=::ntohl(len);
    //防止粘包
    char buf[1024]="";
    int sum=0;
    while(sum<len)
    {
        int count=socket->read(buf,sizeof(buf));
        sum+=count;
        json.append(buf);
        memset(buf,0,sizeof(buf));
    }
    qDebug()<<json;
    return json;
}

