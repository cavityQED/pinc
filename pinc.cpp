#include "gui/pinc_gui.h"

#include <QApplication>
#include <QAction>

#include <signal.h>

#include <pigpio.h>

#define WHEEL_STATUS_INTERRUPT	27
#define WHEEL_STATUS_ADDR		0x90

static pthread_mutex_t		spi_mutex;
static pthread_mutex_t		pin_req_mutex;
static pthread_mutexattr_t	spi_mutex_attr;

static signal_t				wheel_signal;
static pincSPIclient_t		fpga_client;
static pincStepperControl*	steppers;
static pincStepperMove*		move_window;
static pincStepperMove_t	sync_move;
static pincStepperMove_t	curve_move;

static void shutdown(int signum)
{
	gpioTerminate();
	exit(signum);
}

static void wheel_pin_isr(int gpio, int level, uint32_t tick, void* dev)
{
	static uint8_t tx[2] = {WHEEL_STATUS_ADDR, 0};
	static uint8_t rx[2] = {0, 0};

	if(!level)
	{
		spi_write_msg(&fpga_client, tx, 2);

		spi_send(&fpga_client);

		spi_read_msg(&fpga_client, rx, 2);

		signal_update(&wheel_signal, rx[1]);
		wheel_signal.mask <<= 4; 
		wheel_signal.mask |= (wheel_signal.high & 0x03) << 2;
		wheel_signal.mask |= (wheel_signal.low & 0x03);

		if(wheel_signal.cur == 0xFF)
		{
			switch(wheel_signal.mask)
			{
				case 0x2184:
					steppers->jog_last_axis(true);
					break;

				case 0x1248:
					steppers->jog_last_axis(false);
					break;

				default:
					break;
			}

			wheel_signal.mask = 0x0000;
		}
	}
}

int main(int argc, char *argv[])
{	
	QApplication app(argc, argv);

	gpioInitialise();
	gpioSetSignalFunc(SIGINT, shutdown);

	pincMainWindow*			mainWindow	= new pincMainWindow();
	pincControlModeButtons*	ctrl_panel	= new pincControlModeButtons();
	pincJogControl*			jog_panel	= new pincJogControl();
	QWidget*				central		= new QWidget();
	QVBoxLayout*			vlayout		= new QVBoxLayout();
	QHBoxLayout*			hlayout		= new QHBoxLayout();
	
	steppers	= new pincStepperControl();
	move_window	= new pincStepperMove();

	pthread_mutexattr_init(&spi_mutex_attr);
	pthread_mutexattr_settype(&spi_mutex_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_mutex_attr);

	pincStepperConfig_t	config;
	stepper_get_default_config(&config);
	config.spi_mutex		= &spi_mutex;
	config.accel			= 400000;

	config.axis				= X_AXIS;
	config.pin_status		= X_STATUS_INTERRUPT;
	config.pin_spi_hs		= X_SPI_HANDSHAKE;
	config.fpga_addr		= X_FPGA_STATUS_ADDR;
	steppers->addStepper(&config);
	
	config.axis				= Y_AXIS;
	config.pin_status		= Y_STATUS_INTERRUPT;
	config.pin_spi_hs		= Y_SPI_HANDSHAKE;
	config.fpga_addr		= Y_FPGA_STATUS_ADDR;
	steppers->addStepper(&config);

	std::memset(&fpga_client.tr, 0, sizeof(spiTr));
	int spi_mode = SPI_MODE_0;
	fpga_client.cs					= -1;
	fpga_client.fd					= open("/dev/spidev0.0", O_RDWR);
	fpga_client.mutex				= &spi_mutex;
	fpga_client.tr.speed_hz			= config.spi_fpga_speed;
	fpga_client.tr.delay_usecs		= config.spi_delay;
	fpga_client.tr.bits_per_word	= config.spi_bpw;
	fpga_client.tr.cs_change		= config.spi_cs_change;
	ioctl(fpga_client.fd, SPI_IOC_WR_MODE, &spi_mode);
	gpioSetMode(WHEEL_STATUS_INTERRUPT, PI_INPUT);
	gpioSetPullUpDown(WHEEL_STATUS_INTERRUPT, PI_PUD_UP);
	gpioSetISRFuncEx(WHEEL_STATUS_INTERRUPT, FALLING_EDGE, 0, wheel_pin_isr, NULL);
	wheel_signal.cur = 0xFF;

	vlayout->addWidget(ctrl_panel);
	vlayout->addWidget(jog_panel);
	hlayout->addLayout(vlayout);
	hlayout->addWidget(move_window);

	central->setLayout(hlayout);
	mainWindow->setCentralWidget(central);
	mainWindow->setStepperControl(steppers);

	QObject::connect(	ctrl_panel,
						&pincControlModeButtons::controlModeChange,
						mainWindow,
						&pincMainWindow::setControlMode);

	QObject::connect(	jog_panel,
						&pincJogControl::jog,
						mainWindow,
						&pincMainWindow::jog);

	QObject::connect(	move_window,
						&pincStepperMove::move,
						steppers,
						&pincStepperControl::sync_move);

	mainWindow->setStyleSheet(	
		"QMainWindow {"
			"background-color:	#525151;"
			"}"

		"QPushButton{"	
			"background-color:	#22AAFF;"
			"border-style:		outset;"
			"border-width:		2px;"
			"border-color:		#CAEEFF;"
			"border-radius: 	4px;"
			"font: 				bold 12pt;"
			"outline:			0;"
			"min-width:			50px;"
			"max-width:			50px;"
			"min-height:		50px;"
			"max-height:		50px;"
			"}"
			
		"QPushButton:pressed{"
			"background-color:	#FA88A8;"
			"border-style:		inset;"
			"}"
			
		"QPushButton:checked{"
			"background-color:	#FA88A8;"
			"border-color:		#CAEEFF;"
			"}"

		"QGroupBox{"
			"border-style:		inset;"
			"border: 			1px solid #22AAFF;"
			"padding-top:		10px;"
			"margin-top:		10px;"
			"outline:			0;"
			"font:				bold 12pt;"
			"}"
			
		"QGroupBox::title{"
			"subcontrol-position:	top left;"
			"subcontrol-origin:		margin;"
			"top: 					2px;"
			"left:					10px;"
			"color:					#CAEEFF;"
			"}"

		"QLabel{"
			"color:					#CAEEFF;"
			"}"
	);

	mainWindow->show();

	return app.exec();
}