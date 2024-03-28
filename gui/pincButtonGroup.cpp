#include "pinc_buttonGroup.h"

pincButtonGroup::pincButtonGroup(QWidget* parent) : QWidget(parent)
{
	QPushButton*	button;
	QButtonGroup*	group	= new QButtonGroup;
	QGridLayout*	layout	= new QGridLayout;
	QGroupBox*		box		= new QGroupBox;

	group->setExclusive(true);

	for(int r = 0; r < 3; r++)
	{
		for(int c = 0; c < 3; c++)
		{
			button = new QPushButton();
			button->setCheckable(true);
			group->addButton(button);
			layout->addWidget(button, r, c);
		}
	}

	setLayout(layout);
}