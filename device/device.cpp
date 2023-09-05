#include "device.h"

uint32_t							pincDevice::__id			= 0;
std::map<uint32_t, pincDevice*>		pincDevice::__deviceIDmap	= {};

pincDevice::pincDevice(const QString& name, QWidget* parent) : QWidget(parent), m_name(name), m_id(++__id)
{
	m_groupbox = new QGroupBox(this);
	m_groupbox->resize(320,480);
	m_groupbox->setTitle(m_name);

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(m_groupbox);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

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