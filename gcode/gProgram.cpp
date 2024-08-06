#include "gProgram.h"

gProgramToolBar::gProgramToolBar(QWidget* parent) : QToolBar(parent)
{
	QAction* action;

	action = new QAction("Run");
	addAction(action);
	connect(action, &QAction::triggered, this, &gProgramToolBar::run);
	action = new QAction("Hold");
	addAction(action);
	connect(action, &QAction::triggered, this, &gProgramToolBar::hold);
	action = new QAction("Reset");
	addAction(action);
	connect(action, &QAction::triggered, this, &gProgramToolBar::reset);
	action = new QAction("Clear");
	addAction(action);
	connect(action, &QAction::triggered, this, &gProgramToolBar::clear);

	QPalette palette = pincStyle::pincButtonPalette;
	palette.setColor(QPalette::Window, pincStyle::pincWindowPalette.color(QPalette::Window));

	setPalette(palette);
}


gProgram::gProgram(QWidget* parent) : QWidget(parent)
{
	setPalette(pincStyle::pincLineEditPalette);
	setBackgroundRole(QPalette::Window);

	m_block_layout = new QVBoxLayout();
	m_block_layout->setSpacing(0);
	m_block_layout->setContentsMargins(0,0,0,0);
	m_block_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

	setLayout(m_block_layout);
}

gProgram::gProgram(const QString& str, QWidget* parent) : gProgram(parent)
{
	m_str = str;
	makeBlocks(m_str);
}

gProgram::gProgram(const std::string& str, QWidget* parent) : gProgram(parent)
{
	m_str = std::move(str.c_str());
	makeBlocks(m_str);
}

void gProgram::makeBlocks(const QString& str)
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

void gProgram::addBlock(gBlock* block)
{
	m_blocks.push_back(block);
	m_block_layout->addWidget(block);
}