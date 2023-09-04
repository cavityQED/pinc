#include "pincMainWindow.h"
#include "pincLaser2D.h"

#include <QApplication>

#include <signal.h>

#define SYNC_PIN	18

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

	pincMainWindow*	mainWindow	= new pincMainWindow();
	pincLaser2D* 	laser2D		= new pincLaser2D();

	QObject::connect(	mainWindow,
						&pincMainWindow::loadFromFileRequest,
						laser2D,
						&pincLaser2D::loadFromFile);

	mainWindow->setCentralWidget(laser2D);
	mainWindow->show();

	return app.exec();
}