#ifndef PINC_MAIN_WINDOW
#define PINC_MAIN_WINDOW

#include <QMainWindow>

#include "common/motor_common.h"

class pincMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	pincMainWindow(QWidget* parent = nullptr);
	~pincMainWindow() {}

public:

	void setControlMode(CONTROL_MODE mode);
	void jog(AXIS axis, bool dir);

protected:

	CONTROL_MODE	m_ctrl_mode;
	
};

#endif