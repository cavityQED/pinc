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
	m_load_menu			= new loadMenu();

	QHBoxLayout* layout = new QHBoxLayout();
	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->addWidget(m_position_readout);
	vlayout->addWidget(m_laser_panel);
	layout->setMenuBar(m_load_menu);
	layout->addLayout(vlayout);
	layout->addWidget(m_control_panel);

	setLayout(layout);

	connect(	m_xaxis,
				&espStepperMotor::positionChange,
				m_position_readout,
				&positionReadout::setX);

	connect(	m_yaxis,
				&espStepperMotor::positionChange,
				m_position_readout,
				&positionReadout::setY);

	connect(	m_group,
				&espStepperGroup::positionChange,
				m_position_readout,
				&positionReadout::setPosition);

	connect(m_control_panel, &controlPanel::axisButton, this, &pincLaser2D::jog);
	connect(m_control_panel, &controlPanel::modeChange, this, &pincLaser2D::modeChange);
	connect(m_control_panel, &controlPanel::run, this, &pincLaser2D::run);
	connect(	m_group,
				&espStepperGroup::blockCompleted,
				this,
				&pincLaser2D::run_next);

	connect(m_load_menu, &loadMenu::gProgramLoadRequest, m_file_program, &gProgram::getBlocks);
}

void pincLaser2D::modeChange(controlPanel::panelMode_t mode)
{
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
		hold();
}

void pincLaser2D::run()
{
	switch(m_panelMode)
	{
		case controlPanel::panelMode_t::AUTO:
		{
			m_in_program = true;

			if(m_reset)
			{

			}
			else if(!m_reset && m_paused)
			{

			}

			break;
		}

		case controlPanel::panelMode_t::JOG:
		{


			break;
		}

		case controlPanel::panelMode_t::HOME:
		{

			break;
		}

		case controlPanel::panelMode_t::MDI:
		{
			m_cur_program = new gProgram(m_control_panel->text());
			m_cur_program_id = 0;
			run_next();

			break;
		}

		case controlPanel::panelMode_t::EDIT:
		{

			break;
		}

		default:
			break;
			
	} // switch(m_panelMode)
}

void pincLaser2D::hold()
{
	m_file_program->stop();
	m_group->stop();
	m_laser_panel->laser()->off();
}

void pincLaser2D::reset()
{

}

void pincLaser2D::update()
{

}