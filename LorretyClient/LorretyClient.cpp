#include "LorretyClient.h"

LorretyClient::LorretyClient(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	QSettings ini("network.ini", QSettings::IniFormat);
	address=ini.value("ServerInfo/IP").toString();
	port=ini.value("ServerInfo/Port").toInt();
	tcpClient = new QTcpSocket(this);
	connect(tcpClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
}

void LorretyClient::onSocketReadyRead()
{
	auto str = tcpClient->readLine();
	if(str.isEmpty())QMessageBox::critical(this, "��¼ʧ��!", "��������˻��������벻��ȷ, ԭ�������:\n"\
		"1.�˺�����������;\n"\
		"2.��������;\n"\
		"3.δ������ĸ��Сд;\n");
	else if(str=="repeat")QMessageBox::critical(this, "��¼ʧ��!", "�������ظ���¼");
	else if(str.split('|')[1]=="user")
	{
		DoubleChromosphere* dc = new DoubleChromosphere;
		dc->show();
		dc->ID = ui.ID->currentText().toULongLong();
		dc->tcpClient->connectToHost(address, port);
		this->accept();
	}
	else if (str.split('|')[1] == "admin")
	{
		LorretyAdmin* la = new LorretyAdmin;
		la->show();
		la->tcpClient->connectToHost(address, port);
		this->accept();
	}
}

void LorretyClient::on_login_clicked()
{
	tcpClient->connectToHost(address, port);
	if (tcpClient->waitForConnected())
	{
		if (ui.ID->currentText().isEmpty())QMessageBox::warning(this, "�����쳣", "���������˺ź��ٵ�¼");
		if (ui.password->text().isEmpty())QMessageBox::warning(this, "�����쳣", "��������������ٵ�¼");
		tcpClient->write(QString("select ID, Authority from users where ID=%1 and passwd=hex('%2')").arg(ui.ID->currentText(), ui.password->text()).toUtf8());
	}
	else QMessageBox::warning(this, "��¼ʧ��!", "�����쳣");
}
