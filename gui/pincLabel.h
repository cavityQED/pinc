#ifndef PINC_LABEL_H
#define PINC_LABEL_H

#include <QLabel>
#include <QEvent>

#include "pincColors.h"
#include "pincStyle.h"

class pincLabel : public QLabel
{
	Q_OBJECT

public:

	pincLabel(const QString& str, QWidget* parent = nullptr);
	~pincLabel() {}

public:


protected:

	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;

};

#endif