#pragma once

#include <QWidget>
#include "History.h"
#include "ui_LorretyAdmin.h"

class LorretyAdmin : public QWidget
{
	Q_OBJECT
public:
	QTcpSocket* tcpClient;
	LorretyAdmin(QWidget *parent = Q_NULLPTR);
	~LorretyAdmin();
private:
	QMap<quint64, History>histories;
	Ui::LorretyAdmin ui;
	void InsertHistory(History h);
public slots:
	void onConnected();
	void onSocketReadyRead();
	void on_refresh_clicked();
};
