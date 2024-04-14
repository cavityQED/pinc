#include "gui/pinc_gui.h"

#include <QApplication>

#include <signal.h>

#include <pigpio.h>

#define SYNC_PIN	18

// static QTimer*				timer;
// static uint8_t				mode;
// static std::vector<gBlock*>	program;
// static size_t				program_idx;

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
	QWidget*				central		= new QWidget();
	QVBoxLayout*			vlayout		= new QVBoxLayout();

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
						mainWindow,
						&pincMainWindow::jog);

	mainWindow->setStyleSheet(	
		"QMainWindow {"
			"background-color:	#525151;"
			"}"

		"QPushButton{"	
			"background-color:	#50918E;"
			"border-style:		outset;"
			"border-width:		2px;"
			"border-color:		#408DA0;"
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
			"border-color:		#049434;"
			"}"

		"QGroupBox{"
			"border-style:		inset;"
			"border: 			1px solid #FA5898;"
			"margin-top:		10px;"
			"outline:			0;"
			"font:				bold 10pt;"
			"}"
			
		"QGroupBox::title{"
			"subcontrol-position:	top left;"
			"subcontrol-origin:		margin;"
			"top: 					2px;"
			"left:					10px;"
			"}"
	);

	mainWindow->show();

	return app.exec();
}