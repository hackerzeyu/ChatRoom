#include "addgroup.h"
#include "ui_addgroup.h"
#include "tcp.h"

AddGroup::AddGroup(QTcpSocket* socket,QString username,QWidget *parent) :
    QWidget(parent),ui(new Ui::AddGroup),m_socket(socket),
    m_userName(username)
{
    ui->setupUi(this);
    this->setWindowTitle("添加群");
}

AddGroup::~AddGroup()
{
    delete ui;
}

void AddGroup::on_sureBtn_clicked()
{
    QString groupName=ui->groupNameEdit->text();
    if(groupName.isEmpty())
    {
        QMessageBox::information(this,"提示","群名不能为空");
        return;
    }
    QJsonObject obj;
    obj["cmd"]="add_group";
    obj["username"]=m_userName;
    obj["groupname"]=groupName;
    QJsonDocument doc(obj);
    QByteArray json=doc.toJson();
    ::sendBuf(m_socket,json);
    this->close();
}


void AddGroup::on_cancelBtn_clicked()
{
    this->close();
}

