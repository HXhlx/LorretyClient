#pragma once

#include <QButtonGroup>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTableWidget>
#include <QTCPSocket>
#include <QWidget>
#include <QtCharts>
#include <algorithm>
#include <valarray>
#include <random>
#include "Lorrety.h"
#include "History.h"
#include "ui_DoubleChromosphere.h"

using namespace QtCharts;
using namespace std;

class DoubleChromosphere : public QWidget
{
	Q_OBJECT
public:
	quint64 ID;
	QTcpSocket* tcpClient;
	DoubleChromosphere(QWidget* parent = Q_NULLPTR);
	~DoubleChromosphere();
private:
	QMap<quint64,History>puchases;
	QMap<quint64, Lorrety>lorretys;
	QButtonGroup* PR, * PB, * DR, * TR, * DTB, * week;
	QVector<ushort>red, blue;
	Ui::DoubleChromosphere ui;
	ushort pr = 0, pb = 0, dr = 0, Tr = 0, dtb = 0, trp = 6, tbp = 1, tdrp = 1, ttrp = 1, tdtbp = 1, mdrp = 0, mtrp = 0;
	quint64 combination(ushort n, ushort m = 6);
	void Charts(QChartView*view);
	void InsertBlues(Lorrety l);
	void InsertLorrety(Lorrety l);
	void InsertHistory(History h);
	void InsertReds(Lorrety l);
	void closeEvent(QCloseEvent* event);
	void initButton();
	void showLorretyPrice(function<quint64(ushort, ushort)> f);
private slots:
	void on_CID_currentIndexChanged(const QString& text);
	void on_Code_currentIndexChanged(const QString& text);
	void on_Delete_clicked();
	void on_DRC_clicked();
	void on_DTBC_clicked();
	void on_DTC_clicked();
	void on_DTP_valueChanged(int dtp);
	void on_DTPur_clicked();
	void on_DTT_clicked();
	void on_LID_currentIndexChanged(const QString& text);
	void on_MaxTab_currentChanged(int flag);
	void on_MDRP_currentIndexChanged(const QString& text);
	void on_MTRP_currentIndexChanged(const QString& text);
	void on_NC_clicked();
	void on_NormalT_clicked();
	void on_PBC_clicked();
	void on_PBR_clicked();
	void on_PP_valueChanged(int pp);
	void on_PPur_clicked();
	void on_PRC_clicked();
	void on_PRR_clicked();
	void on_Purchase_cellChanged(int row, int colume);
	void on_TDRP_currentIndexChanged(const QString& text);
	void on_TDTBP_currentIndexChanged(const QString& text);
	void on_TTRP_currentIndexChanged(const QString& text);
	void on_TBP_currentIndexChanged(const QString& text);
	void on_TRC_clicked();
	void on_TRP_currentIndexChanged(const QString& text);
	void onConnected();
	void onDisconnected();
	void onSocketReadyRead();
	void buttonPR(int br);
	void buttonPB(int br);
	void buttonDR(int br);
	void buttonTR(int br);
	void buttonDTB(int br);
	void buttonWeek(int bw);
};
