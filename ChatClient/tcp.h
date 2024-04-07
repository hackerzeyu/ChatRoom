#ifndef TCP_H
#define TCP_H
#include <QByteArray>
#include <QTcpSocket>
#include <winsock2.h>

//发送数据
void sendBuf(QTcpSocket* socket,QByteArray& json);
QByteArray recvBuf(QTcpSocket* socket);

#endif // TCP_H
