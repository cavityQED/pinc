#ifndef ESP_STEPPER_GROUP_H
#define ESP_STEPPER_GROUP_H

#include "espStepperMotor.h"
#include "gcode/gBlock.h"

#define SYNC_PIN 18

class espStepperGroup : public QGroupBox
{
	Q_OBJECT

public:

	espStepperGroup(QWidget* parent = nullptr);
	~espStepperGroup() {}

	bool status(uint8_t mask, bool get = true);
	void waitUntil(uint8_t mask, uint32_t delay = 200);

	bool addStepper(espStepperMotor* stepper);

	position_t<double> position();

	void updatePosition();
	void stop();
	void pause();
	void resume();

	void jog(AXIS axis, bool dir);
	void move(const move_msg_t& msg);
	void runBlock(const gBlock& blk);

	virtual void timerEvent(QTimerEvent* event) override;

public:

	void set_jog_mm(double mm);

signals:

	void positionChange(const position_t<double>& pos);
	void moveReady();
	void blockCompleted();

protected:

	static const int 					m_timerPeriod = 5;
	double								m_feedrate;
	std::map<AXIS, espStepperMotor*>	m_steppers;
};

#endif