#include "pincMainWindow.h"

pincMainWindow::pincMainWindow(QWidget* parent) : QMainWindow(parent)
{
	m_ctrl_mode = EDIT_CTRL;

	setPalette(pincStyle::pincWindowPalette);
}

void pincMainWindow::setControlMode(CONTROL_MODE mode)
{
	m_ctrl_mode = mode;

	switch(m_ctrl_mode)
	{
		case AUTO_CTRL:
		{
			printf("AUTO Mode\n");
			break;
		}

		case HOME_CTRL:
		{
			printf("HOME Mode\n");
			break;
		}

		case EDIT_CTRL:
		{
			printf("EDIT Mode\n");
			break;
		}

		case MDI_CTRL:
		{
			printf("MDI Mode\n");
			break;
		}

		case MANUAL_CTRL:
		{
			printf("MANUAL Mode\n");
			break;
		}

		case JOG_CTRL:
		{
			printf("JOG Mode\n");
			break;
		}

		default:
			break;
	}
}

void pincMainWindow::tryjog(const PINC_AXIS axis, bool dir)
{
	if(m_ctrl_mode == JOG_CTRL)
		jog(axis, dir);

	else if(m_ctrl_mode == HOME_CTRL)
		home(axis);
}