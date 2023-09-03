#include "device/espStepperMotor.h"
#include "device/espStepperGroup.h"
#include "device/laser.h"
#include "panel/controlPanel.h"
#include "panel/positionReadout.h"
#include "gcode/gBlock.h"
#include "common/motor_common.h"
#include "pincLaser2D.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>

#include <signal.h>

#define SYNC_PIN	18

static espStepperMotor* 	xaxis;
static espStepperMotor* 	yaxis;
static espStepperGroup*		group;
static controlPanel*		panel;
static Laser*				laser;
static QTimer*				timer;
static uint8_t				mode;
static std::vector<gBlock*>	program;
static size_t				program_idx;

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

	espStepperMotor::config_t xconfig;
	xconfig.pin			= 17;
	xconfig.axis		= X_AXIS;
	xconfig.spr			= 3200;
	xconfig.mmpr		= 8;
	xconfig.accel		= 320000;
	xconfig.jog_speed	= 40;
	xconfig.min_speed	= 10;
	xconfig.max_speed	= 60;

	espStepperMotor::config_t yconfig;
	yconfig.pin			= 27;
	yconfig.axis		= Y_AXIS;
	yconfig.spr			= 1600;
	yconfig.mmpr		= 4;
	yconfig.accel		= 160000;
	yconfig.jog_speed	= 40;
	yconfig.min_speed	= 10;
	yconfig.max_speed	= 60;

	QMainWindow*	mainWindow		= new QMainWindow();
	loadMenu*		load_menu		= new loadMenu();
	pincLaser2D* 	laserController	= new pincLaser2D(xconfig, yconfig);

	QObject::connect(load_menu, &loadMenu::gProgramLoadRequest, laserController, &pincLaser2D::loadFromFile);
	mainWindow->menuBar()->addMenu(load_menu);
	mainWindow->setCentralWidget(laserController);
	mainWindow->show();

	return app.exec();
}