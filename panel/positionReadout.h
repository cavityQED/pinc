#ifndef POSITION_READOUT_H
#define POSITION_READOUT_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTimer>
#include <QTimerEvent>

#include <iostream>

#include "common/motor_common.h"

class positionReadout : public QGroupBox
{
	Q_OBJECT

public:
	positionReadout(QWidget *parent = 0);
	~positionReadout() {}
	
	void createWidgets();
	void setStyleSheets();
		
public slots:

	void setPosition(const position_t<double>& pos);

	void setX(double x);
	void setY(double y);
	void setZ(double z);

	QPushButton* x_zero()	{return home_x;}
	QPushButton* y_zero()	{return home_y;}
	QPushButton* z_zero()	{return home_z;}
		
private:	
	//Main Group Box Layout
	QVBoxLayout *axis_position_box_layout;
	
	//Axis-Specific Position Widgets	
	QWidget *x_pos;
	QWidget *y_pos;
	QWidget *z_pos;
	
	//Axis-Specific Line Edits
	//Positions will be displayed here
	QLineEdit *x_lineEdit;
	QLineEdit *y_lineEdit;
	QLineEdit *z_lineEdit;
	
	//Axis-Specific Layouts
	QHBoxLayout *x_pos_layout;
	QHBoxLayout *y_pos_layout;
	QHBoxLayout *z_pos_layout;

	QPushButton*	home_x;
	QPushButton*	home_y;
	QPushButton*	home_z;
	
	QString unit;
};

#endif
