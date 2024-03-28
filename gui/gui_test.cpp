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

	pincButtonGroup* group = new pincButtonGroup();

	group->show();

	return app.exec();
}