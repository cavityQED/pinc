#include "pinc.h"

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

	style 			= new pincStyle();
	mainWindow		= new pincMainWindow();
	centralWindow	= new pincCentralWindow();
	editWindow		= new pincEditWindow();
	ctrlModePanel	= new pincControlModeButtons();
	jogPanel		= new pincJogControl();
	steppers		= new pincStepperControl();
	toolbar			= new gProgramToolBar();
	processor		= new pincProcessor();
	curProgram		= new gProgram();
	mdiProgram		= new gProgram();
	autoProgram		= new gProgram();

	app.setStyle(style);
	qRegisterMetaType<PINC_AXIS>();

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

	jogPanel->wheel_config(&spi_mutex, config.spi_fpga_speed);

	QVBoxLayout* vlayout = new QVBoxLayout();
	QHBoxLayout* hlayout = new QHBoxLayout();

	vlayout->addWidget(steppers);
	vlayout->addWidget(ctrlModePanel);
	vlayout->addWidget(jogPanel);
	hlayout->addLayout(vlayout);
	hlayout->addWidget(editWindow);
	hlayout->setStretchFactor(vlayout, 0);
	hlayout->setStretchFactor(editWindow, 1);

	centralWindow->addLayout(hlayout);
	mainWindow->addToolBar(toolbar);
	mainWindow->setCentralWidget(centralWindow);

	QObject::connect(	toolbar,		&gProgramToolBar::clear,					editWindow,		&pincEditWindow::clear		);
	QObject::connect(	toolbar,		&gProgramToolBar::hold,						processor,		&pincProcessor::hold		);
	QObject::connect(	toolbar,		&gProgramToolBar::reset,					processor,		&pincProcessor::reset		);
	QObject::connect(	toolbar,		&gProgramToolBar::run,						processor,		&pincProcessor::run			);
	QObject::connect(	steppers,		&pincStepperControl::moveReady,				processor,		&pincProcessor::next		);
	QObject::connect(	processor,		&pincProcessor::blockReady,					steppers,		&pincStepperControl::run	);

	QObject::connect(	jogPanel,		&pincJogControl::jog,						processor,		&pincProcessor::jogHandle	);
	QObject::connect(	processor,		&pincProcessor::jog,						steppers,		&pincStepperControl::jog	);
	QObject::connect(	processor,		&pincProcessor::home,						steppers,		&pincStepperControl::home	);

	QObject::connect(	processor,		&pincProcessor::jogEnable,					jogPanel,		&QGroupBox::setChecked		);
	QObject::connect(	processor,		&pincProcessor::modeChangeEnable,			ctrlModePanel,	&QGroupBox::setChecked		);
	QObject::connect(	processor,		&pincProcessor::editEnable,					editWindow,		&QGroupBox::setChecked		);

	QObject::connect(	ctrlModePanel,	&pincControlModeButtons::controlModeChange,	processor,		&pincProcessor::setMode		);
	QObject::connect(	ctrlModePanel,	
						&pincControlModeButtons::controlModeChange,
						[] (CONTROL_MODE mode)
						{
							if(mode == AUTO_CTRL)
								curProgram = autoProgram;
							else if(mode == MDI_CTRL)
								curProgram = mdiProgram;

							editWindow->setProgram(curProgram);
							processor->setProgram(curProgram);
							processor->setMode(mode);
						}	
					);

	mainWindow->show();

	return app.exec();
}