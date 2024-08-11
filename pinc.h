#ifndef PINC_H
#define PINC_H

#include "gui/pinc_gui.h"
#include "pincProcessor.h"

#include <QApplication>
#include <QAction>
#include <QMetaType>

#include <signal.h>

#include <pigpio.h>

Q_DECLARE_METATYPE(PINC_AXIS);
Q_DECLARE_METATYPE(CONTROL_MODE);

static pthread_mutex_t			spi_mutex;
static pthread_mutex_t			pin_req_mutex;
static pthread_mutexattr_t		spi_mutex_attr;

static pincStyle*				style;
static pincMainWindow*			mainWindow;
static pincCentralWindow*		centralWindow;
static pincEditWindow*			editWindow;
static pincControlModeButtons*	ctrlModePanel;
static pincJogControl*			jogPanel;
static pincStepperControl*		steppers;
static gProgramToolBar*			toolbar;
static pincProcessor*			processor;

static gProgram*				autoProgram;
static gProgram*				mdiProgram;
static gProgram*				curProgram;

#endif