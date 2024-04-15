#include "pincMainWindow.h"

pincMainWindow::pincMainWindow(QWidget* parent) : QMainWindow(parent)
{


}

void pincMainWindow::setControlMode(CONTROL_MODE mode)
{
	switch(mode)
	{
		case AUTO:
		{
			printf("AUTO Mode\n");
			break;
		}

		case HOME:
		{
			printf("HOME Mode\n");
			break;
		}

		case EDIT:
		{
			printf("EDIT Mode\n");
			break;
		}

		case MDI:
		{
			printf("MDI Mode\n");
			break;
		}

		case MANUAL:
		{
			printf("MANUAL Mode\n");
			break;
		}

		case JOG:
		{
			printf("JOG Mode\n");
			break;
		}

		default:
			break;
	}
}

void pincMainWindow::jog(AXIS axis, bool dir)
{
	const char* dir_string = (dir)? "Positive" : "Negative";
	switch(axis)
	{
		case X_AXIS:
		{
			printf("Jogging X Axis in %s Direction\n", dir_string);
			break;
		}

		case Y_AXIS:
		{
			printf("Jogging Y Axis in %s Direction\n", dir_string);
			break;
		}

		case Z_AXIS:
		{
			printf("Jogging Z Axis in %s Direction\n", dir_string);
			break;
		}

		default:
			break;
	}
}