#ifndef LASER_PANEL_H
#define LASER_PANEL_H

#include "device/laser.h"

#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

class laserPanel : public QGroupBox
{
	Q_OBJECT

public:

	laserPanel(QWidget* parent = nullptr);
	~laserPanel() {}

	Laser*	laser() {return m_laser;}

protected:

	Laser*			m_laser;
	QPushButton*	m_on;
	QPushButton*	m_off;
	QPushButton*	m_ena;
	QPushButton*	m_up;
	QPushButton*	m_down;
};

#endif