#pragma once

#include <QTCPSocket>
#include <QDialog>
#include "DoubleChromosphere.h"
#include "LorretyAdmin.h"
#include "ui_LorretyClient.h"

class LorretyClient : public QDialog
{
    Q_OBJECT
public:
    LorretyClient(QWidget* parent = Q_NULLPTR);
private:
    QString address;
    quint16 port;
    QTcpSocket* tcpClient;
    Ui::LorretyClient ui;
public slots:
    void onSocketReadyRead();
    void on_login_clicked();
};
