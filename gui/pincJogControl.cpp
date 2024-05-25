#include "pincJogControl.h"

pincJogControl::pincJogControl(QWidget* parent) : QGroupBox(parent)
{
	m_button_max_jog	= new QPushButton("100");
	m_button_med_jog	= new QPushButton("10"); 
	m_button_min_jog	= new QPushButton("1");

	m_button_max_jog->setCheckable(true);
	m_button_med_jog->setCheckable(true);
	m_button_min_jog->setCheckable(true);

	QButtonGroup* steps_group = new QButtonGroup();
	steps_group->addButton(m_button_max_jog);
	steps_group->addButton(m_button_med_jog);
	steps_group->addButton(m_button_min_jog);
	steps_group->setExclusive(true);

	connect(m_button_max_jog, &QPushButton::toggled,
		[this](bool checked)
		{
			if(checked)
				setJogSteps(800);
			else return;
		});

	connect(m_button_med_jog, &QPushButton::toggled,
		[this](bool checked)
		{
			if(checked)
				setJogSteps(400);
			else return;
		});

	connect(m_button_min_jog, &QPushButton::toggled,
		[this](bool checked)
		{
			if(checked)
				setJogSteps(100);
			else return;
		});

	m_button_x_pos		= new QPushButton("X+");
	m_button_x_neg		= new QPushButton("X-");
	m_button_y_pos		= new QPushButton("Y+");
	m_button_y_neg		= new QPushButton("Y-");
	m_button_z_pos		= new QPushButton("Z+");
	m_button_z_neg		= new QPushButton("Z-");

	m_button_x_pos->setShortcut(Qt::Key_D);
	m_button_x_neg->setShortcut(Qt::Key_A);
	m_button_y_pos->setShortcut(Qt::Key_W);
	m_button_y_neg->setShortcut(Qt::Key_S);

	connect(m_button_x_pos, &QPushButton::clicked, [this] {jog(X_AXIS, 1);});
	connect(m_button_x_neg, &QPushButton::clicked, [this] {jog(X_AXIS, 0);});
	connect(m_button_y_pos, &QPushButton::clicked, [this] {jog(Y_AXIS, 1);});
	connect(m_button_y_neg, &QPushButton::clicked, [this] {jog(Y_AXIS, 0);});
	connect(m_button_z_pos, &QPushButton::clicked, [this] {jog(Z_AXIS, 1);});
	connect(m_button_z_neg, &QPushButton::clicked, [this] {jog(Z_AXIS, 0);});

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(m_button_max_jog, 0, 0);
	layout->addWidget(m_button_med_jog, 0, 1);
	layout->addWidget(m_button_min_jog, 0, 2);

	layout->addWidget(m_button_x_pos, 2, 2);
	layout->addWidget(m_button_x_neg, 2, 0);
	layout->addWidget(m_button_y_pos, 1, 1);
	layout->addWidget(m_button_y_neg, 3, 1);
	layout->addWidget(m_button_z_pos, 1, 2);
	layout->addWidget(m_button_z_neg, 3, 0);

	setLayout(layout);
	setTitle("Jog Control");
}