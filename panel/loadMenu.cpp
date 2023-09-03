#include "loadMenu.h"

loadMenu::loadMenu(QWidget* parent) : QMenu("Load", parent)
{
	m_load = new QAction("G Program");
	addAction(m_load);

	connect(m_load, &QAction::triggered, this, &loadMenu::load);
}

void loadMenu::load()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open G Code File", "~/Desktop");
	QFile file(filename);

	if(!file.open(QFile::ReadOnly))
		return;

	else
		emit gProgramLoadRequest(file.readAll());
}