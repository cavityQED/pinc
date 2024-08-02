#include "gui/pinc_gui.h"

#include <QApplication>
#include <QAction>
#include <QMetaType>

#include <signal.h>

#include <pigpio.h>

static pthread_mutex_t		spi_mutex;
static pthread_mutex_t		pin_req_mutex;
static pthread_mutexattr_t	spi_mutex_attr;

static pincStyle* style;

Q_DECLARE_METATYPE(PINC_AXIS);

static void shutdown(int signum)
{
	gpioTerminate();
	exit(signum);
}

int main(int argc, char *argv[])
{	
	style = new pincStyle();
	QApplication app(argc, argv);
	app.setStyle(style);

	gpioInitialise();
	gpioSetSignalFunc(SIGINT, shutdown);

	pthread_mutexattr_init(&spi_mutex_attr);
	pthread_mutexattr_settype(&spi_mutex_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_mutex_attr);

	pincStepperControl*		steppers	= new pincStepperControl();
	pincMainWindow*			mainWindow	= new pincMainWindow();
	pincCentralWindow*		central		= new pincCentralWindow();
	pincControlModeButtons*	ctrl_panel	= new pincControlModeButtons();
	pincJogControl*			jog_panel	= new pincJogControl();
	pincEditWindow*			editWindow	= new pincEditWindow();
	QVBoxLayout*			vlayout		= new QVBoxLayout();
	QHBoxLayout*			hlayout		= new QHBoxLayout();

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

	vlayout->addWidget(steppers);
	vlayout->addWidget(ctrl_panel);
	vlayout->addWidget(jog_panel);
	hlayout->addLayout(vlayout);
	hlayout->addWidget(editWindow);
	hlayout->setStretchFactor(vlayout, 0);
	hlayout->setStretchFactor(editWindow, 1);

	central->addLayout(hlayout);
	mainWindow->setCentralWidget(central);
	mainWindow->setStepperControl(steppers);
	mainWindow->setPalette(pincStyle::pincWindowPalette);

	QObject::connect(	ctrl_panel,
						&pincControlModeButtons::controlModeChange,
						mainWindow,
						&pincMainWindow::setControlMode);

	qRegisterMetaType<PINC_AXIS>();

	QObject::connect(	jog_panel,
						&pincJogControl::jog,
						mainWindow,
						&pincMainWindow::tryjog);

	QObject::connect(	mainWindow,
						&pincMainWindow::jog,
						steppers,
						&pincStepperControl::jog);

	QObject::connect(	mainWindow,
						&pincMainWindow::home,
						steppers,
						&pincStepperControl::home);


	QAction* mdi_run = new QAction();
	mainWindow->addAction(mdi_run);
	mdi_run->setShortcut(Qt::Key_F1);
	QObject::connect(mdi_run, &QAction::triggered, [editWindow, steppers]() {steppers->run(*editWindow->getBlock());});

	mainWindow->show();

	return app.exec();
}