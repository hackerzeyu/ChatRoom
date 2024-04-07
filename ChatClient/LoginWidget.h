#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include "mainwidget.h"
#include "tcp.h"

#define IP "101.132.163.255"
#define PORT 8000


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class LoginWidget : public QWidget
{
    Q_OBJECT
public:
    LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
    void login_handle(QJsonObject& obj);
private slots:
    void on_loginBtn_clicked();
    void on_registerBtn_clicked();

private:
    void initUI();

private:
    Ui::Widget *ui;
    QTcpSocket* m_socket;
    MainWidget* m_mainWidget;
};
#endif // LOGINWIDGET_H
