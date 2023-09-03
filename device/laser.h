#ifndef LASER_H
#define LASER_H

#include <pigpio.h>

#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "device.h"
#include "gcode/gBlock.h"

#define LASER_PWM	24
#define LASER_ENA	23

class Laser : public QGroupBox
{
	Q_OBJECT

public:

	Laser(QWidget* parent = nullptr);
	~Laser() {}

	void on();
	void off();

	void inc();
	void dec();

	void set(int pwr);
	void ena(bool override);

	void runBlock(const gBlock& blk);

signals:

	void blockCompleted();

protected:

	int m_pwr = 0;
};

#endif