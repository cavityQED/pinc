#include "laser.h"

Laser::Laser(QWidget* parent) : QGroupBox(parent)
{
	QPushButton* on_button = new QPushButton("ON");
	QPushButton* off_button = new QPushButton("OFF");
	QPushButton* up_button = new QPushButton(QChar(0x25B2));
	QPushButton* down_button = new QPushButton(QChar(0x25BC));

	connect(on_button, &QPushButton::released, this, &Laser::on);
	connect(off_button, &QPushButton::released, this, &Laser::off);
	connect(up_button, &QPushButton::released, this, &Laser::inc);
	connect(down_button, &QPushButton::released, this, &Laser::dec);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(on_button);
	layout->addWidget(off_button);
	layout->addWidget(up_button);
	layout->addWidget(down_button);

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

	gpioSetMode(LASER_PWM, PI_OUTPUT);
	gpioSetMode(LASER_ENA, PI_OUTPUT);
	gpioSetPWMfrequency(LASER_PWM, 5000);
	off();
}

void Laser::on()
{
	gpioPWM(LASER_PWM, m_pwr*255/100);
}

void Laser::off()
{
	gpioPWM(LASER_PWM, 0);
}

void Laser::set(int pwr)
{
	m_pwr = pwr;
	gpioPWM(LASER_PWM, m_pwr*255/100);
}

void Laser::ena(bool override)
{
	gpioWrite(LASER_ENA, override);
}

void Laser::inc()
{
	m_pwr += 5;
	if(m_pwr > 100)
		m_pwr = 100;
	on();
}

void Laser::dec()
{
	m_pwr -= 5;
	if(m_pwr < 0)
		m_pwr = 0;
	on();
}

void Laser::runBlock(const gBlock& blk)
{
	switch((int)blk.num())
	{
		case 0:
			off();
			break;

		case 1:
			on();
			break;

		case 2:
			if(blk.args().count('P') > 0)
				set(blk.args().at('P'));
			break;

		case 3:
			ena(true);
			break;

		case 4:
			ena(false);
			break;

		default:
			break;
	}
}
