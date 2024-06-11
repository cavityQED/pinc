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

	QLabel*	label = new QLabel(str);
	label->setGeometry(0,0,20,40);
	
	m_lcd = new QLCDNumber();
	m_lcd->setGeometry(40,0,100,40);

	QHBoxLayout* layout	= new QHBoxLayout();
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->addWidget(label);
	layout->addWidget(m_lcd);

	setLayout(layout);
}

void pincPosition::set(double pos)
{
	m_lcd->display(pos);
}