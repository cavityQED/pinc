#ifndef LOAD_MENU_H
#define LOAD_MENU_H

#include <QMenu>
#include <QFile>
#include <QFileDialog>
#include <QAction>

#include "gcode/gProgram.h"

class loadMenu : public QMenu
{
	Q_OBJECT

public:

	loadMenu(QWidget* parent = nullptr);
	~loadMenu() {}

	void load();

signals:

	void gProgramLoadRequest(const QString& str);

protected:

	QAction*	m_load;
};

#endif