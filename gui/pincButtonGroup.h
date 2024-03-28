#ifndef PINC_BUTTON_GROUP
#define PINC_BUTTON_GROUP

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QGroupBox>
#include <QVector>

class pincButtonGroup : public QWidget
{
	Q_OBJECT

public:

	pincButtonGroup(QWidget* parent = nullptr);
	~pincButtonGroup() {}

};


#endif