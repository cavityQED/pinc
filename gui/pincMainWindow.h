#ifndef PINC_MAIN_WINDOW
#define PINC_MAIN_WINDOW

#include <QMainWindow>

#include "c/stepper/stepper_common.h"
#include "pincStepperControl.h"
#include "pincColors.h"
#include "pincStyle.h"

class pincMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	pincMainWindow(QWidget* parent = nullptr);
	~pincMainWindow() {}

	void setStepperControl(pincStepperControl* ctrl) {m_stepper_ctrl = ctrl;}

public:

	void setControlMode(CONTROL_MODE mode);
	void tryjog(const PINC_AXIS axis, bool dir);

signals:

	void jog(const PINC_AXIS axis, bool dir);
	void home(const PINC_AXIS axis);

protected:

	CONTROL_MODE			m_ctrl_mode;
	pincStepperControl*		m_stepper_ctrl;

};

#endif