#include "pincMainWindow.h"

pincMainWindow::pincMainWindow(QWidget* parent) : QMainWindow(parent)
{
	setPalette(pincStyle::pincWindowPalette);
}