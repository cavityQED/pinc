#include "laserPanel.h"

laserPanel::laserPanel(QWidget* parent) : QGroupBox(parent)
{
	m_laser = new Laser();
	m_on = new QPushButton("ON");
	m_off = new QPushButton("OFF");
	m_up = new QPushButton(QChar(0x2191));
	m_down = new QPushButton(QChar(0x2193));
	m_ena = new QPushButton("ENA");
	m_ena->setCheckable(true);

	connect(m_on, &QPushButton::released, m_laser, &Laser::on);
	connect(m_off, &QPushButton::released, m_laser, &Laser::off);
	connect(m_up, &QPushButton::released, m_laser, &Laser::inc);
	connect(m_down, &QPushButton::released, m_laser, &Laser::dec);
	connect(m_ena, &QPushButton::toggled, m_laser, &Laser::ena);

	QVBoxLayout* on_off = new QVBoxLayout();
	on_off->addWidget(m_on);
	on_off->addWidget(m_off);

	QVBoxLayout* up_down = new QVBoxLayout();
	up_down->addWidget(m_up);
	up_down->addWidget(m_down);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addLayout(on_off);
	layout->addLayout(up_down);
	layout->addWidget(m_ena);

	setLayout(layout);

	setStyleSheet(	"QPushButton{"	
						"background-color: #75B8C8;"
						"border-style: outset;"
						"border-width: 3px;"
						"border-color: #408DA0;"
						"border-radius: 4px;"
						"font: bold 16pt;"
						"outline: 0;"
						"min-width: 40px;"
						"max-width: 40px;"
						"min-height: 40px;"
						"max-height: 40px;}"
						
					"QPushButton:pressed{"
						"background-color: #408DA0;"
						"border-style: inset;}"
						
					"QPushButton:checked{"
						"background-color: #0EFF5E;"
						"border-color: #049434;}"

					"QGroupBox{"
						"border-style: inset;"
						"border: 3px solid gray;"
						"margin-top: 10px;"
						"padding-left: 5px;"
						"padding-top: 5px;"
						"outline: 0;"
						"font: bold 8pt;}"
						
					"QGroupBox::title{"
						"subcontrol-position: top left;"
						"subcontrol-origin: margin;"
						"top: 0; left: 8px;}");
}