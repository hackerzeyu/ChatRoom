#include "creategroup.h"
#include "ui_creategroup.h"
#include "tcp.h"

CreateGroup::CreateGroup(QTcpSocket* socket,QString userName,QWidget *parent) :
    QWidget(parent),ui(new Ui::CreateGroup),m_socket(socket),
    m_userName(userName)
{
    ui->setupUi(this);
    this->setWindowTitle("创建群聊");
}

CreateGroup::~CreateGroup()
{
    delete ui;
}

void CreateGroup::on_cancelBtn_clicked()
{
    this->close();
}


void CreateGroup::on_createBtn_clicked()
{
    QString groupName=ui->groupNameEdit->text();
    if(groupName.isEmpty())
    {
        QMessageBox::information(this,"提示","群名不能为空!");
        return;
    }
    QJsonObject obj;
    obj["cmd"]="create_group";
    obj["groupname"]=groupName;
    obj["owner"]=m_userName;
    QByteArray json=QJsonDocument(obj).toJson();
    ::sendBuf(m_socket,json);
    this->close();
}
