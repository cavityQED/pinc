#include "gProgram.h"

gProgram::gProgram(QWidget* parent) : QWidget(parent)
{

}

gProgram::gProgram(const QString& str, QWidget* parent) : QWidget(parent)
{
	m_str = str;
	getBlocks(m_str);
}

gProgram::gProgram(const std::string& str, QWidget* parent) : QWidget(parent)
{
	m_str = std::move(str.c_str());
	getBlocks(m_str);
}

void gProgram::getBlocks(const QString& str)
{
	m_str = str;
	m_blocks.clear();

	//Regex to match any string between two parentheses
	QRegularExpression	commentRegex("\\([\\w\\W]*\\)");
	//Remove comments
	QString	no_comment_str(str);
	no_comment_str.replace(commentRegex, "");

	QRegularExpression					blockString("[GMLgml]\\d+[^GMLgml]*");
	QRegularExpressionMatch				match;
	QRegularExpressionMatchIterator		strings;
	gBlock*								blk;

	strings = blockString.globalMatch(no_comment_str);
	while(strings.hasNext())
	{
		blk = new gBlock(strings.next().captured());
		m_blocks.push_back(blk);
	}

	for(auto b : m_blocks)
		std::cout << b << '\n';

}