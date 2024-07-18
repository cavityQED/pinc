#include "pincPosition.h"


pincPosition::pincPosition(PINC_AXIS axis, QWidget* parent) : QWidget(parent)
{
	QString str;
	switch(axis)
	{
		case X_AXIS:
			str = "X";
			break;
		case Y_AXIS:
			str = "Y";
			break;
		case Z_AXIS:
			str = "Z";
			break;
		default:
			str = "";
			break;
	}

	QFont linefont;
	linefont.setWeight(QFont::Bold);
	m_line = new QLineEdit();
	m_line->setFont(linefont);
	m_line->setPalette(pincStyle::pincLineEditPalette);
	m_line->setAlignment(Qt::AlignRight);
	m_line->setMinimumHeight(50);
	m_line->setReadOnly(true);

	QLabel*			label	= new QLabel(str);
	QHBoxLayout*	layout	= new QHBoxLayout();

	label->setPalette(pincStyle::pincGroupBoxPalette);
	layout->addWidget(label);
	layout->addWidget(m_line);

	setLayout(layout);
	set(0);
}

void pincPosition::set(double pos)
{
	m_line->setText(QString::number(pos, 'f', 2));
}