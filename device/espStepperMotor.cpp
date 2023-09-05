#include "espStepperMotor.h"

espStepperMotor::espStepperMotor(int pin, const QString& name, QWidget* parent) : spiDevice(name, parent), m_handshake_pin(pin)
{
	gpioSetMode(m_handshake_pin, PI_OUTPUT);
	gpioSetPullUpDown(m_handshake_pin, PI_PUD_DOWN);
	gpioWrite(m_handshake_pin, 0);

	m_msgJOG.func = MOVE;
	m_msgJOG.mode = JOG_MODE;
	m_msgJOG.vf_mmps = 40;
	m_msgJOG.dir = true;
}

espStepperMotor::espStepperMotor(	const config_t& config,
									const QString& name,
									QWidget* parent)
	: spiDevice(name, parent), m_handshake_pin(config.pin)
{
	gpioSetMode(m_handshake_pin, PI_OUTPUT);
	gpioSetPullUpDown(m_handshake_pin, PI_PUD_DOWN);
	gpioWrite(m_handshake_pin, 0);

	set_axis(config.axis);
	set_spr(config.spr);
	set_mmpr(config.mmpr);
	set_spmm();
	set_accel(config.accel);
	set_min_speed(config.min_speed);
	set_max_speed(config.max_speed);
	set_jog_speed(config.jog_speed);
	espPrint();
	receive();

	m_msgJOG.func	= MOVE;
	m_msgJOG.mode	= JOG_MODE;
	m_msgJOG.dir	= true;
	m_msgJOG.end.x	= 1;
}

void espStepperMotor::send(const stepper_msg_in_t& msg)
{
	msg >> m_spimsg.sendbuf();

	std::scoped_lock(__spi_host->lock());

	//std::cout << "Sending Message\n" << msg << '\n';

	__spi_host->send(this, &m_spimsg);

	m_msgOUT << m_spimsg.recvbuf();

	//std::cout << "Message Received\n" << m_msgOUT << '\n';
}

void espStepperMotor::send(const move_msg_t& msg)
{
	msg >> m_spimsg.sendbuf();

	std::scoped_lock(__spi_host->lock());

	//std::cout << "Sending\t" << msg << '\n';

	__spi_host->send(this, &m_spimsg);

	m_msgOUT << m_spimsg.recvbuf();

	//std::cout << "Message Received\n" << m_msgOUT << '\n';
}

void espStepperMotor::espPrint()
{
	m_msgIN			= {};
	m_msgIN.func	= PRINT_INFO;

	send(m_msgIN);
}

void espStepperMotor::set_axis(uint8_t axis)
{
	m_axis = axis;

	m_msgIN				= {};
	m_msgIN.func		= SET_AXIS;
	m_msgIN.set_data	= m_axis;

	send(m_msgIN);
}

void espStepperMotor::set_mode(uint8_t mode)
{
	m_msgIN				= {};
	m_msgIN.func		= SET_MODE;
	m_msgIN.set_data	= mode;

	send(m_msgIN);

	m_status = m_msgOUT.status;
}

void espStepperMotor::set_min_speed(double min_speed)
{
	m_min_speed = min_speed;

	m_msgIN				= {};
	m_msgIN.func		= SET_MIN_SPEED;
	m_msgIN.set_data	= m_min_speed * m_spr / m_mmpr;

	send(m_msgIN);
}

void espStepperMotor::set_max_speed(double max_speed)
{
	m_max_speed = max_speed;

	m_msgIN				= {};
	m_msgIN.func		= SET_MAX_SPEED;
	m_msgIN.set_data	= m_max_speed * m_spr / m_mmpr;

	send(m_msgIN);
}

void espStepperMotor::set_accel(uint32_t accel)
{
	m_accel = accel;

	m_msgIN				= {};
	m_msgIN.func		= SET_ACCELERATION;
	m_msgIN.set_data	= m_accel;

	send(m_msgIN);
}

void espStepperMotor::jog(bool dir)
{
	m_msgJOG.dir = dir;

	send(m_msgJOG);
	startTimer(m_timerPeriod);
}

void espStepperMotor::stop()
{
	m_msgIN.func = STOP;
	send(m_msgIN);
}

void espStepperMotor::pause()
{
	m_msgIN.func		= PAUSE;
	m_msgIN.set_data	= (double)true;
	send(m_msgIN);
}

void espStepperMotor::resume()
{
	m_msgIN.func		= PAUSE;
	m_msgIN.set_data	= (double)false;
	send(m_msgIN);	
}

void espStepperMotor::receive()
{
	m_msgIN			= {};
	m_msgIN.func	= RECEIVE;

	send(m_msgIN);

	m_status 		= m_msgOUT.status;
	m_step_position	= m_msgOUT.step_position;

	emit positionChange(mm_position());
}