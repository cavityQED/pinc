#ifndef PINC_CONTROL_MODE_BUTTONS
#define PINC_CONTROL_MODE_BUTTONS

#include <QFrame>
#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVector>

#include "c/stepper/stepper_common.h"
#include "pincPanel.h"
#include "pincButton.h"
#include "pincStyle.h"

class pincControlModeButtons : public pincPanel
{
	Q_OBJECT

public:

	pincControlModeButtons(QWidget* parent = nullptr);
	~pincControlModeButtons() {}

	CONTROL_MODE mode() {return m_mode;}

signals:

	void controlModeChange(CONTROL_MODE mode);

protected:

	CONTROL_MODE	m_mode;

	pincButton*		m_button_jog;
	pincButton*		m_button_auto;
	pincButton*		m_button_mdi;
	pincButton*		m_button_edit;
	pincButton*		m_button_home;
	pincButton*		m_button_man;

	
};

#endif