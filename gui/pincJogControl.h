#ifndef PINC_JOG_CONTROL
#define PINC_JOG_CONTROL

#include <cstring>

#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QGridLayout>

#include "c/spi/spi_client.h"
#include "c/io/pin_request.h"
#include "c/stepper/stepper_common.h"

#define WHEEL_STATUS_INTERRUPT	27
#define WHEEL_STATUS_ADDR		0x90

class pincJogControl : public QGroupBox
{
	Q_OBJECT

public:

	pincJogControl(QWidget* parent = nullptr);
	~pincJogControl() {}

	void wheel_config(pthread_mutex_t* spi_mutex, int spi_speed);
	PINC_AXIS last_axis() {return m_last_axis;}

	static void wheel_input(int gpio, int level, uint32_t tick, void* dev);

signals:

	void jog(const PINC_AXIS axis, bool dir);
	void setJogSteps(uint32_t steps);

protected:

	QPushButton*	m_button_max_jog;
	QPushButton*	m_button_med_jog;
	QPushButton*	m_button_min_jog;

	QPushButton*	m_button_x_pos;
	QPushButton*	m_button_x_neg;
	QPushButton*	m_button_y_pos;
	QPushButton*	m_button_y_neg;
	QPushButton*	m_button_z_pos;
	QPushButton*	m_button_z_neg;

	PINC_AXIS		m_last_axis;

	signal_t		wheel_signal;
	pincSPIclient_t	fpga_spi_client;
};


#endif