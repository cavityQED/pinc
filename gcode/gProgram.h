#ifndef G_PROGRAM_H
#define G_PROGRAM_H

#include "common/motor_common.h"
#include "gBlock.h"

#include <QWidget>
#include <QStringRef>

class gProgram : public QWidget
{
	Q_OBJECT

public:

	gProgram(QWidget* parent = nullptr);
	gProgram(const QString& str, QWidget* parent = nullptr);
	gProgram(const std::string& str, QWidget* parent = nullptr);
	~gProgram() {}

	void getBlocks(const QString& str);

	gBlock*			operator[](size_t idx)	{return m_blocks[idx];}
	gBlock*			at(size_t idx)			{return m_blocks.at(idx);}
	const QString&	str()					{return m_str;}

	size_t size() {return m_blocks.size();}

protected:

	QString					m_str;
	std::vector<gBlock*>	m_blocks;
};

#endif