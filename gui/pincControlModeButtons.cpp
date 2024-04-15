#include "pincControlModeButtons.h"

pincControlModeButtons::pincControlModeButtons(QWidget* parent) : QGroupBox(parent)
{
	m_button_jog	= new QPushButton("JOG");
	m_button_auto	= new QPushButton("AUTO");
	m_button_edit	= new QPushButton("EDIT");
	m_button_mdi	= new QPushButton("MDI");
	m_button_home	= new QPushButton("HOME");
	m_button_man	= new QPushButton("MAN");

	m_button_jog->setCheckable(true);
	m_button_auto->setCheckable(true);
	m_button_edit->setCheckable(true);
	m_button_mdi->setCheckable(true);
	m_button_home->setCheckable(true);
	m_button_man->setCheckable(true);

	connect(m_button_jog, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::JOG);		
					else return;
		});

	connect(m_button_auto, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::AUTO);		
					else return;
		});

	connect(m_button_edit, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::EDIT);
					else return;
		});

	connect(m_button_mdi, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::MDI);		
					else return;
		});

	connect(m_button_home, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::HOME);		
					else return;
		});

	connect(m_button_man, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::MANUAL);	
					else return;
		});


	QButtonGroup* mode_button_group = new QButtonGroup();
	mode_button_group->setExclusive(true);
	mode_button_group->addButton(m_button_jog);
	mode_button_group->addButton(m_button_auto);
	mode_button_group->addButton(m_button_edit);
	mode_button_group->addButton(m_button_mdi);
	mode_button_group->addButton(m_button_home);
	mode_button_group->addButton(m_button_man);

	QGridLayout* layout = new QGridLayout();
	layout->addWidget(m_button_jog, 0, 0);
	layout->addWidget(m_button_auto, 0, 1);
	layout->addWidget(m_button_home, 0, 2);
	layout->addWidget(m_button_edit, 1, 0);
	layout->addWidget(m_button_mdi, 1, 1);
	layout->addWidget(m_button_man, 1, 2);

	setLayout(layout);
	setTitle("Control Mode");

}