#ifndef ADDGROUP_H
#define ADDGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class AddGroup;
}

class AddGroup : public QWidget
{
    Q_OBJECT

public:
    explicit AddGroup(QTcpSocket* socket,QString username,QWidget *parent = nullptr);
    ~AddGroup();

private slots:
    void on_sureBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::AddGroup *ui;
    QTcpSocket* m_socket;
    QString m_userName;
};

#endif // ADDGROUP_H
