#ifndef G_PROGRAM_H
#define G_PROGRAM_H

#include <iostream>

#include <QWidget>
#include <QStringRef>
#include <QToolBar>
#include <QVector>

#include "gBlock.h"

class gProgramToolBar : public QToolBar
{
	Q_OBJECT

public:

	gProgramToolBar(QWidget* parent = nullptr);
	~gProgramToolBar() {}

signals:

	void run();
	void hold();
	void reset();
	void clear();

protected:


};


class gProgram : public QWidget
{
	Q_OBJECT

public:

	gProgram(QWidget* parent = nullptr);
	gProgram(const QString& str, QWidget* parent = nullptr);
	gProgram(const std::string& str, QWidget* parent = nullptr);
	~gProgram() {}

	void makeBlocks(const QString& str);
	void addBlock(gBlock* block);

	const QString&				str()		const	{return m_str;}
	const QVector<gBlock*>&		blocks()	const	{return m_blocks;}

	size_t size() {return m_blocks.size();}

protected:

	QString				m_str;
	QVector<gBlock*>	m_blocks;
	QVBoxLayout*		m_block_layout;
};

#endif