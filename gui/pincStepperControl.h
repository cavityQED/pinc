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

#define SYNC_PIN	23

class pincStepperControl : public QGroupBox
{
	Q_OBJECT

public:

	pincStepperControl(QWidget* parent = nullptr);
	~pincStepperControl() {}

	void addStepper(pincStepperConfig_t* config);

public:

	void jog(PINC_AXIS axis, bool dir);
	void jog_last_axis(bool dir) {jog(m_last_axis, dir);}
	void sync_move(pincStepperMove_t* move, bool convert = false);

protected:

	std::map<PINC_AXIS, std::shared_ptr<pincPiStepper>>	m_steppers;

	uint8_t	spi_mode;
	int		fd_CS0;
	int		fd_CS1;
	
	PINC_AXIS	m_last_axis;

	sem_t	pin_req_sem;
};

#endif