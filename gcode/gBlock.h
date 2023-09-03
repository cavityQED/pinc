#ifndef GBLOCK_H
#define GBLOCK_H

#include <map>
#include <memory>
#include <vector>

#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

#include "common/motor_common.h"

class gBlock : public QObject
{
	Q_OBJECT

public:

	gBlock(QObject* parent = nullptr);
	gBlock(const QString& str, QObject* parent = nullptr);
	~gBlock() {}

	void setArguments();

	int&				start()			{return m_start;}
	const int&			start()	const	{return m_start;}

	char&				ltr()			{return m_ltr;}
	const char&			ltr()	const	{return m_ltr;}

	double&				num()			{return m_num;}
	const double&		num()	const	{return m_num;}

	QString&			str()			{return m_str;}
	const QString&		str()	const	{return m_str;}

	const std::map<char, double>&	args()	const {return m_args;}

	static void makeBlocks(QString& str, std::vector<gBlock*>& blocks);

protected:

	int							m_start;
	char						m_ltr;
	double						m_num;
	QString						m_str;
	std::map<char, double>		m_args;
	move_msg_t					m_msg;
};

std::ostream& operator<<(std::ostream& out, const gBlock* block);

#endif