#include "pincControlModeButtons.h"

pincControlModeButtons::pincControlModeButtons(QWidget* parent) : pincPanel("Control Mode", parent)
{
	m_button_jog	= new pincButton("JOG");
	m_button_auto	= new pincButton("AUTO");
	m_button_edit	= new pincButton("EDIT");
	m_button_mdi	= new pincButton("MDI");
	m_button_home	= new pincButton("HOME");
	m_button_man	= new pincButton("MAN");

	connect(m_button_jog, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::JOG_CTRL);		
			else return;
		});

	connect(m_button_auto, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::AUTO_CTRL);		
			else return;
		});

	connect(m_button_edit, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::EDIT_CTRL);
			else return;
		});

	connect(m_button_mdi, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::MDI_CTRL);		
			else return;
		});

	connect(m_button_home, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::HOME_CTRL);		
			else return;
		});

	connect(m_button_man, &QPushButton::toggled, 
		[this](bool checked) 
		{	
			if(checked) 
				controlModeChange(CONTROL_MODE::MANUAL_CTRL);	
			else return;
		});

	QButtonGroup* mode_button_group = new QButtonGroup();
	mode_button_group->addButton(m_button_jog);
	mode_button_group->addButton(m_button_auto);
	mode_button_group->addButton(m_button_edit);
	mode_button_group->addButton(m_button_mdi);
	mode_button_group->addButton(m_button_home);
	mode_button_group->addButton(m_button_man);
	mode_button_group->setExclusive(true);
	m_button_edit->setChecked(true);

	QGridLayout* grid = new QGridLayout();
	grid->addWidget(m_button_jog, 0, 0);
	grid->addWidget(m_button_auto, 0, 1);
	grid->addWidget(m_button_home, 0, 2);
	grid->addWidget(m_button_edit, 1, 0);
	grid->addWidget(m_button_mdi, 1, 1);
	grid->addWidget(m_button_man, 1, 2);
	grid->setSizeConstraint(QLayout::SetFixedSize);

	setLayout(grid);
}