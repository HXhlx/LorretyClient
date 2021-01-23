#include "History.h"

History::History(QString str)
{
	setValues(str);
}

void History::Insert(QTcpSocket* tcpSocket)
{
	QStringList data = getValues();
	tcpSocket->write(QString("insert into Lorretys (ID, ptime, type, Nums, num, multi, price) values(%1 , '%2' , '%3' , '%4' , %5 , %6, %7)\n").arg(ID).arg(data[0], data[1], data[2], data[3], data[4], data[5]).toUtf8());
}

void History::Update(QTcpSocket* tcpSocket)
{}

void History::Delete(QTcpSocket* tcpSocket,quint64 no)
{
	tcpSocket->write(QString("delete from Lorretys where No = %1\n").arg(no).toUtf8());
}

void History::Select(QTcpSocket* tcpSocket, quint64 id)
{
	tcpSocket->write(QString("select * from Lorretys where ID = %1").arg(id).toUtf8());
}

void History::setValues(QString str)
{
	QStringList strs = str.trimmed().split('|');
	No = strs[0].toULongLong();
	ID = strs[1].toULongLong();
	time = QDateTime::fromString(strs[2], "yyyy-MM-ddThh:mm:ss.000");
	type = strs[3];
	nums = strs[4];
	num = strs[5].toUInt();
	multi = strs[6].toUShort();
	price = strs[7].toUInt();
}

QStringList History::getValues()
{
	return { time.toString("yyyy-MM-dd hh:mm:ss") ,type,nums,QString::number(num),QString::number(multi),QString::number(price) };
}
