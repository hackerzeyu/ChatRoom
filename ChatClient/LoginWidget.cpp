#include "LoginWidget.h"
#include "ui_LoginWidget.h"
#include "tcp.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    initUI();

    //连接服务器
    m_socket=new QTcpSocket(this);
    m_socket->connectToHost(QHostAddress(IP),PORT);

    connect(m_socket,&QTcpSocket::readyRead,this,[=](){
        QByteArray json=::recvBuf(m_socket);
        QJsonDocument doc=QJsonDocument::fromJson(json);
        QJsonObject obj=doc.object();
        if(obj["cmd"].toString()=="register_reply")
        {
            if(obj["result"].toString()=="register_success")
                QMessageBox::information(this,"提示","注册成功!");
            else
                QMessageBox::information(this,"提示","用户名已经存在!");
        }
        else if(obj["cmd"].toString()=="login_reply")
        {
            login_handle(obj);
        }
    });

    connect(m_socket,&QTcpSocket::connected,this,[=](){
        QMessageBox::information(this,"提示","成功连接到远程服务器!");
    });

}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::initUI()
{
    this->setWindowIcon(QIcon(":/image/qq.jpg"));
    this->setWindowTitle("登录");
    ui->pwdEdit->setEchoMode(QLineEdit::Password);
}

void LoginWidget::login_handle(QJsonObject& obj)
{
    if(obj["result"].toString()=="login_fail")
    {
        QMessageBox::information(this,"提示","用户名或密码错误!");
    }
    else if(obj["result"].toString()=="user_login")
    {
        QMessageBox::information(this,"提示","该用户在别处登录,请检查账号安全性!");
    }
    else
    {
        QString userName=ui->userEdit->text();
        QString friendList=obj["friend_list"].toString();
        QString groupList=obj["group_list"].toString();
        QMessageBox::information(this,"提示","登录成功!");
        //屏蔽原来信号
        m_socket->disconnect(SIGNAL(readyRead()));
        this->hide();
        m_mainWidget=new MainWidget(m_socket,userName,friendList,groupList);
        m_mainWidget->show();
        QJsonObject js;
        js["cmd"]="user_online";
        js["username"]=userName;
        QByteArray json=QJsonDocument(js).toJson();
        ::sendBuf(m_socket,json);
    }
}

void LoginWidget::on_loginBtn_clicked()
{
    if(ui->userEdit->text().isEmpty() || ui->pwdEdit->text().isEmpty())
    {
        QMessageBox::information(this,"提示","用户名和密码不能为空!");
        return;
    }
    QJsonObject obj;
    obj.insert("cmd","login");
    obj.insert("username",ui->userEdit->text());
    obj.insert("password",ui->pwdEdit->text());
    QJsonDocument doc(obj);
    QByteArray json=doc.toJson();
    ::sendBuf(m_socket,json);
}

void LoginWidget::on_registerBtn_clicked()
{
    if(ui->userEdit->text().isEmpty() || ui->pwdEdit->text().isEmpty())
    {
        QMessageBox::information(this,"提示","用户名和密码不能为空!");
        return;
    }
    QJsonObject obj;
    obj.insert("cmd","register");
    obj.insert("username",ui->userEdit->text());
    obj.insert("password",ui->pwdEdit->text());
    QJsonDocument doc(obj);
    QByteArray json=doc.toJson();
    ::sendBuf(m_socket,json);
}

