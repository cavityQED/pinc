#ifndef STEPPER_CONFIG_WINDOW_H
#define STEPPER_CONFIG_WINDOW_H

#include "motor_common.h"

class stepperConfigWindow : public QGroupBox
{
	Q_OBJECT

public:

	stepperConfigWindow(QWidget* parent = nullptr);
	~stepperConfigWindow() {}

signals:

	void update(uint16_t cmd, uint32_t value);

protected:

	

}


#endif