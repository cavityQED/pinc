#ifndef PINC_CONTROL_MODE_BUTTONS
#define PINC_CONTROL_MODE_BUTTONS

#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QVector>

#include "common/motor_common.h"

class pincControlModeButtons : public QGroupBox
{
	Q_OBJECT

public:

	pincControlModeButtons(QWidget* parent = nullptr);
	~pincControlModeButtons() {}

signals:

	void controlModeChange(CONTROL_MODE mode);

protected:

	QPushButton*	m_button_jog;
	QPushButton*	m_button_auto;
	QPushButton*	m_button_mdi;
	QPushButton*	m_button_edit;
	QPushButton*	m_button_home;
	QPushButton*	m_button_man;


};

#endif