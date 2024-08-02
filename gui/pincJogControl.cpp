#include "pincJogControl.h"

pincJogControl::pincJogControl(QWidget* parent) : pincPanel("Jog Control", parent)
{
	m_button_max_jog = new pincButton("100");
	m_button_med_jog = new pincButton("10"); 
	m_button_min_jog = new pincButton("1");

	QButtonGroup* steps_group = new QButtonGroup();
	steps_group->addButton(m_button_max_jog);
	steps_group->addButton(m_button_med_jog);
	steps_group->addButton(m_button_min_jog);
	steps_group->setExclusive(true);
	m_button_med_jog->setChecked(true);

	connect(m_button_max_jog, &QPushButton::toggled,
		[this](bool checked) {if(checked) setJogSteps(800);});

	connect(m_button_med_jog, &QPushButton::toggled,
		[this](bool checked) {if(checked) setJogSteps(400);});

	connect(m_button_min_jog, &QPushButton::toggled,
		[this](bool checked) {if(checked) setJogSteps(100);});

	m_button_x_pos = new pincButton("X+");
	m_button_x_neg = new pincButton("X-");
	m_button_y_pos = new pincButton("Y+");
	m_button_y_neg = new pincButton("Y-");
	m_button_z_pos = new pincButton("Z+");
	m_button_z_neg = new pincButton("Z-");

	QButtonGroup* dir_group = new QButtonGroup();
	dir_group->addButton(m_button_x_pos);
	dir_group->addButton(m_button_x_neg);
	dir_group->addButton(m_button_y_pos);
	dir_group->addButton(m_button_y_neg);
	dir_group->addButton(m_button_z_pos);
	dir_group->addButton(m_button_z_neg);
	dir_group->setExclusive(true);
	m_button_x_pos->setChecked(true);

	m_button_x_pos->setShortcut(Qt::Key_D);
	m_button_x_neg->setShortcut(Qt::Key_A);
	m_button_y_pos->setShortcut(Qt::Key_W);
	m_button_y_neg->setShortcut(Qt::Key_S);

	connect(m_button_x_pos, &QPushButton::clicked, [this] {m_last_axis = X_AXIS; jog(X_AXIS, 1);});
	connect(m_button_x_neg, &QPushButton::clicked, [this] {m_last_axis = X_AXIS; jog(X_AXIS, 0);});
	connect(m_button_y_pos, &QPushButton::clicked, [this] {m_last_axis = Y_AXIS; jog(Y_AXIS, 1);});
	connect(m_button_y_neg, &QPushButton::clicked, [this] {m_last_axis = Y_AXIS; jog(Y_AXIS, 0);});
	connect(m_button_z_pos, &QPushButton::clicked, [this] {m_last_axis = Z_AXIS; jog(Z_AXIS, 1);});
	connect(m_button_z_neg, &QPushButton::clicked, [this] {m_last_axis = Z_AXIS; jog(Z_AXIS, 0);});

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(m_button_max_jog, 0, 0);
	layout->addWidget(m_button_med_jog, 0, 1);
	layout->addWidget(m_button_min_jog, 0, 2);

	layout->addWidget(m_button_x_pos, 2, 2);
	layout->addWidget(m_button_x_neg, 2, 0);
	layout->addWidget(m_button_y_pos, 1, 1);
	layout->addWidget(m_button_y_neg, 3, 1);
	layout->addWidget(m_button_z_pos, 1, 2);
	layout->addWidget(m_button_z_neg, 3, 0);
	layout->setSizeConstraint(QLayout::SetFixedSize);

	m_last_axis = X_AXIS;

	setLayout(layout);
	setCheckable(true);
}

void pincJogControl::wheel_config(pthread_mutex_t* spi_mutex, int spi_speed)
{
	std::memset(&fpga_spi_client.tr, 0, sizeof(spiTr));
	int spi_mode = SPI_MODE_0;
	
	fpga_spi_client.cs					= -1;
	fpga_spi_client.fd					= open("/dev/spidev0.0", O_RDWR);
	fpga_spi_client.mutex				= spi_mutex;
	fpga_spi_client.tr.speed_hz			= spi_speed;

	ioctl(fpga_spi_client.fd, SPI_IOC_WR_MODE, &spi_mode);
	gpioSetMode(WHEEL_STATUS_INTERRUPT, PI_INPUT);
	gpioSetPullUpDown(WHEEL_STATUS_INTERRUPT, PI_PUD_UP);
	gpioSetISRFuncEx(WHEEL_STATUS_INTERRUPT, FALLING_EDGE, 0, wheel_input, this);
	wheel_signal.cur = 0xFF;
}

void pincJogControl::wheel_input(int gpio, int level, uint32_t tick, void* dev)
{
	pincJogControl* ctrl = (pincJogControl*)dev;

	if(!level)
	{
		uint8_t tx[2] = {WHEEL_STATUS_ADDR, 0};
		uint8_t rx[2] = {0,0};

		spi_write_msg(&ctrl->fpga_spi_client, tx, 2);
		spi_send(&ctrl->fpga_spi_client);
		spi_read_msg(&ctrl->fpga_spi_client, rx, 2);

		signal_update(&ctrl->wheel_signal, rx[1]);

		ctrl->wheel_signal.mask <<= 4;
		ctrl->wheel_signal.mask |= (ctrl->wheel_signal.high & 0x03) << 2;
		ctrl->wheel_signal.mask |= (ctrl->wheel_signal.low & 0x03);

		if(ctrl->wheel_signal.cur == 0xFF)
		{
			switch(ctrl->wheel_signal.mask)
			{
				case 0x2184:
					ctrl->jog(ctrl->last_axis(), 1);
					break;

				case 0x1248:
					ctrl->jog(ctrl->last_axis(), 0);
					break;

				default:
					break;
			}

			ctrl->wheel_signal.mask = 0x0000;
		}
	}
}