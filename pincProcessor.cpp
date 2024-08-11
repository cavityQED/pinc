#include "pincProcessor.h"

pincProcessor::pincProcessor(QWidget* parent) : QWidget(parent)
{
	m_programReset = false;

	reset();
}

void pincProcessor::run()
{
	if(!m_program)
		return;

	if(m_programRunning && m_programPaused)
	{
		m_programPaused		= false;

		emit pause(false);
	}
	else if(m_programReset)
	{
		m_programReset		= false;
		m_programRunning	= true;
		m_programPaused		= false;

		emit modeChangeEnable(false);
		emit blockReady(*m_program->blocks()[0]);
	}
}

void pincProcessor::hold()
{
	if(m_programRunning && !m_programPaused)
	{
		m_programPaused		= true;

		emit pause(true);
	}
}

void pincProcessor::reset()
{
	if(m_programReset)
		return;

	m_programRunning	= false;
	m_programPaused		= false;
	m_programReset		= true;
	m_programBlockID	= 0;

	emit modeChangeEnable(true);
}

void pincProcessor::next()
{
	if(!m_programRunning || m_programPaused)
		return;

	if(++m_programBlockID < m_program->blocks().size())
		emit blockReady(*m_program->blocks()[m_programBlockID]);
	else
	{
		emit endProgram();
		emit modeChangeEnable(true);
	}

}

void pincProcessor::setProgram(gProgram* program)
{
	m_program = program;
}

void pincProcessor::setMode(CONTROL_MODE mode)
{
	if(m_ctrlMode == mode)
		return;

	m_ctrlMode = mode;

	switch(m_ctrlMode)
	{
		case AUTO_CTRL:
		case MDI_CTRL:
			emit jogEnable(false);
			break;

		case JOG_CTRL:
		case HOME_CTRL:
			emit jogEnable(true);
			break;

		case EDIT_CTRL:
			emit editEnable(true);
			break;

		default:
			emit editEnable(false);
			break;
	}
}

void pincProcessor::jogHandle(PINC_AXIS axis, bool dir)
{
	m_curAxis = axis;

	if(m_ctrlMode == CONTROL_MODE::JOG_CTRL)
		emit jog(m_curAxis, dir);
	else if(m_ctrlMode == CONTROL_MODE::HOME_CTRL)
		emit home(m_curAxis);
}