#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class CreateGroup;
}

class CreateGroup : public QWidget
{
    Q_OBJECT

public:
    explicit CreateGroup(QTcpSocket* socket,QString userName,QWidget *parent = nullptr);
    ~CreateGroup();

private slots:
    void on_cancelBtn_clicked();

    void on_createBtn_clicked();

private:
    Ui::CreateGroup *ui;
    QTcpSocket* m_socket;
    QString m_userName;
};

#endif // CREATEGROUP_H
