#ifndef PINC_MAIN_WINDOW
#define PINC_MAIN_WINDOW

#include <QMainWindow>

#include "c/stepper/stepper_common.h"
#include "pincStepperControl.h"
#include "pincColors.h"
#include "pincStyle.h"

class pincMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	pincMainWindow(QWidget* parent = nullptr);
	~pincMainWindow() {}

};

#endif