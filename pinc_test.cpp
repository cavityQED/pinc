#include "device/device.h"
#include "device/espStepperMotor.h"
#include "panel/controlPanel.h"
#include "common/motor_common.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>

#include <ctime>
#include <chrono>

espStepperMotor* xaxis;
espStepperMotor* yaxis;

class mainWindow : public QMainWindow
{

public:

	mainWindow(QWidget* parent = nullptr)
	{
		auto menu = menuBar();
		auto file = menu->addMenu("File");

		auto quit = file->addAction("Quit");
		QObject::connect(quit, &QAction::triggered, this, &QMainWindow::close);

		QToolBar* tb = new QToolBar();
		tb->setMovable(false);

		addToolBar(Qt::LeftToolBarArea, tb);
	}

	~mainWindow() {}

protected:

};

void jog(AXIS a, bool dir)
{
	if(a == X_AXIS)
		xaxis->jog(dir);
	else if(a == Y_AXIS)
		yaxis->jog(dir);
}

void setMode(uint8_t mode)
{
	switch(mode)
	{
		case JOG:
		{
			xaxis->set_mode(JOG_MODE);
			yaxis->set_mode(JOG_MODE);
			break;
		}

		case AUTO:
		{
			xaxis->set_mode(SYNC_MODE);
			yaxis->set_mode(SYNC_MODE);
			break;
		}

		default:
			break;
	}
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	gpioInitialise();

	xaxis = new espStepperMotor(17, "X Axis");
	yaxis = new espStepperMotor(27, "Y Axis");

	xaxis->set_axis(X_AXIS);
	xaxis->set_mode(JOG_MODE);
	xaxis->set_jog_steps(400);
	xaxis->set_accel(240000);
	xaxis->set_min_speed(4);

	yaxis->set_axis(Y_AXIS);
	yaxis->set_mode(JOG_MODE);
	yaxis->set_jog_steps(400);
	yaxis->set_accel(240000);
	yaxis->set_min_speed(4);

	controlPanel* panel = new controlPanel();

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(xaxis);
	layout->addWidget(yaxis);
	layout->addWidget(panel);

	QObject::connect(panel, &controlPanel::axisButton, [](AXIS a, bool dir)	{jog(a, dir);});
	QObject::connect(panel, &controlPanel::modeChange, [](uint8_t mode)		{setMode(mode);});

	QWidget* centralWidget = new QWidget();
	centralWidget->setLayout(layout);

	mainWindow* window = new mainWindow();
	window->setStyleSheet(	"QGroupBox{"
							"border:		2px solid gray;"
							"margin-top:	1ex;}");
	window->setCentralWidget(centralWidget);
	window->show();

	return app.exec();
}