#ifndef PINC_STEPPER_CONTROL_H
#define PINC_STEPPER_CONTROL_H

#include <memory>
#include <cstring>

#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QTimer>

#include "pincPosition.h"

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

	void monitor_start(int msec)	{m_timer.start(msec);}
	void monitor_stop()				{m_timer.stop();}
	void update();

public:

	void jog(PINC_AXIS axis, bool dir);
	void home(PINC_AXIS axis, bool dir);
	void sync_move(pincStepperMove_t* move, bool convert = false);

protected:

	std::map<PINC_AXIS, std::shared_ptr<pincPiStepper>>	m_steppers;
	std::map<PINC_AXIS, pincPosition*>	m_positions;

	uint8_t	spi_mode;
	int		fd_CS0;
	int		fd_CS1;
	
	sem_t	pin_req_sem;

	QTimer	m_timer;

	QVBoxLayout	m_pos_layout;
};

#endif