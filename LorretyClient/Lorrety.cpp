#include "Lorrety.h"

Lorrety::Lorrety(QString str)
{
	setValues(str);
}

void Lorrety::Insert(QTcpSocket* tcpSocket)
{}

void Lorrety::Update(QTcpSocket* tcpSocket)
{}

void Lorrety::Delete(QTcpSocket* tcpSocket)
{}

void Lorrety::Select(QTcpSocket* tcpSocket)
{
	tcpSocket->write(QString("select * from DoubleChromosphere order by `code` desc\n").toUtf8());
}

void Lorrety::setValues(QString str)
{
	QStringList strs = str.trimmed().split('|');
	code = strs[0].toUInt();
	date = QDate::fromString(strs[1], "yyyy-MM-dd");
	week = strs[2];
	foreach(QString s, strs[3].split(','))nums.append(s.toUShort());
	nums.append(strs[4].toUShort());
	sales = strs[5].toULong();
	poolmoney = strs[6].toULong();
	content = strs[7];
	foreach(QString s, strs[8].split(','))
	{
		QStringList ss = s.split(':');
		prizegrades << QPair<ulong, ulong>(ss[1].toULong(), ss[2].toULong());
	}
	detailsLink = strs[9];
	videoLink = strs[10];
}

QStringList Lorrety::getValues()
{
	QStringList strs;
	strs << QString::number(code) << date.toString("yyyy-MM-dd") + "(" + week + ")";
	QString snums = "";
	foreach(ushort num, nums)snums += QString::asprintf("%02d ", num);
	strs << snums.trimmed() << QString::number(sales);
	QString sprize = "";
	for (ushort i = 0; i < 6; ++i)sprize += QString::number(prizegrades[i].first) + "," + QString::number(prizegrades[i].second) + ";";
	strs << sprize.left(sprize.length() - 1) << QString::number(poolmoney);
	return strs;
}
