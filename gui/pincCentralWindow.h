#ifndef PINC_CENTRAL_WINDOW_H
#define PINC_CENTRAL_WINDOW_H

#include <QWidget>
#include <QVBoxLayout>

#include "pincColors.h"
#include "pincStyle.h"

class pincCentralWindow : public QWidget
{
	Q_OBJECT

public:

	pincCentralWindow(QWidget* parent = nullptr);
	~pincCentralWindow() {}

	void addWidget(QWidget* w)	{vlayout->addWidget(w);}
	void addLayout(QLayout* l)	{vlayout->addLayout(l);}

protected:

	QVBoxLayout* vlayout;
};

#endif