#include "DoubleChromosphere.h"

quint64 DoubleChromosphere::combination(ushort n, ushort m)
{
	if (n < m)return 0;
	quint64 c = 1;
	for (ushort i = 0; i < m; ++i)c = c * (n - i) / (i + 1);
	return c;
}

DoubleChromosphere::DoubleChromosphere(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	for (ushort i = 1; i <= 33; ++i)red.push_back(i);
	for (ushort i = 1; i <= 16; ++i)blue.push_back(i);
	tcpClient = new QTcpSocket(this);
	QTableWidgetItem* headerItem;
	QStringList headerText;
	headerText << "期号" << "开奖日期" << "开奖号码" << "总销售额(元)" << "奖项" << "奖池(元)";
	ui.history->setColumnCount(headerText.count());
	for (quint64 i = 0; i < ui.history->columnCount(); ++i)
	{
		headerItem = new QTableWidgetItem(headerText.at(i));
		QFont font = headerItem->font();
		font.setBold(true);
		font.setPixelSize(12);
		headerItem->setTextColor(Qt::black);
		headerItem->setFont(font);
		headerItem->setBackgroundColor(Qt::blue);
		ui.history->setHorizontalHeaderItem(i, headerItem);
	}
	ui.history->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.currentLorrety->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.lorretyPrice->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.Reds->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.Blues->setEditTriggers(QAbstractItemView::NoEditTriggers);
	initButton();
	connect(tcpClient, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(tcpClient, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(tcpClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
}

DoubleChromosphere::~DoubleChromosphere()
{}

void DoubleChromosphere::on_CID_currentIndexChanged(const QString& text)
{
	Lorrety& lorrety = lorretys[text.toULongLong()];
	ui.CR1->setText(QString::asprintf("%02d", lorrety.nums[0]));
	ui.CR2->setText(QString::asprintf("%02d", lorrety.nums[1]));
	ui.CR3->setText(QString::asprintf("%02d", lorrety.nums[2]));
	ui.CR4->setText(QString::asprintf("%02d", lorrety.nums[3]));
	ui.CR5->setText(QString::asprintf("%02d", lorrety.nums[4]));
	ui.CR6->setText(QString::asprintf("%02d", lorrety.nums[5]));
	ui.CB->setText(QString::asprintf("%02d", lorrety.nums[6]));
	for (quint64 i = 0; i < 2; ++i)
	{
		QTableWidgetItem* item = new QTableWidgetItem(QString::number(lorrety.prizegrades[i].second) + "元");
		item->setTextAlignment(Qt::AlignCenter);
		ui.lorretyPrice->setItem(i, 3, item);
	}
}

void DoubleChromosphere::on_DTPur_clicked()
{
	History puchase;
	QStringList Dan, Tuo, Blues;
	foreach(auto button, DR->buttons())
		if (button->isChecked())Dan << button->text();
	foreach(auto button, TR->buttons())
		if (button->isChecked())Tuo << button->text();
	foreach(auto button, DTB->buttons())
		if (button->isChecked())Blues << button->text();
	puchase.ID = ID;
	puchase.time = QDateTime::currentDateTime();
	puchase.type = "胆拖";
	puchase.nums = QString("胆: ") + Dan.join(' ') + " 拖: " + Tuo.join(' ') + " 蓝: " + Blues.join(' ');
	puchase.num = combination(Tuo.size(), 6 - Dan.size()) * Blues.size();
	puchase.multi = ui.PP->value();
	puchase.price = 2 * puchase.multi * puchase.num;
	puchases.isEmpty() ? puchase.No = 0 : puchase.No = puchases.lastKey();
	puchases[puchase.No + 1] = puchase;
	puchase.Insert(tcpClient);
	InsertHistory(puchase);
	ui.Purchase->resizeColumnsToContents();
}

void DoubleChromosphere::on_DTT_clicked()
{
	ui.stackedWidget->setCurrentIndex(0);
}

void DoubleChromosphere::on_DRC_clicked()
{
	dr = 0;
	for (ushort i = 1; i <= 33; ++i)qobject_cast<QCheckBox*>(DR->button(i))->setChecked(false);
	ui.DTPur->setEnabled(false);
	on_DTP_valueChanged(ui.DTP->value());
}

void DoubleChromosphere::on_Code_currentIndexChanged(const QString& text)
{
	ui.history->clearContents();
	ui.history->setRowCount(0);
	QRegExp reg("\\D");
	QString str = text;
	quint64 num = str.remove(reg).toULongLong();
	int curRow = ui.history->rowCount();
	for (auto it = lorretys.end() - 1; curRow < num; curRow = ui.history->rowCount(), --it)InsertLorrety(*it);
	ui.history->resizeColumnsToContents();
}

void DoubleChromosphere::on_Delete_clicked()
{
	int rows = ui.Purchase->rowCount();
	for (quint64 i = 0; i < rows; ++i)
	{
		auto item = ui.Purchase->item(i, 0);
		if (item->checkState() == Qt::Checked)
		{
			quint64 no = item->data(Qt::UserRole).toULongLong();
			History::Delete(tcpClient, no);
			puchases.remove(no);
		}
	}
	ui.Purchase->clearContents();
	ui.Purchase->setRowCount(0);
	for (auto it = puchases.begin(); it != puchases.end(); ++it)InsertHistory(*it);
}

void DoubleChromosphere::on_DTP_valueChanged(int dtp)
{
	quint64 res = dr ? combination(Tr, 6 - dr) * dtb : 0;
	ui.DantuoResult->setText(QString("共%1注,%2倍,总计%3元").arg(res).arg(dtp).arg(2 * res * dtp));
}

void DoubleChromosphere::on_DTBC_clicked()
{
	dtb = 0;
	for (ushort i = 1; i <= 16; ++i)qobject_cast<QCheckBox*>(DTB->button(i))->setChecked(false);
	ui.DTPur->setEnabled(false);
	on_DTP_valueChanged(ui.DTP->value());
}

void DoubleChromosphere::on_DTC_clicked()
{
	ushort mdtbp = ui.MDTBP->currentText().left(ui.MDRP->currentText().size() - 1).toUShort();
	showLorretyPrice([this, mdtbp](ushort r, ushort b) {return r < mdrp || r + tdrp>6 + mdrp ? 0 : combination(tdtbp - mdtbp, mdtbp - b) * combination(ttrp - mtrp, mtrp + mdrp - r) * combination(mtrp, r - mdrp);	});
}

void DoubleChromosphere::on_LID_currentIndexChanged(const QString& text)
{
	Lorrety& lorrety = lorretys[text.toULongLong()];
	ui.LR1->setText(QString::asprintf("%02d", lorrety.nums[0]));
	ui.LR2->setText(QString::asprintf("%02d", lorrety.nums[1]));
	ui.LR3->setText(QString::asprintf("%02d", lorrety.nums[2]));
	ui.LR4->setText(QString::asprintf("%02d", lorrety.nums[3]));
	ui.LR5->setText(QString::asprintf("%02d", lorrety.nums[4]));
	ui.LR6->setText(QString::asprintf("%02d", lorrety.nums[5]));
	ui.LB->setText(QString::asprintf("%02d", lorrety.nums[6]));
	ui.LorretyDate->setText(QString("开奖日期: ") + lorrety.date.toString("yyyy-MM-dd"));
	for (quint64 i = 0; i < 6; ++i)
	{
		QTableWidgetItem* item = new QTableWidgetItem(QString::number(lorrety.prizegrades[i].first));
		item->setTextAlignment(Qt::AlignCenter);
		ui.currentLorrety->setItem(i, 1, item);
		item = new QTableWidgetItem(QString::number(lorrety.prizegrades[i].second));
		item->setTextAlignment(Qt::AlignCenter);
		ui.currentLorrety->setItem(i, 2, item);
	}
}

void DoubleChromosphere::on_MaxTab_currentChanged(int flag)
{
	if (flag == 5)
	{
		ui.Purchase->clearContents();
		ui.Purchase->setRowCount(0);
		History::Select(tcpClient, ID);
	}
}

void DoubleChromosphere::on_MDRP_currentIndexChanged(const QString& text)
{
	mdrp = text.left(text.size() - 1).toUShort();
	if (mdrp + mtrp > 6)
	{
		mtrp = 6 - mdrp;
		ui.MTRP->setCurrentIndex(mtrp);
	}
}

void DoubleChromosphere::on_MTRP_currentIndexChanged(const QString& text)
{
	mtrp = text.left(text.size() - 1).toUShort();
	if (mdrp + mtrp > 6)
	{
		mdrp = 6 - mtrp;
		ui.MDRP->setCurrentIndex(mdrp);
	}
}

void DoubleChromosphere::on_PBC_clicked()
{
	pb = 0;
	for (ushort i = 1; i <= 16; ++i)qobject_cast<QCheckBox*>(PB->button(i))->setChecked(false);
	ui.PPur->setEnabled(false);
	on_PP_valueChanged(ui.PP->value());
}

void DoubleChromosphere::on_PBR_clicked()
{
	on_PBC_clicked();
	QVector<ushort>Blue = blue;
	std::random_shuffle(Blue.begin(), Blue.end());
	pb = ui.PBP->currentText().split("个")[0].toUShort();
	for (ushort i = 0; i < pb; ++i)qobject_cast<QCheckBox*>(PB->button(Blue[i]))->setChecked(true);
	on_PP_valueChanged(ui.PP->value());
	pr <= 20 && pr >= 6 && pb > 0 ? ui.PPur->setEnabled(true) : ui.PPur->setEnabled(false);
}

void DoubleChromosphere::on_PP_valueChanged(int pp)
{
	quint64 res = combination(pr) * pb;
	ui.NormalResult->setText(QString("共%1注,%2倍,总计%3元").arg(res).arg(pp).arg(2 * res * pp));
}

void DoubleChromosphere::on_PPur_clicked()
{
	History puchase;
	QStringList Reds, Blues;
	foreach(auto button, PR->buttons())
		if (button->isChecked())Reds << button->text();
	foreach(auto button, PB->buttons())
		if (button->isChecked())Blues << button->text();
	puchase.ID = ID;
	puchase.time = QDateTime::currentDateTime();
	Reds.size() == 6 && Blues.size() == 1 ? puchase.type = "单式" : puchase.type = "复式";
	puchase.nums = QString("红: ") + Reds.join(' ') + " 蓝: " + Blues.join(' ');
	puchase.num = combination(Reds.size()) * Blues.size();
	puchase.multi = ui.PP->value();
	puchase.price = 2 * puchase.multi * puchase.num;
	puchases.isEmpty() ? puchase.No = 0 : puchase.No = puchases.lastKey();
	puchases[puchase.No + 1] = puchase;
	puchase.Insert(tcpClient);
	InsertHistory(puchase);
	ui.Purchase->resizeColumnsToContents();
}

void DoubleChromosphere::on_PRC_clicked()
{
	pr = 0;
	for (ushort i = 1; i <= 33; ++i)qobject_cast<QCheckBox*>(PR->button(i))->setChecked(false);
	ui.PPur->setEnabled(false);
	on_PP_valueChanged(ui.PP->value());
}

void DoubleChromosphere::on_PRR_clicked()
{
	on_PRC_clicked();
	QVector<ushort>Red = red;
	std::random_shuffle(Red.begin(), Red.end());
	pr = ui.PRP->currentText().split("个")[0].toUShort();
	for (ushort i = 0; i < pr; ++i)qobject_cast<QCheckBox*>(PR->button(Red[i]))->setChecked(true);
	on_PP_valueChanged(ui.PP->value());
	pr <= 20 && pr >= 6 && pb > 0 ? ui.PPur->setEnabled(true) : ui.PPur->setEnabled(false);
}

void DoubleChromosphere::on_Purchase_cellChanged(int row, int colume)
{
	switch (ui.Purchase->item(row, 0)->checkState())
	{
	case Qt::Unchecked:ui.Delete->setEnabled(false); break;
	case Qt::Checked:ui.Delete->setEnabled(true); break;
	}

}

void DoubleChromosphere::on_TDRP_currentIndexChanged(const QString& text)
{
	tdrp = text.left(text.length() - 1).toShort();
	ushort m = ui.MDRP->count() - 1;
	for (; m > tdrp; --m)ui.MDRP->removeItem(m);
	while (m < tdrp)ui.MDRP->addItem(QString::number(++m) + "个");
	if (mdrp > m)mdrp = m;
	if (tdrp + ttrp > 33)
	{
		ttrp = 33 - tdrp;
		ui.TTRP->setCurrentIndex(ttrp - 1);
	}
	quint64 res = combination(ttrp, 6 - tdrp) * tdtbp;
	ui.DTR->setText(QString("投注金额: 共计%1注, %2元").arg(res).arg(res * 2));
}

void DoubleChromosphere::on_TDTBP_currentIndexChanged(const QString& text)
{
	tdtbp = text.left(text.size() - 1).toUShort();
	quint64 res = combination(ttrp, 6 - tdrp) * tdtbp;
	ui.DTR->setText(QString("投注金额: 共计%1注, %2元").arg(res).arg(res * 2));
}

void DoubleChromosphere::on_TTRP_currentIndexChanged(const QString& text)
{
	ttrp = text.left(text.size() - 1).toUShort();
	ushort n = ttrp > 6 ? 6 : ttrp, m = ui.MTRP->count() - 1;
	for (; m > n; --m)ui.MTRP->removeItem(m);
	while (m < n)ui.MTRP->addItem(QString::number(++m) + "个");
	if (mtrp > m)mtrp = m;
	if (tdrp + ttrp > 33)
	{
		tdrp = 33 - ttrp;
		ui.TTRP->setCurrentIndex(tdrp - 1);
	}
	quint64 res = combination(ttrp, 6 - tdrp) * tdtbp;
	ui.DTR->setText(QString("投注金额: 共计%1注, %2元").arg(res).arg(res * 2));
}

void DoubleChromosphere::on_TBP_currentIndexChanged(const QString& text)
{
	tbp = text.left(text.size() - 1).toUShort();
	quint64 res = combination(trp) * tbp;
	ui.NR->setText(QString("投注金额: 共计%1注, %2元").arg(res).arg(res * 2));
}

void DoubleChromosphere::on_TRC_clicked()
{
	Tr = 0;
	for (ushort i = 1; i <= 33; ++i)qobject_cast<QCheckBox*>(TR->button(i))->setChecked(false);
	ui.DTPur->setEnabled(false);
	on_DTP_valueChanged(ui.DTP->value());
}

void DoubleChromosphere::on_TRP_currentIndexChanged(const QString& text)
{
	trp = text.left(text.size() - 1).toUShort();
	quint64 res = combination(trp) * tbp;
	ui.NR->setText(QString("投注金额: 共计%1注, %2元").arg(res).arg(res * 2));
}

void DoubleChromosphere::on_NC_clicked()
{
	ushort mrp = ui.MRP->currentText().left(ui.MRP->currentText().size() - 1).toUShort(), mbp = ui.MBP->currentText().left(ui.MBP->currentText().size() - 1).toUShort();
	showLorretyPrice([this, mrp, mbp](ushort r, ushort b) {return combination(trp - mrp, mrp - r) * combination(mrp, r) * combination(tbp - mbp, mbp - b); });
}

void DoubleChromosphere::on_NormalT_clicked()
{
	ui.stackedWidget->setCurrentIndex(1);
}

void DoubleChromosphere::onConnected()
{
	Lorrety::Select(tcpClient);
}

void DoubleChromosphere::initButton()
{
	PR = new QButtonGroup(this);
	PR->addButton(ui.PR1, 1);
	PR->addButton(ui.PR2, 2);
	PR->addButton(ui.PR3, 3);
	PR->addButton(ui.PR4, 4);
	PR->addButton(ui.PR5, 5);
	PR->addButton(ui.PR6, 6);
	PR->addButton(ui.PR7, 7);
	PR->addButton(ui.PR8, 8);
	PR->addButton(ui.PR9, 9);
	PR->addButton(ui.PR10, 10);
	PR->addButton(ui.PR11, 11);
	PR->addButton(ui.PR12, 12);
	PR->addButton(ui.PR13, 13);
	PR->addButton(ui.PR14, 14);
	PR->addButton(ui.PR15, 15);
	PR->addButton(ui.PR16, 16);
	PR->addButton(ui.PR17, 17);
	PR->addButton(ui.PR18, 18);
	PR->addButton(ui.PR19, 19);
	PR->addButton(ui.PR20, 20);
	PR->addButton(ui.PR21, 21);
	PR->addButton(ui.PR22, 22);
	PR->addButton(ui.PR23, 23);
	PR->addButton(ui.PR24, 24);
	PR->addButton(ui.PR25, 25);
	PR->addButton(ui.PR26, 26);
	PR->addButton(ui.PR27, 27);
	PR->addButton(ui.PR28, 28);
	PR->addButton(ui.PR29, 29);
	PR->addButton(ui.PR30, 30);
	PR->addButton(ui.PR31, 31);
	PR->addButton(ui.PR32, 32);
	PR->addButton(ui.PR33, 33);
	PR->setExclusive(false);
	connect(PR, SIGNAL(buttonClicked(int)), this, SLOT(buttonPR(int)));
	PB = new QButtonGroup(this);
	PB->addButton(ui.PB1, 1);
	PB->addButton(ui.PB2, 2);
	PB->addButton(ui.PB3, 3);
	PB->addButton(ui.PB4, 4);
	PB->addButton(ui.PB5, 5);
	PB->addButton(ui.PB6, 6);
	PB->addButton(ui.PB7, 7);
	PB->addButton(ui.PB8, 8);
	PB->addButton(ui.PB9, 9);
	PB->addButton(ui.PB10, 10);
	PB->addButton(ui.PB11, 11);
	PB->addButton(ui.PB12, 12);
	PB->addButton(ui.PB13, 13);
	PB->addButton(ui.PB14, 14);
	PB->addButton(ui.PB15, 15);
	PB->addButton(ui.PB16, 16);
	PB->setExclusive(false);
	connect(PB, SIGNAL(buttonClicked(int)), this, SLOT(buttonPB(int)));
	DR = new QButtonGroup(this);
	DR->addButton(ui.DR1, 1);
	DR->addButton(ui.DR2, 2);
	DR->addButton(ui.DR3, 3);
	DR->addButton(ui.DR4, 4);
	DR->addButton(ui.DR5, 5);
	DR->addButton(ui.DR6, 6);
	DR->addButton(ui.DR7, 7);
	DR->addButton(ui.DR8, 8);
	DR->addButton(ui.DR9, 9);
	DR->addButton(ui.DR10, 10);
	DR->addButton(ui.DR11, 11);
	DR->addButton(ui.DR12, 12);
	DR->addButton(ui.DR13, 13);
	DR->addButton(ui.DR14, 14);
	DR->addButton(ui.DR15, 15);
	DR->addButton(ui.DR16, 16);
	DR->addButton(ui.DR17, 17);
	DR->addButton(ui.DR18, 18);
	DR->addButton(ui.DR19, 19);
	DR->addButton(ui.DR20, 20);
	DR->addButton(ui.DR21, 21);
	DR->addButton(ui.DR22, 22);
	DR->addButton(ui.DR23, 23);
	DR->addButton(ui.DR24, 24);
	DR->addButton(ui.DR25, 25);
	DR->addButton(ui.DR26, 26);
	DR->addButton(ui.DR27, 27);
	DR->addButton(ui.DR28, 28);
	DR->addButton(ui.DR29, 29);
	DR->addButton(ui.DR30, 30);
	DR->addButton(ui.DR31, 31);
	DR->addButton(ui.DR32, 32);
	DR->addButton(ui.DR33, 33);
	DR->setExclusive(false);
	connect(DR, SIGNAL(buttonClicked(int)), this, SLOT(buttonDR(int)));
	TR = new QButtonGroup(this);
	TR->addButton(ui.TR1, 1);
	TR->addButton(ui.TR2, 2);
	TR->addButton(ui.TR3, 3);
	TR->addButton(ui.TR4, 4);
	TR->addButton(ui.TR5, 5);
	TR->addButton(ui.TR6, 6);
	TR->addButton(ui.TR7, 7);
	TR->addButton(ui.TR8, 8);
	TR->addButton(ui.TR9, 9);
	TR->addButton(ui.TR10, 10);
	TR->addButton(ui.TR11, 11);
	TR->addButton(ui.TR12, 12);
	TR->addButton(ui.TR13, 13);
	TR->addButton(ui.TR14, 14);
	TR->addButton(ui.TR15, 15);
	TR->addButton(ui.TR16, 16);
	TR->addButton(ui.TR17, 17);
	TR->addButton(ui.TR18, 18);
	TR->addButton(ui.TR19, 19);
	TR->addButton(ui.TR20, 20);
	TR->addButton(ui.TR21, 21);
	TR->addButton(ui.TR22, 22);
	TR->addButton(ui.TR23, 23);
	TR->addButton(ui.TR24, 24);
	TR->addButton(ui.TR25, 25);
	TR->addButton(ui.TR26, 26);
	TR->addButton(ui.TR27, 27);
	TR->addButton(ui.TR28, 28);
	TR->addButton(ui.TR29, 29);
	TR->addButton(ui.TR30, 30);
	TR->addButton(ui.TR31, 31);
	TR->addButton(ui.TR32, 32);
	TR->addButton(ui.TR33, 33);
	TR->setExclusive(false);
	connect(TR, SIGNAL(buttonClicked(int)), this, SLOT(buttonTR(int)));
	DTB = new QButtonGroup(this);
	DTB->addButton(ui.DTB1, 1);
	DTB->addButton(ui.DTB2, 2);
	DTB->addButton(ui.DTB3, 3);
	DTB->addButton(ui.DTB4, 4);
	DTB->addButton(ui.DTB5, 5);
	DTB->addButton(ui.DTB6, 6);
	DTB->addButton(ui.DTB7, 7);
	DTB->addButton(ui.DTB8, 8);
	DTB->addButton(ui.DTB9, 9);
	DTB->addButton(ui.DTB10, 10);
	DTB->addButton(ui.DTB11, 11);
	DTB->addButton(ui.DTB12, 12);
	DTB->addButton(ui.DTB13, 13);
	DTB->addButton(ui.DTB14, 14);
	DTB->addButton(ui.DTB15, 15);
	DTB->addButton(ui.DTB16, 16);
	DTB->setExclusive(false);
	connect(DTB, SIGNAL(buttonClicked(int)), this, SLOT(buttonDTB(int)));
	week = new QButtonGroup(this);
	week->addButton(ui.All, 0);
	week->addButton(ui.Two, 2);
	week->addButton(ui.Four, 4);
	week->addButton(ui.Day, 7);
	ui.All->setChecked(true);
	connect(week, SIGNAL(buttonClicked(int)), this, SLOT(buttonWeek(int)));
}

void DoubleChromosphere::InsertLorrety(Lorrety l)
{
	QStringList strs = l.getValues();
	int curRow = ui.history->rowCount();
	ui.history->insertRow(curRow);
	for (quint64 j = 0; j < strs.count(); ++j)
	{
		if (j == 2)
		{
			QFrame* frame = new QFrame;
			frame->setFixedSize(180, 30);
			frame->setStyleSheet("QLabel {font-size: 13px;min-width: 20px;max-width: 20px;max-height: 20px;min-height: 20px;border-width: 0;border-radius: 10px;background: red;color: white;}");
			for (ushort k = 0; k < 6; ++k)
			{
				QLabel* label = new QLabel(frame);
				label->setText(QString::asprintf("%02d", l.nums[k]));
				label->setAlignment(Qt::AlignCenter);
				label->move(5 + 25 * k, 5);
			}
			QLabel* label = new QLabel(frame);
			label->setStyleSheet("QLabel {font-size: 13px;min-width: 20px;max-width: 20px;max-height: 20px;min-height: 20px;border-width: 0;border-radius: 10px;background: blue;color: white;}");
			label->setText(QString::asprintf("%02d", l.nums[6]));
			label->setAlignment(Qt::AlignCenter);
			label->move(155, 5);
			ui.history->setCellWidget(curRow, j, frame);
		}
		else
		{
			QTableWidgetItem* item = new QTableWidgetItem(strs[j]);
			item->setTextAlignment(Qt::AlignCenter);
			if (j == 1)item->setData(Qt::UserRole, l.week);
			ui.history->setItem(curRow, j, item);
		}
	}
}

void DoubleChromosphere::InsertHistory(History h)
{
	ui.Purchase->insertRow(0);
	QStringList data = h.getValues();
	for (ushort i = 0; i < data.size(); ++i)
	{
		QTableWidgetItem* item = new QTableWidgetItem(data[i]);
		item->setTextAlignment(Qt::AlignCenter);
		if (i == 0)
		{
			item->setCheckState(Qt::Unchecked);
			item->setData(Qt::UserRole, h.No);
		}
		ui.Purchase->setItem(0, i, item);
	}
}

void DoubleChromosphere::InsertReds(Lorrety l)
{
	ui.Reds->insertRow(0);
	QTableWidgetItem* item = new QTableWidgetItem(QString::number(l.code));
	item->setTextAlignment(Qt::AlignCenter);
	ui.Reds->setItem(0, 0, item);
	for (ushort k = 0; k < 6; ++k)
	{
		QLabel* label = new QLabel();
		label->setText(QString::asprintf("%02d", l.nums[k]));
		label->setStyleSheet("QLabel {font-size: 13px;min-width: 20px;max-width: 20px;max-height: 20px;min-height: 20px;border-width: 0;border-radius: 10px;background: red;color: white;}");
		label->setAlignment(Qt::AlignCenter);
		ui.Reds->setCellWidget(0, l.nums[k], label);
	}
}

void DoubleChromosphere::closeEvent(QCloseEvent* event)
{}

void DoubleChromosphere::Charts(QChartView* view)
{
	QChart* chart = new QChart;
	valarray<qreal>datas(10);
	chart->setAnimationOptions(QChart::SeriesAnimations);
	view->setChart(chart);
	view->setRenderHint(QPainter::Antialiasing);
	QLineSeries* series = new QLineSeries;
	chart->legend()->setVisible(false);
	chart->addSeries(series);
	QBarCategoryAxis* axisX = new QBarCategoryAxis;
	QValueAxis* axisY = new QValueAxis;
	ushort r = 0;
	QStringList categories;
	for (auto it = lorretys.begin(); it != lorretys.end() && r < 10; ++r, ++it)
	{
		categories << QString::number(it.key());
		qreal data;
		if (view->objectName() == "Bonus")  data = it.value().prizegrades[0].second / 10000.;
		else if (view->objectName() == "Notes")data = it.value().prizegrades[0].first;
		else if (view->objectName() == "Pool")data = it.value().poolmoney / 100000000.;
		datas[r] = data;
		series->append(r, data);
	}
	axisX->append(categories);
	axisX->setTitleText("期数");
	if (view->objectName() == "Bonus")
	{
		axisY->setRange(datas.min() - 100, datas.max() + 100);
		axisY->setTitleText("万元");
	}
	else if (view->objectName() == "Notes")
	{
		axisY->setRange(0, datas.max() + 10.);
		axisY->setTitleText("注数");
	}
	else if (view->objectName() == "Pool")
	{
		axisY->setRange(datas.min() - 1, datas.max() + 1);
		axisY->setTitleText("亿元");
	}
	chart->setAxisX(axisX, series);
	chart->setAxisY(axisY, series);
}

void DoubleChromosphere::InsertBlues(Lorrety l)
{
	ui.Blues->insertRow(0);
	QTableWidgetItem* item = new QTableWidgetItem(QString::number(l.code));
	item->setTextAlignment(Qt::AlignCenter);
	ui.Blues->setItem(0, 0, item);
	QLabel* label = new QLabel();
	label->setStyleSheet("QLabel {font-size: 13px;min-width: 20px;max-width: 20px;max-height: 20px;min-height: 20px;border-width: 0;border-radius: 10px;background: blue;color: white;}");
	label->setText(QString::asprintf("%02d", l.nums[6]));
	label->setAlignment(Qt::AlignCenter);
	ui.Blues->setCellWidget(0, l.nums[6], label);
}

void DoubleChromosphere::showLorretyPrice(function<quint64(ushort, ushort)> f)
{
	QVector<quint64>nums = { f(6,1),f(6,0),f(5,1),f(5,0) + f(4,1),f(4,0) + f(3,1),f(2,1) + f(1,1) + f(0,1) };
	ulong sum = 0;
	for (ushort i = 0; i < 6; ++i)
	{
		QTableWidgetItem* item = new QTableWidgetItem(QString::number(nums[i]));
		item->setTextAlignment(Qt::AlignCenter);
		ui.lorretyPrice->setItem(i, 2, item);
		sum += nums[i] * lorretys[ui.CID->currentText().toULongLong()].prizegrades[i].second;
		item = new QTableWidgetItem(QString::number(nums[i] * lorretys[ui.CID->currentText().toULongLong()].prizegrades[i].second) + "元");
		item->setTextAlignment(Qt::AlignCenter);
		ui.lorretyPrice->setItem(i, 4, item);
	}
	ui.Charge->setTitle(QString("中奖金额: %1元").arg(sum));
}

void DoubleChromosphere::onDisconnected()
{
	QMessageBox::information(this, "连接服务器", "已断开连接");
}

void DoubleChromosphere::onSocketReadyRead()
{
	for (static quint64 i = 0; tcpClient->canReadLine(); ++i)
	{
		QString str = tcpClient->readLine();
		if (str[0] == 'd')
		{
			Lorrety lorrety(QString(str.right(str.length() - 2)));
			lorretys[lorrety.code] = lorrety;
			if (i < 100)
			{
				InsertReds(lorrety);
				InsertBlues(lorrety);
				if (i == 9)
				{
					Charts(ui.Bonus);
					Charts(ui.Notes);
					Charts(ui.Pool);
				}
				if (i < 30)
				{
					InsertLorrety(lorrety);
					ui.CID->addItem(QString::number(lorrety.code));
					ui.LID->addItem(QString::number(lorrety.code));
				}
			}
		}
		else if (str[0] == 'l')
		{
			History history(QString(str.right(str.length() - 2)));
			puchases[history.No] = history;
			InsertHistory(history);
		}
	}
	ui.Reds->resizeColumnsToContents();
	ui.Blues->resizeColumnsToContents();
	ui.history->resizeColumnsToContents();
	ui.Purchase->resizeColumnsToContents();
}

void DoubleChromosphere::buttonPR(int br)
{
	QCheckBox* checkBox = qobject_cast<QCheckBox*>(PR->button(br));
	checkBox->isChecked() ? ++pr : --pr;
	pr <= 20 && pr >= 6 && pb > 0 ? ui.PPur->setEnabled(true) : ui.PPur->setEnabled(false);
	on_PP_valueChanged(ui.PP->value());
}

void DoubleChromosphere::buttonPB(int br)
{
	QCheckBox* checkBox = qobject_cast<QCheckBox*>(PB->button(br));
	checkBox->isChecked() ? ++pb : --pb;
	pr <= 20 && pr >= 6 && pb > 0 ? ui.PPur->setEnabled(true) : ui.PPur->setEnabled(false);
	on_PP_valueChanged(ui.PP->value());
}

void DoubleChromosphere::buttonDR(int br)
{
	QCheckBox* checkBox = qobject_cast<QCheckBox*>(DR->button(br));
	checkBox->isChecked() ? ++dr : --dr;
	checkBox = qobject_cast<QCheckBox*>(TR->button(br));
	if (checkBox->isChecked())
	{
		checkBox->setChecked(false);
		--Tr;
	}
	dr >= 1 && dr <= 5 && Tr > 0 && dtb > 0 ? ui.DTPur->setEnabled(true) : ui.DTPur->setEnabled(false);
	on_DTP_valueChanged(ui.DTP->value());
}

void DoubleChromosphere::buttonTR(int br)
{
	QCheckBox* checkBox = qobject_cast<QCheckBox*>(TR->button(br));
	checkBox->isChecked() ? ++Tr : --Tr;
	checkBox = qobject_cast<QCheckBox*>(DR->button(br));
	if (checkBox->isChecked())
	{
		checkBox->setChecked(false);
		--dr;
	}
	dr >= 1 && dr <= 5 && Tr > 0 && dtb > 0 ? ui.DTPur->setEnabled(true) : ui.DTPur->setEnabled(false);
	on_DTP_valueChanged(ui.DTP->value());
}

void DoubleChromosphere::buttonDTB(int br)
{
	QCheckBox* checkBox = qobject_cast<QCheckBox*>(DTB->button(br));
	checkBox->isChecked() ? ++dtb : --dtb;
	on_DTP_valueChanged(ui.DTP->value());
	dr >= 1 && dr <= 5 && Tr > 0 && dtb > 0 ? ui.DTPur->setEnabled(true) : ui.DTPur->setEnabled(false);
}

void DoubleChromosphere::buttonWeek(int bw)
{
	QCheckBox* checkBox = qobject_cast<QCheckBox*>(week->button(bw));
	if (bw)
		for (quint64 row = 0; row < ui.history->rowCount(); ++row)
		{
			if (ui.history->item(row, 1)->data(Qt::UserRole) != checkBox->text())ui.history->hideRow(row);
			else ui.history->showRow(row);
		}
	else for (quint64 row = 0; row < ui.history->rowCount(); ++row)ui.history->showRow(row);
}
