#ifndef PINC_POSITION_H
#define PINC_POSITION_H

#include <QWidget>
#include <QFrame>
#include <QLineEdit>
#include <QLabel>
#include <QLCDNumber>
#include <QHBoxLayout>
#include <QDebug>

#include "c/stepper/stepper_common.h"
#include "pincPanel.h"
#include "pincColors.h"
#include "pincLineEdit.h"
#include "pincLabel.h"
#include "pincStyle.h"

class pincPosition : public QWidget
{
	Q_OBJECT

public:

	pincPosition(PINC_AXIS axis, QWidget* parent = nullptr);
	~pincPosition() {}

	const QLineEdit* line() {return m_line;}

public:

	void set(double pos);

protected:

	QLineEdit*	m_line;
	
};

#endif