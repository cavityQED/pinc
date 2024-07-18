#include "pincEditWindow.h"

pincLabelScroll::pincLabelScroll(QWidget* parent) : QScrollArea(parent)
{
	setPalette(pincStyle::pincLineEditPalette);
	setBackgroundRole(QPalette::Window);

	pincLabel* label;
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);

	for(int i = 0; i < 20; i++)
	{
		label = new pincLabel("Label " + QString::number(i));
		label->setMinimumHeight(30);
		label->setMaximumHeight(30);
		layout->addWidget(label);
	}

	QWidget* widget = new QWidget();
	widget->setLayout(layout);
	setWidget(widget);
}

pincEditWindow::pincEditWindow(QWidget* parent) : pincPanel("", parent)
{
	m_text_panel	= new pincPanel("Program");
	m_line_panel	= new pincPanel("Input");
	m_line_input	= new QLineEdit();
	m_scroll		= new pincLabelScroll();

	m_text_panel->clrFrameState(QStyle::State_Sunken);
	m_line_panel->clrFrameState(QStyle::State_Sunken);
	m_line_input->setPalette(pincStyle::pincLineEditPalette);

	QVBoxLayout* textlayout = new QVBoxLayout();
	textlayout->addWidget(m_scroll);
	m_text_panel->setLayout(textlayout);

	QVBoxLayout* linelayout = new QVBoxLayout();
	linelayout->addWidget(m_line_input);
	m_line_panel->setLayout(linelayout);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_text_panel);
	layout->addWidget(m_line_panel);
	layout->setStretchFactor(m_text_panel, 1);
	layout->setStretchFactor(m_line_panel, 0);
	layout->setContentsMargins(0,0,0,0);

	setLayout(layout);
}