#ifndef PINC_MAIN_WINDOW
#define PINC_MAIN_WINDOW

#include <QMainWindow>

class pincMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	pincMainWindow(QWidget* parent = nullptr);
	~pincMainWindow() {}

};

#endif