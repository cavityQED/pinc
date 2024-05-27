#ifndef PINC_STEPPER_MOVE_H
#define PINC_STEPPER_MOVE_H

#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

extern "C" {
	#include "c/stepper/pi_stepper.h" 
}

class pincStepperMove : public QGroupBox
{
	Q_OBJECT

public:

	pincStepperMove(QWidget* parent = nullptr);
	~pincStepperMove() {}

	pincStepperMove_t* get_move() {return &m_move;}

public:

	void make_move();
	void clear();

signals:

	void move(pincStepperMove_t* m, bool convert = true);

protected:

	QLineEdit*		edit_v0;
	QLineEdit*		edit_vf;
	QLineEdit*		edit_x0;
	QLineEdit*		edit_y0;
	QLineEdit*		edit_z0;
	QLineEdit*		edit_xf;
	QLineEdit*		edit_yf;
	QLineEdit*		edit_zf;
	QLineEdit*		edit_xc;
	QLineEdit*		edit_yc;
	QLineEdit*		edit_zc;
	QLineEdit*		edit_accel;
	QLineEdit*		edit_radius;
	QRadioButton*	radio_cw;
	QPushButton*	button_move;
	QPushButton*	button_clr;
	QVBoxLayout*	vlayout;

	pincStepperMove_t m_move;
};

#endif