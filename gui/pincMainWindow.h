#ifndef PINC_MAIN_WINDOW
#define PINC_MAIN_WINDOW

#include <QMainWindow>

#include "c/stepper/stepper_common.h"
#include "pincStepperControl.h"

class pincMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	pincMainWindow(QWidget* parent = nullptr);
	~pincMainWindow() {}

	void setStepperControl(pincStepperControl* ctrl) {m_stepper_ctrl = ctrl;}

public:

	void setControlMode(CONTROL_MODE mode);
	void jog(PINC_AXIS axis, bool dir);

protected:

	CONTROL_MODE			m_ctrl_mode;
	pincStepperControl*		m_stepper_ctrl;

};

#endif