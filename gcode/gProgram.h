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

	void makeBlocks(const QString& str);

	void getBlocks(const QString& str);

	gBlock*			operator[](size_t idx)	{return m_blocks[idx];}
	gBlock*			at(size_t idx)			{return m_blocks.at(idx);}
	const QString&	str()					{return m_str;}

	size_t size() {return m_blocks.size();}

public slots:

	void start() {}
	void stop() {}
	void pause() {}
	void resume() {}
	void reset() {}
	void execute_next() {}

	void save() {}
	void load() {}
	void load(const std::string& gfile);

protected:

	QString					m_str;
	std::vector<gBlock*>	m_blocks;
};

#endif