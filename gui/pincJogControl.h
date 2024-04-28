#ifndef PINC_JOG_CONTROL
#define PINC_JOG_CONTROL

#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QGridLayout>

#include "c/stepper/stepper_common.h"

class pincJogControl : public QGroupBox
{
	Q_OBJECT

public:

	pincJogControl(QWidget* parent = nullptr);
	~pincJogControl() {}

signals:

	void jog(PINC_AXIS axis, bool dir);
	void setJogSteps(uint32_t steps);

protected:

	QPushButton* m_button_max_jog;
	QPushButton* m_button_med_jog;
	QPushButton* m_button_min_jog;

	QPushButton* m_button_x_pos;
	QPushButton* m_button_x_neg;
	QPushButton* m_button_y_pos;
	QPushButton* m_button_y_neg;
	QPushButton* m_button_z_pos;
	QPushButton* m_button_z_neg;

};


#endif