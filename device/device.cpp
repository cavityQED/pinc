#include "device.h"

uint32_t							pincDevice::__id			= 0;
std::map<uint32_t, pincDevice*>		pincDevice::__deviceIDmap	= {};

pincDevice::pincDevice(const QString& name, QWidget* parent) : QGroupBox(parent), m_name(name), m_id(++__id)
{
	setTitle(m_name);

	__deviceIDmap.insert(std::make_pair(m_id, this));
}

pincDevice* pincDevice::get(uint32_t id)
{
	if(__deviceIDmap.find(id) != __deviceIDmap.end())
		return __deviceIDmap[id];
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