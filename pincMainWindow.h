#ifndef PINC_MAIN_WINDOW_H
#define PINC_MAIN_WINDOW_H

#include "device/espStepperGroup.h"
#include "panel/controlPanel.h"
#include "panel/positionReadout.h"

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>

class pincMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	pincMainWindow();
	~pincMainWindow() {}

signals:

	void loadFromFileRequest(const QString& filename);

protected:


	QMenu*	m_fileMenu;
	QMenu*	m_editMenu;
	QMenu*	m_loadMenu;
	QMenu*	m_configMenu;

	QAction*	m_gProgramLoadFromFileAction;
};

#endif