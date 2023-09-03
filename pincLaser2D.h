#ifndef PINC_LASER_2D_H
#define PINC_LASER_2D_H

#include "device/espStepperGroup.h"
#include "gcode/gProgram.h"
#include "panel/controlPanel.h"
#include "panel/laserPanel.h"
#include "panel/positionReadout.h"
#include "panel/loadMenu.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QDebug>

#define SYNC_PIN	18

class pincLaser2D : public QGroupBox
{
	Q_OBJECT

public:

	pincLaser2D(	const espStepperMotor::config_t& xconfig,
					const espStepperMotor::config_t& yconfig,
					QWidget* parent = nullptr);
	~pincLaser2D() {}


public:

	void modeChange(controlPanel::panelMode_t mode);
	void jog(AXIS axis, bool dir);
	void setJogDistance(double mm);

	void run();
	void hold();
	void reset();
	void update();
	void run_next();

protected:
	
	bool						m_reset				= true;
	bool						m_paused			= false;
	bool						m_in_program		= false;
	size_t 						m_cur_program_id	= 0;
	controlPanel::panelMode_t	m_panelMode;
	controlPanel*				m_control_panel;
	positionReadout*			m_position_readout;
	espStepperMotor*			m_xaxis;
	espStepperMotor*			m_yaxis;
	espStepperGroup*			m_group;
	laserPanel*					m_laser_panel;
	gProgram*					m_file_program;
	gProgram*					m_cur_program;
	loadMenu*					m_load_menu;

	QMenu*						m_program_menu;

};


#endif