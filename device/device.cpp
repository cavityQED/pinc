#include "device.h"

uint32_t							pincDevice::id	= 0;
std::map<uint32_t, pincDevice*>		pincDevice::deviceIDmap		= {};

pincDevice::pincDevice(const QString& name, QWidget* parent) : QWidget(parent), m_name(name)
{
	m_groupbox = new QGroupBox(this);
	m_groupbox->resize(320,480);
	m_groupbox->setTitle(m_name);

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(m_groupbox);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

pincDevice* pincDevice::get(uint32_t id)
{
	if(deviceIDmap.find(id) != deviceIDmap.end())
		return deviceIDmap[id];
	else
		return nullptr;
}

bool pincDevice::event(QEvent* ev)
{

	switch(ev->type())
	{


		default:
			return QWidget::event(ev);
	}
}