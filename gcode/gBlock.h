#ifndef GBLOCK_H
#define GBLOCK_H

#include <QWidget>
#include <QVector>
#include <QMap>
#include <QString>
#include <QChar>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QHBoxLayout>

#include "gui/pincLabel.h"

class gBlock : public QWidget
{
	Q_OBJECT

public:

	gBlock(QWidget* parent = nullptr);
	gBlock(const QString& str, QWidget* parent = nullptr);
	~gBlock() {}

	void setArguments(const QString& str);
	void addArgument(const QString& str);
	void clrArgument(QChar arg);

	QChar&				ltr()			{return m_ltr;}
	const QChar&		ltr()	const	{return m_ltr;}

	double&				num()			{return m_num;}
	const double&		num()	const	{return m_num;}

	const QMap<QChar,double>&		args()		const	{return m_args;}
	const QMap<QChar, pincLabel*>&	labels()	const	{return m_labels;}

signals:

	void selected(pincLabel* label);

protected:

	QChar						m_ltr;
	double						m_num;
	QMap<QChar, double>			m_args;
	QMap<QChar, pincLabel*>		m_labels;

	QHBoxLayout*				m_layout;
};

std::ostream& operator<<(std::ostream& out, const gBlock* block);

#endif