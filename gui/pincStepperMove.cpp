#include "pincStepperMove.h"

pincStepperMove::pincStepperMove(QWidget* parent) : QGroupBox(parent)
{
	QLabel* 		label;
	QHBoxLayout*	hlayout;

	vlayout		= new QVBoxLayout();

	edit_x0		= new QLineEdit();
	edit_y0		= new QLineEdit();
	edit_z0		= new QLineEdit();
	label		= new QLabel("Start Position mm (x0,y0,z0):");
	hlayout		= new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(edit_x0);
	hlayout->addWidget(edit_y0);
	hlayout->addWidget(edit_z0);
	vlayout->addLayout(hlayout);

	edit_xf		= new QLineEdit();
	edit_yf		= new QLineEdit();
	edit_zf		= new QLineEdit();
	label		= new QLabel("End Position mm (xf,yf,zf):");
	hlayout		= new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(edit_xf);
	hlayout->addWidget(edit_yf);
	hlayout->addWidget(edit_zf);
	vlayout->addLayout(hlayout);

	edit_xc		= new QLineEdit();
	edit_yc		= new QLineEdit();
	edit_zc		= new QLineEdit();
	label		= new QLabel("Center Position mm (xc,yc,zc):");
	hlayout		= new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(edit_xc);
	hlayout->addWidget(edit_yc);
	hlayout->addWidget(edit_zc);
	vlayout->addLayout(hlayout);

	edit_v0		= new QLineEdit();
	edit_vf		= new QLineEdit();
	label		= new QLabel("Speeds mm/s (v0, vf):");
	hlayout		= new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(edit_v0);
	hlayout->addWidget(edit_vf);
	vlayout->addLayout(hlayout);

	edit_accel	= new QLineEdit();
	label		= new QLabel("Accel (mm/s/s):");
	hlayout		= new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(edit_accel);
	vlayout->addLayout(hlayout);

	edit_radius	= new QLineEdit();
	radio_cw	= new QRadioButton("Clockwise:");
	label		= new QLabel("Radius (mm):");
	hlayout		= new QHBoxLayout();
	hlayout->addWidget(label);
	hlayout->addWidget(edit_radius);
	hlayout->addWidget(radio_cw);
	vlayout->addLayout(hlayout);

	button_move	= new QPushButton("Move");
	button_clr	= new QPushButton("Clear");
	hlayout		= new QHBoxLayout();
	hlayout->addWidget(button_move);
	hlayout->addWidget(button_clr);
	vlayout->addLayout(hlayout);

	setLayout(vlayout);

	connect(	button_move,
				&QPushButton::clicked,
				this,
				[this](){this->make_move();});

	connect(	button_clr,
				&QPushButton::clicked,
				this,
				[this](){clear();});

	clear();
}

void pincStepperMove::make_move()
{
	memset(&m_move, 0, sizeof(pincStepperMove_t));

	m_move.v0_sps	= edit_v0->text().toUInt();
	m_move.vf_sps	= edit_vf->text().toUInt();
	m_move.accel	= edit_accel->text().toUInt();

	m_move.cur.x	= edit_x0->text().toInt();
	m_move.cur.y	= edit_y0->text().toInt();
	m_move.cur.z	= edit_z0->text().toInt();

	m_move.end.x 	= edit_xf->text().toInt();
	m_move.end.y 	= edit_yf->text().toInt();
	m_move.end.z 	= edit_zf->text().toInt();

	m_move.center.x 	= edit_xc->text().toInt();
	m_move.center.y 	= edit_yc->text().toInt();
	m_move.center.z 	= edit_zc->text().toInt();

	m_move.radius	= edit_radius->text().toUInt();
	m_move.cw		= radio_cw->isChecked();

	if(m_move.radius)
		m_move.mode = CURVE_MOVE;
	else
		m_move.mode = LINE_MOVE;

	emit move(&m_move);
}

void pincStepperMove::clear()
{
	edit_v0		->setText("10");	
	edit_vf		->setText("40");	
	edit_x0		->setText("0");	
	edit_y0		->setText("0");	
	edit_z0		->setText("0");	
	edit_xf		->setText("0");	
	edit_yf		->setText("0");	
	edit_zf		->setText("0");
	edit_xc		->setText("0");	
	edit_yc		->setText("0");	
	edit_zc		->setText("0");	
	edit_accel	->setText("5000");		
	edit_radius	->setText("0");		
}
