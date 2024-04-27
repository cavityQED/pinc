#ifndef PINC_STEPPER_CONTROL_H
#define PINC_STEPPER_CONTROL_H

#include "c/stepper/pi_stepper.h"

#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>

class pincStepperControl : public QGroupBox
{
	Q_OBJECT

public:

	pincStepperControl(QWidget* parent = nullptr);
	~pincStepperControl() {}

protected:

	pincPiStepper		stepper;

}

#endif