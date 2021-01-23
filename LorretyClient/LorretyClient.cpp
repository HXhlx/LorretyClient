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
	if(str.isEmpty())QMessageBox::critical(this, "登录失败!", "你输入的账户名或密码不正确, 原因可能是:\n"\
		"1.账号名输入有误;\n"\
		"2.忘记密码;\n"\
		"3.未区分字母大小写;\n");
	else if(str=="repeat")QMessageBox::critical(this, "登录失败!", "不允许重复登录");
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
		if (ui.ID->currentText().isEmpty())QMessageBox::warning(this, "输入异常", "请你输入账号后再登录");
		if (ui.password->text().isEmpty())QMessageBox::warning(this, "输入异常", "请你输入密码后再登录");
		tcpClient->write(QString("select ID, Authority from users where ID=%1 and passwd=hex('%2')").arg(ui.ID->currentText(), ui.password->text()).toUtf8());
	}
	else QMessageBox::warning(this, "登录失败!", "网络异常");
}
