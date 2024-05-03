#include "gui/pinc_gui.h"

#include <QApplication>

#include <signal.h>

#include <pigpio.h>

#define SYNC_PIN	18

#define X_SPI_HANDSHAKE		6
#define X_STATUS_INTERRUPT	21

// static QTimer*				timer;
// static uint8_t				mode;
// static std::vector<gBlock*>	program;
// static size_t				program_idx;

static pthread_mutex_t		spi_mutex;
static pthread_mutexattr_t	spi_mutex_attr;

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

	pincMainWindow*			mainWindow	= new pincMainWindow();
	pincControlModeButtons*	ctrl_panel	= new pincControlModeButtons();
	pincJogControl*			jog_panel	= new pincJogControl();
	pincStepperControl*		steppers	= new pincStepperControl();
	QWidget*				central		= new QWidget();
	QVBoxLayout*			vlayout		= new QVBoxLayout();

	pthread_mutexattr_init(&spi_mutex_attr);
	pthread_mutexattr_settype(&spi_mutex_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_mutex_attr);

	pincStepperConfig_t	config;
	stepper_get_default_config(&config);
	config.axis			= X_AXIS;
	config.pin_status	= X_STATUS_INTERRUPT;
	config.pin_spi_hs	= X_SPI_HANDSHAKE;
	config.spi_mutex	= &spi_mutex;
	steppers->addStepper(&config);

	vlayout->addWidget(ctrl_panel);
	vlayout->addWidget(jog_panel);

	central->setLayout(vlayout);
	mainWindow->setCentralWidget(central);

	QObject::connect(	ctrl_panel,
						&pincControlModeButtons::controlModeChange,
						mainWindow,
						&pincMainWindow::setControlMode);

	QObject::connect(	jog_panel,
						&pincJogControl::jog,
						steppers,
						&pincStepperControl::jog);

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
	);

	mainWindow->show();

	return app.exec();
}