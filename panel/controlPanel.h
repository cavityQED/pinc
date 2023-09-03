#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QTextEdit>
#include <QGridLayout>
#include <QSpacerItem>

#include "common/motor_common.h"

class controlPanel : public QWidget
{
	Q_OBJECT

public:

	enum panelMode_t
	{
		AUTO	= 0x01,
		JOG		= 0x02,
		HOME	= 0x04,
		MDI		= 0x08,
		EDIT	= 0x10
	};

public:

	controlPanel(QWidget* parent = nullptr);
	~controlPanel() {}

	void setupModeGroup();
	void setupArrowGroup();
	void setupJogGroup();
	void setupControlGroup();
	void setupTextBox();

public:

	QTextEdit* textBox() {return m_textBox;}
	QString text() const {return m_textBox->toPlainText();}

signals:

	void modeChange(panelMode_t mode);
	void axisButton(AXIS axis, bool dir);
	void jogSpeed(int speed);
	void jogDistance(double mm);

	void run();
	void hold();
	void reset();

protected:

	QGroupBox*		m_mode_group;
	QGroupBox*		m_arrow_group;
	QGroupBox*		m_jog_group;
	QGroupBox*		m_control_group;

	QPushButton* 	m_jog_button;
	QPushButton* 	m_home_button;
	QPushButton* 	m_auto_button;
	QPushButton* 	m_MDI_button;
	QPushButton* 	m_edit_button;
	QPushButton* 	m_two_button;

	QPushButton*	m_up_button;
	QPushButton*	m_down_button;
	QPushButton*	m_left_button;
	QPushButton*	m_right_button;

	QPushButton*	m_jog100_button;
	QPushButton*	m_jog10_button;
	QPushButton*	m_jog1_button;
	QPushButton*	m_jogXpos_button;
	QPushButton*	m_jogXneg_button;
	QPushButton*	m_jogYpos_button;
	QPushButton*	m_jogYneg_button;
	QPushButton*	m_jogZpos_button;
	QPushButton*	m_jogZneg_button;

	QPushButton*	m_reset_button;
	QPushButton*	m_run_button;
	QPushButton*	m_hold_button;

	QTextEdit*		m_textBox;
};

#endif