#include "gui/pinc_gui.h"

#include <QApplication>
#include <QAction>
#include <QMetaType>

#include <signal.h>

#include <pigpio.h>

static pthread_mutex_t		spi_mutex;
static pthread_mutex_t		pin_req_mutex;
static pthread_mutexattr_t	spi_mutex_attr;

static pincStepperControl*	steppers;
static pincStepperMove*		move_window;
static pincStepperMove_t	sync_move;
static pincStepperMove_t	curve_move;

Q_DECLARE_METATYPE(PINC_AXIS);

static void shutdown(int signum)
{
	gpioTerminate();
	exit(signum);
}

int main(int argc, char *argv[])
{	
	QApplication app(argc, argv);

	gpioInitialise();
	gpioSetSignalFunc(SIGINT, shutdown);

	pthread_mutexattr_init(&spi_mutex_attr);
	pthread_mutexattr_settype(&spi_mutex_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_mutex_attr);

	pincMainWindow*			mainWindow	= new pincMainWindow();
	pincControlModeButtons*	ctrl_panel	= new pincControlModeButtons();
	pincJogControl*			jog_panel	= new pincJogControl();
	QWidget*				central		= new QWidget();
	QVBoxLayout*			vlayout		= new QVBoxLayout();
	QHBoxLayout*			hlayout		= new QHBoxLayout();
	
	steppers	= new pincStepperControl();
	move_window	= new pincStepperMove();

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

	jog_panel->wheel_config(&spi_mutex, config.spi_fpga_speed);

	vlayout->addWidget(ctrl_panel);
	vlayout->addWidget(jog_panel);
	hlayout->addWidget(steppers);
	hlayout->addLayout(vlayout);
	hlayout->addWidget(move_window);

	central->setLayout(hlayout);
	mainWindow->setCentralWidget(central);
	mainWindow->setStepperControl(steppers);

	QObject::connect(	ctrl_panel,
						&pincControlModeButtons::controlModeChange,
						mainWindow,
						&pincMainWindow::setControlMode);

	qRegisterMetaType<PINC_AXIS>();
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