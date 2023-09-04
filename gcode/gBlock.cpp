#include "gBlock.h"

gBlock::gBlock(QObject* parent) : QObject(parent)
{

}

gBlock::gBlock(const QString& str, QObject* parent) : QObject(parent), m_str(str)
{
	setArguments();
}

void gBlock::setArguments()
{
	static QRegularExpression letter("[GMLgml]");
	static QRegularExpression number("[0-9]+\\.?[0-9]?");
	static QRegularExpression argstr("[A-Za-z]\\s*\\-?[0-9]*\\.?[0-9]*");

	int offset = 0;
	
	auto m = letter.match(m_str);
	if(m.hasMatch())
	{
		m_ltr = m.captured()[0].toUpper().unicode();
		offset = m.capturedEnd();
	}

	m = number.match(m_str, offset);
	if(m.hasMatch())
	{
		m_num = m.capturedTexts()[0].toDouble();
		offset = m.capturedEnd();
	}

	auto iter = argstr.globalMatch(m_str, offset);
	while(iter.hasNext())
	{
		QString str = iter.next().captured();
		char var = str[0].toUpper().unicode();
		str.remove(0,1);
		double val = str.toDouble();
		m_args.insert(std::make_pair(var, val));
	}
}

std::ostream& operator<<(std::ostream& out, const gBlock* block)
{
	out << "\nBlock " << block->ltr() << block->num() << ":\n";
	out << "\tString:\t" << block->str().toStdString() << '\n';
	out << "\tArguments:";

	if(block->args().empty())
		out << "\tNone\n";
	else
	{
		out << '\n';
		for(auto arg : block->args())
			out << "\t\t[" << arg.first << ", " << arg.second << "]\n"; 
	}

	return out;
}