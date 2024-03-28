#include "pinc_gui.h"

#include <QApplication>

#include <signal.h>

static void shutdown(int signum)
{
	exit(signum);
}

int main(int argc, char *argv[])
{	
	QApplication app(argc, argv);

	pincMainWindow*		mainWindow = new pincMainWindow();
	pincButtonGroup* 	group = new pincButtonGroup();

	mainWindow->setCentralWidget(group);
	mainWindow->show();

	return app.exec();
}