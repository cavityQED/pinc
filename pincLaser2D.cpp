#include "pincLaser2D.h"

pincLaser2D::pincLaser2D(	const espStepperMotor::config_t& xconfig,
							const espStepperMotor::config_t& yconfig,
							QWidget* parent)
	: QGroupBox(parent)
{
	m_xaxis = new espStepperMotor(xconfig);
	m_yaxis = new espStepperMotor(yconfig);
	m_group = new espStepperGroup();
	m_group->addStepper(m_xaxis);
	m_group->addStepper(m_yaxis);

	m_cur_program		= new gProgram();
	m_file_program		= new gProgram();
	m_control_panel		= new controlPanel();
	m_position_readout	= new positionReadout();
	m_laser_panel		= new laserPanel();

	m_group->updatePosition();

	QHBoxLayout* layout = new QHBoxLayout();
	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->addWidget(m_position_readout);
	vlayout->addWidget(m_laser_panel);
	layout->addLayout(vlayout);
	layout->addWidget(m_control_panel);

	setLayout(layout);

	connect(	m_group,
				&espStepperGroup::positionChange,
				m_position_readout,
				&positionReadout::setPosition);
	
	connect(	m_group,
				&espStepperGroup::blockCompleted,
				this,
				&pincLaser2D::run_next);

	connect(m_control_panel, &controlPanel::axisButton,	this, &pincLaser2D::jog);
	connect(m_control_panel, &controlPanel::modeChange,	this, &pincLaser2D::modeChange);
	connect(m_control_panel, &controlPanel::run,		this, &pincLaser2D::run);
	connect(m_control_panel, &controlPanel::hold,		this, &pincLaser2D::hold);
	connect(m_control_panel, &controlPanel::reset,		this, &pincLaser2D::reset);
}

void pincLaser2D::modeChange(controlPanel::panelMode_t mode)
{
	if(m_in_program)
		return;

	m_panelMode = mode;

	switch(m_panelMode)
	{
		case controlPanel::panelMode_t::AUTO:
		{
			m_control_panel->textBox()->setText(m_file_program->str());
			break;
		}

		case controlPanel::panelMode_t::MDI:
		{
			m_control_panel->textBox()->clear();
		}

		default:
			break;
	}
}

void pincLaser2D::jog(AXIS axis, bool dir)
{
	if(m_panelMode != controlPanel::panelMode_t::JOG)
		return;

	switch(axis)
	{
		case X_AXIS:
			m_xaxis->jog(dir);
			break;
		case Y_AXIS:
			m_yaxis->jog(dir);
			break;
		default:
			break;
	}
}

void pincLaser2D::setJogDistance(double mm)
{
	m_group->set_jog_mm(mm);
}

void pincLaser2D::run_next()
{
	if(m_cur_program_id < m_cur_program->size())
	{
		m_reset = false;

		gBlock* tmp = m_cur_program->at(m_cur_program_id++);

		switch(tmp->ltr())
		{
			case 'g': case 'G':
			{
				m_group->runBlock(*tmp);
				break;
			}

			case 'l': case 'L':
			{
				m_laser_panel->laser()->runBlock(*tmp);
				run_next();
				break;
			}

			default:
				break;
		}
	}

	else
	{
		m_in_program = false;
		hold();
	}
}

void pincLaser2D::run()
{
	m_paused = m_in_program && !m_reset && !m_group->anyStatus(IN_MOTION, true);

	switch(m_panelMode)
	{
		case controlPanel::panelMode_t::AUTO:
		{
			if(m_reset)
			{
				m_cur_program = m_file_program;
				m_in_program = true;
				run_next();
			}

			else if(m_paused)
				m_group->resume();

			break;
		}

		case controlPanel::panelMode_t::MDI:
		{
			if(m_reset)
			{
				m_cur_program = new gProgram(m_control_panel->text());
				m_in_program = true;
				run_next();
			}

			else if(m_paused)
				m_group->resume();

			break;
		}

		case controlPanel::panelMode_t::EDIT:
		case controlPanel::panelMode_t::HOME:
		case controlPanel::panelMode_t::JOG:
			break;

		default:
			break;
			
	} // switch(m_panelMode)
}

void pincLaser2D::hold()
{
	if(m_in_program)
		m_group->pause();
	m_group->updatePosition();
}

void pincLaser2D::reset()
{

	hold();
	m_laser_panel->laser()->off();
	m_group->stop();
	m_cur_program = nullptr;
	m_cur_program_id = 0;
	m_in_program = false;
	m_reset = true;
}

void pincLaser2D::update()
{

}

void pincLaser2D::loadFromFile(const QString& filename)
{
	QFile file(filename);

	if(!file.open(QFile::ReadOnly))
		return;
	else
		loadFromString(file.readAll());
}