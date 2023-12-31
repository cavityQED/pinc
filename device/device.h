#ifndef DEVICE_H
#define DEVICE_H

#include <QWidget>
#include <QGroupBox>
#include <QString>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

#include <mutex>
#include <vector>
#include <map>
#include <iostream>

class pincDevice : public QGroupBox
{
	Q_OBJECT

public:

	pincDevice(const QString& name = "", QWidget* parent = nullptr);
	virtual ~pincDevice() {}

	QString	name()	{return m_name;}
	uint32_t id()	{return m_id;}

	static pincDevice* get(uint32_t id);

	virtual bool event(QEvent* ev) override;

protected:

	static uint32_t							__id;
	static std::map<uint32_t, pincDevice*>	__deviceIDmap;

	QString		m_name;
	uint32_t	m_id;

	QGroupBox*	m_groupbox;

};

#endif