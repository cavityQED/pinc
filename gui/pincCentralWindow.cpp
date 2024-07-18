#include "pincCentralWindow.h"

pincCentralWindow::pincCentralWindow(QWidget* parent) : QWidget(parent)
{
	vlayout = new QVBoxLayout();
	vlayout->setContentsMargins(10,10,10,10);
	vlayout->setSpacing(10);
	setLayout(vlayout);
	setPalette(pincStyle::pincWindowPalette);
}