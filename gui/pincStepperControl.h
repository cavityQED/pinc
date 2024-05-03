#ifndef PINC_STEPPER_CONTROL_H
#define PINC_STEPPER_CONTROL_H

#include <memory>
#include <cstring>

#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>

extern "C" {
	#include "c/stepper/pi_stepper.h" 
}

class pincStepperControl : public QGroupBox
{
	Q_OBJECT

public:

	pincStepperControl(QWidget* parent = nullptr);
	~pincStepperControl() {}

	void addStepper(pincStepperConfig_t* config);

public:

	void jog(PINC_AXIS axis, bool dir);

protected:

	std::map<PINC_AXIS, std::shared_ptr<pincPiStepper>>	m_steppers;

	uint8_t	spi_mode;
	int		fd_CS0;
	int		fd_CS1;
	
};

#endif