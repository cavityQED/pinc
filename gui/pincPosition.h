#ifndef PINC_POSITION_H
#define PINC_POSITION_H

#include <QWidget>
#include <QLabel>
#include <QLCDNumber>
#include <QHBoxLayout>

#include "c/stepper/stepper_common.h"

class pincPosition : public QWidget
{
	Q_OBJECT

public:

	pincPosition(PINC_AXIS axis, QWidget* parent = nullptr);
	~pincPosition() {}

public:

	void set(double pos);

protected:

	QLCDNumber*	m_lcd;

};

#endif