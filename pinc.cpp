#include "gui/pinc_gui.h"

#include <QApplication>
#include <QAction>

#include <signal.h>

#include <pigpio.h>

// static QTimer*				timer;
// static uint8_t				mode;
// static std::vector<gBlock*>	program;
// static size_t				program_idx;

static pthread_mutex_t		spi_mutex;
static pthread_mutex_t		pin_req_mutex;
static pthread_mutexattr_t	spi_mutex_attr;

static pincStepperControl*	steppers;
static pincStepperMove_t	sync_move;
static pincStepperMove_t	curve_move;

static void shutdown(int signum)
{
	gpioTerminate();
	exit(signum);
}

static void sync_test(bool checked)
{
	printf("SYNC TEST\n");
	steppers->sync_move(&sync_move);
}

static void curve_test(bool checked)
{
	printf("CURVE TEST\n");
	steppers->sync_move(&curve_move);
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
	
	steppers	= new pincStepperControl();

	pthread_mutexattr_init(&spi_mutex_attr);
	pthread_mutexattr_settype(&spi_mutex_attr, PTHREAD_MUTEX_ERRORCHECK | PTHREAD_MUTEX_DEFAULT);
	pthread_mutex_init(&spi_mutex, &spi_mutex_attr);
	pthread_mutex_init(&pin_req_mutex, &spi_mutex_attr);

	pincStepperConfig_t	config;
	stepper_get_default_config(&config);
	config.axis				= X_AXIS;
	config.pin_status		= X_STATUS_INTERRUPT;
	config.pin_spi_hs		= X_SPI_HANDSHAKE;
	config.spi_mutex		= &spi_mutex;
	config.pin_req_mutex	= &pin_req_mutex;
	config.accel			= 192000;
	steppers->addStepper(&config);
	config.axis				= Y_AXIS;
	config.pin_status		= Y_STATUS_INTERRUPT;
	config.pin_spi_hs		= Y_SPI_HANDSHAKE;
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

	memset(&sync_move, 0, sizeof(pincStepperMove_t));
	memset(&curve_move, 0, sizeof(pincStepperMove_t));

	sync_move.mode	= LINE_MOVE | SYNC_MOVE;
	sync_move.v_sps	= config.jog_speed;
	sync_move.end.x	= 10 * config.spmm;
	sync_move.end.y = 10 * config.spmm;


	curve_move.mode		= CURVE_MOVE | SYNC_MOVE;
	curve_move.v_sps	= config.jog_speed;
	curve_move.end.x	= 10 * config.spmm;
	curve_move.center.x = curve_move.end.x / 2;
	curve_move.radius	= curve_move.end.x / 2;
	curve_move.cw		= 1;

	QAction* test_move = new QAction(ctrl_panel);
	ctrl_panel->addAction(test_move);
	test_move->setShortcut(Qt::Key_F1);
	QObject::connect(	test_move,
						&QAction::triggered,
						sync_test);

	QAction* curve_action = new QAction(ctrl_panel);
	ctrl_panel->addAction(curve_action);
	curve_action->setShortcut(Qt::Key_F2);
	QObject::connect(	curve_action,
						&QAction::triggered,
						curve_test);

	return app.exec();
}