#include "pincPanel.h"

pincPanel::pincPanel(const QString& title, QWidget* parent) : QGroupBox(title, parent)
{
	setFrameState(QStyle::State_Sunken);
	setPalette(pincStyle::pincGroupBoxPalette);
	setCheckable(true);

	QFont f(font());
	f.setWeight(QFont::Bold);
	setFont(f);
}

void pincPanel::paintEvent(QPaintEvent* event)
{
	QGroupBox::paintEvent(event);
	QStylePainter			painter(this);
	QStyleOptionGroupBox	option;
	initStyleOption(&option);

	option.subControls	|= QStyle::SC_GroupBoxFrame;
	option.subControls &= ~QStyle::SC_GroupBoxCheckBox;
	option.lineWidth = 5;
	option.state |= frameState();

	if(isCheckable())
		option.state |= (isChecked()? QStyle::State_On : QStyle::State_Off);

	option.textColor = option.palette.color(QPalette::WindowText);

	style()->drawComplexControl(QStyle::CC_GroupBox, &option, &painter, this);
}