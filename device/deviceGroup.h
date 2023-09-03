#ifndef DEVICE_GROUP_H
#define DEVICE_GROUP_H

#include <algorithm>

#include "device.h"

class deviceGroup : public QGroupBox
{
	Q_OBJECT

public:

	deviceGroup(const QString& name = "", QWidget* parent = nullptr);
	~deviceGroup() {}

	void addDevice(pincDevice* dev)
	{
		if(std::find(m_devices.begin(), m_devices.end()) == m_devices.end())
			m_devices.push_back(dev);
	}

protected:

	std::vector<pincDevice*>	m_devices;

};


#endif