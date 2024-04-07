#include "application.h"
#include "ui_application.h"
#include "tcp.h"

Application::Application(QTcpSocket* socket,QString sender,QString receiver,QWidget *parent) :
    QWidget(parent),ui(new Ui::Application),m_socket(socket),
    m_sender(sender),m_receiver(receiver)
{
    ui->setupUi(this);
    initUI();
}

Application::~Application()
{
    delete ui;
}

void Application::initUI()
{
    this->setWindowTitle("好友申请");
    ui->label->clear();
    QString text=QString("%1请求添加你为好友!").arg(m_sender);
    ui->label->setText(text);
}

void Application::on_agreeBtn_clicked()
{
    QJsonObject obj;
    obj["cmd"]="send_friend";
    obj["username"]=m_sender;
    obj["friendname"]=m_receiver;
    obj["result"]="agree";
    QByteArray json=QJsonDocument(obj).toJson();
    ::sendBuf(m_socket,json);
    this->close();
}

void Application::on_refuseBtn_clicked()
{
    QJsonObject obj;
    obj["cmd"]="send_friend";
    obj["friendname"]=m_receiver;
    obj["result"]="disagree";
    QByteArray json=QJsonDocument(obj).toJson();
    ::sendBuf(m_socket,json);
    this->close();
}

