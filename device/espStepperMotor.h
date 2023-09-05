#ifndef ESP_STEPPER_MOTOR_H
#define ESP_STEPPER_MOTOR_H

#include "pigpio.h"

#include "spiDevice.h"
#include "common/motor_common.h"

class espStepperMotor : public spiDevice
{
	Q_OBJECT

public:

	struct config_t
	{
		int			pin;
		uint8_t		axis;
		uint32_t	spr;
		uint32_t	mmpr;
		uint32_t	jog_speed;
		uint32_t	min_speed;
		uint32_t	max_speed;
		uint32_t	accel;
	};

public:

	espStepperMotor(int pin, const QString& name = "", QWidget* parent = nullptr);
	espStepperMotor(const config_t& config, const QString& name = "", QWidget* parent = nullptr);
	~espStepperMotor() {}

	virtual void trigger()	override	{gpioWrite(m_handshake_pin, 1);}
	virtual void reload()	override	{gpioWrite(m_handshake_pin, 0);}

	void send(const stepper_msg_in_t& msg);
	void send(const move_msg_t& msg);

	void espPrint();

public:

	void set_axis(uint8_t axis);
	void set_mode(uint8_t mode);

	void set_min_speed(double min_speed);
	void set_max_speed(double max_speed);
	void set_accel(uint32_t accel);

	void set_spr(uint32_t spr)				{m_spr = spr;}
	void set_mmpr(uint32_t mmpr)			{m_mmpr = mmpr;}
	void set_spmm()							{m_msgIN.func = SET_SPMM; m_msgIN.set_data = spmm(); send(m_msgIN);}
	void set_jog_mm(double mm)				{m_msgJOG.end.x = mm;}
	void set_jog_speed(double jog_speed)	{m_msgJOG.vf_mmps = jog_speed;}

	void jog(bool dir);
	void stop();
	void pause();
	void resume();
	void receive();

public:

	int			step_position()	const	{return m_step_position;}
	int			step_offset()	const	{return m_step_offset;}
	int			handshake_pin()	const	{return m_handshake_pin;}

	uint8_t 	status()		const	{return m_status;}
	uint8_t		axis()			const	{return m_axis;}
	
	uint32_t	spr()			const	{return m_spr;}
	uint32_t	mmpr()			const	{return m_mmpr;}
	uint32_t	spmm()			const	{return m_spr / m_mmpr;}

	double		min_speed()		const	{return m_min_speed;}
	double		max_speed()		const	{return m_max_speed;}
	uint32_t	accel()			const	{return m_accel;}

	double		jog_speed()		const	{return m_msgJOG.vf_mmps;}
	double		mm_position()	const	{return std::round(1000*(double)m_step_position / (double)spmm())/1000;}

signals:

	void positionChange(double mm);

protected:

	static const int m_timerPeriod = 10;

	int			m_step_position;
	int			m_step_offset;
	int			m_handshake_pin;

	uint8_t		m_status;	
	uint8_t		m_axis;

	uint32_t	m_spr	= 1600;		//steps per rotation
	uint32_t	m_mmpr	= 4;		//mm per rotation

	uint32_t	m_min_speed;
	uint32_t	m_max_speed;
	uint32_t	m_accel;

	move_msg_t			m_msgJOG = {};
	stepper_msg_in_t	m_msgIN;
	stepper_msg_out_t	m_msgOUT;
	spiMsg				m_spimsg;
};

#endif