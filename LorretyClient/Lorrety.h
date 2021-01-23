#pragma once
#pragma execution_character_set("utf-8")

#include <QDateTime>
#include <QHash>
#include <QTcpSocket>

struct Lorrety
{
	QDate date;
	QList<ushort>nums;
	QVector<QPair<ulong,ulong>>prizegrades;
	QString content, detailsLink, videoLink, week;
	uint code;
	ulong sales, poolmoney;
	Lorrety() = default;
	Lorrety(QString str);
	void Insert(QTcpSocket* tcpSocket);
	void Update(QTcpSocket* tcpSocket);
	void Delete(QTcpSocket* tcpSocket);
	static void Select(QTcpSocket* tcpSocket);
	void setValues(QString str);
	QStringList getValues();
};
