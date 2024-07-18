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
#include "pincPanel.h"
#include "pincButton.h"
#include "pincStyle.h"

#define WHEEL_STATUS_INTERRUPT	27
#define WHEEL_STATUS_ADDR		0x90

class pincJogControl : public pincPanel
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

	pincButton*	m_button_max_jog;
	pincButton*	m_button_med_jog;
	pincButton*	m_button_min_jog;

	pincButton*	m_button_x_pos;
	pincButton*	m_button_x_neg;
	pincButton*	m_button_y_pos;
	pincButton*	m_button_y_neg;
	pincButton*	m_button_z_pos;
	pincButton*	m_button_z_neg;

	PINC_AXIS		m_last_axis;

	signal_t		wheel_signal;
	pincSPIclient_t	fpga_spi_client;
};


#endif