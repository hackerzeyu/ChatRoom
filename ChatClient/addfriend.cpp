#include "addfriend.h"
#include "ui_addfriend.h"
#include "tcp.h"

AddFriend::AddFriend(QTcpSocket* socket,QString userName,QWidget *parent) :
    QWidget(parent),ui(new Ui::AddFriend),
    m_userName(userName),m_socket(socket)
{
    ui->setupUi(this);
    this->setWindowTitle("添加好友");
}

AddFriend::~AddFriend()
{
    delete ui;
}

void AddFriend::on_sureBtn_clicked()
{
    if(ui->friendNameEdit->text().isEmpty())
    {
        QMessageBox::information(this,"提示","好友名不能为空!");
        return;
    }
    if(ui->friendNameEdit->text()==m_userName)
    {
        QMessageBox::warning(this,"警告","不能添加自己账户为好友!");
        return;
    }
    QJsonObject obj;
    obj["cmd"]="add_friend";
    obj["username"]=m_userName;
    obj["friendname"]=ui->friendNameEdit->text();
    QJsonDocument doc(obj);
    QByteArray json=doc.toJson();
    ::sendBuf(m_socket,json);
    this->close();
}

void AddFriend::on_cancelBtn_clicked()
{
    this->close();
}

