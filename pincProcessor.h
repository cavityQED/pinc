#ifndef PINC_PROCESSOR_H
#define PINC_PROCESSOR_H

#include <QWidget>

#include "c/stepper/stepper_common.h"
#include "gcode/gProgram.h"


class pincProcessor : public QWidget
{
	Q_OBJECT

public:

	pincProcessor(QWidget* parent = nullptr);
	~pincProcessor() {}

public:

	CONTROL_MODE	mode()	const	{return m_ctrlMode;}
	PINC_AXIS		axis()	const	{return m_curAxis;}

public:

	void run();
	void hold();
	void reset();
	void next();

	void setProgram(gProgram* program);
	void setMode(CONTROL_MODE mode);

	void jogHandle(PINC_AXIS axis, bool dir);

signals:

	void blockReady(const gBlock& block);
	void pause(bool p);
	void endProgram();

	void jog(PINC_AXIS axis, bool dir);
	void home(PINC_AXIS axis);

	void jogEnable(bool en);
	void modeChangeEnable(bool en);
	void editEnable(bool en);

protected:

	CONTROL_MODE		m_ctrlMode;
	PINC_AXIS			m_curAxis;
	bool				m_programRunning;
	bool				m_programPaused;
	bool				m_programReset;
	int					m_programBlockID;
	gProgram*			m_program;

};



#endif