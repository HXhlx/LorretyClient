#pragma once
#pragma execution_character_set("utf-8")

#include <QDateTime>
#include <QTcpSocket>

struct History
{
	quint64 No,ID;
	QDateTime time;
	QString type, nums;
	uint num, price;
	ushort multi;
	History() = default;
	History(QString str);
	void Insert(QTcpSocket* tcpSocket);
	void Update(QTcpSocket* tcpSocket);
	static void Delete(QTcpSocket* tcpSocket, quint64 no);
	static void Select(QTcpSocket* tcpSocket,quint64 id);
	void setValues(QString str);
	QStringList getValues();
};

