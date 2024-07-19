#include "pincEditWindow.h"

pincEditWindow::pincEditWindow(QWidget* parent) : pincPanel("", parent)
{
	m_text_panel	= new pincPanel("Program");
	m_line_panel	= new pincPanel("Input");
	m_line_input	= new QLineEdit();
	m_scroll		= new QScrollArea();
	m_scroll_layout	= new QVBoxLayout();
	m_cur_label		= nullptr;

	m_scroll->setPalette(pincStyle::pincLineEditPalette);
	m_scroll->setBackgroundRole(QPalette::Window);
	m_scroll_layout->setSpacing(0);
	m_scroll_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
/*
	pincLabel* label;

	for(int i = 0; i < 20; i++)
	{
		label = new pincLabel("Label " + QString::number(i));
		label->setMinimumHeight(30);
		label->setMaximumHeight(30);
		connect(label, &pincLabel::selected, this, &pincEditWindow::setCurLabel);
		m_scroll_layout->addWidget(label);
	}
*/

	QWidget* widget = new QWidget();
	widget->setLayout(m_scroll_layout);
	m_scroll->setWidget(widget);

	m_text_panel->clrFrameState(QStyle::State_Sunken);
	m_line_panel->clrFrameState(QStyle::State_Sunken);
	m_line_input->setPalette(pincStyle::pincLineEditPalette);
	m_line_input->installEventFilter(this);

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

bool pincEditWindow::eventFilter(QObject* obj, QEvent* evt)
{
	if(obj == m_line_input && evt->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(evt);

		if(keyEvent->key() == Qt::Key_Return)
		{
			if(m_cur_label)
				m_cur_label->setText(m_line_input->text());
			else
				addLine(m_line_input->text());

			m_line_input->clear();
			setCurLabel(nullptr);

			this->setFocus();

			return true;
		}

		return false;
	}

	else
		return QGroupBox::eventFilter(obj, evt);
}

void pincEditWindow::setCurLabel(pincLabel* label)
{
	if(m_cur_label)
	{
		m_cur_label->setSelected(false);
		m_cur_label->setBackgroundRole(QPalette::Window);
		m_cur_label->setForegroundRole(QPalette::Text);
	}

	m_cur_label = label;

	m_line_input->setFocus();
}

void pincEditWindow::addLine(const QString& str)
{
	pincLabel* label = new pincLabel(str);
	label->setMinimumHeight(30);
	label->setMaximumHeight(30);
	connect(label, &pincLabel::selected, this, &pincEditWindow::setCurLabel);
	m_scroll->widget()->layout()->addWidget(label);
	m_scroll->ensureWidgetVisible(label);
}