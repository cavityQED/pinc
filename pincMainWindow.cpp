#include "pincMainWindow.h"

pincMainWindow::pincMainWindow()
{
	m_fileMenu		= new QMenu("File");
	m_editMenu		= new QMenu("Edit");
	m_loadMenu		= new QMenu("Load");
	m_configMenu	= new QMenu("Configure");

	m_gProgramLoadFromFileAction = new QAction("G Program");
	m_loadMenu->addAction(m_gProgramLoadFromFileAction);

	menuBar()->addMenu(m_fileMenu);
	menuBar()->addMenu(m_editMenu);
	menuBar()->addMenu(m_loadMenu);
	menuBar()->addMenu(m_configMenu);

	connect(	m_gProgramLoadFromFileAction,
				&QAction::triggered,
				[this]()
				{
					QString filename = QFileDialog::getOpenFileName(this, "G Code File");
					if(filename.size())
						emit this->loadFromFileRequest(filename);
				});
}