#include "LorretyAdmin.h"

LorretyAdmin::LorretyAdmin(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	tcpClient = new QTcpSocket(this);
	connect(tcpClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
	connect(tcpClient, SIGNAL(connected()), this, SLOT(onConnected()));
	ui.Purchase->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

LorretyAdmin::~LorretyAdmin()
{}

void LorretyAdmin::onConnected()
{
	tcpClient->write(QString("select * from Lorretys").toUtf8());
}

void LorretyAdmin::InsertHistory(History h)
{
	ui.Purchase->insertRow(0);
	QStringList data = h.getValues();
	for (ushort i = 0; i < data.size(); ++i)
	{
		QTableWidgetItem* item = new QTableWidgetItem(data[i]);
		item->setTextAlignment(Qt::AlignCenter);
		if (i == 0)item->setData(Qt::UserRole, h.No);
		ui.Purchase->setItem(0, i, item);
	}
}

void LorretyAdmin::onSocketReadyRead()
{
	while (tcpClient->canReadLine())
	{
		auto str = tcpClient->readLine();
		if (str[0] == 'l')
		{
			History history(QString(str.right(str.length() - 2)));
			histories[history.No] = history;
			InsertHistory(history);
		}
	}
	ui.Purchase->resizeColumnsToContents();
}

void LorretyAdmin::on_refresh_clicked()
{
	ui.Purchase->clearContents();
	ui.Purchase->setRowCount(0);
	onConnected();
}
