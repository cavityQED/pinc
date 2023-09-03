#include "deviceGroup.h"

deviceGroup::deviceGroup(const QString& name, QWidget* parent) : QGroupBox(parent)
{
	setTitle(name);
}