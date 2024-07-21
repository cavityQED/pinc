#include "gBlock.h"

gBlock::gBlock(QWidget* parent) : QWidget(parent)
{
	m_labels.clear();
	m_layout = new QHBoxLayout();
	m_layout->setSpacing(2);
	setLayout(m_layout);
	setContentsMargins(0,0,0,0);
}

gBlock::gBlock(const QString& str, QWidget* parent) : gBlock(parent)
{
	setArguments(str);
}

void gBlock::setArguments(const QString& str)
{
	static QRegularExpression letter("[GMLgml]");
	static QRegularExpression number("[0-9]+\\.?[0-9]?");
	static QRegularExpression argstr("[A-Za-z]\\s*\\-?[0-9]*\\.?[0-9]*");

	int offset = 0;
	
	auto m = letter.match(str);
	if(m.hasMatch())
	{
		m_ltr = m.captured()[0].toUpper();
		offset = m.capturedEnd();

		m = number.match(str, offset);
		if(m.hasMatch())
		{
			m_num = m.capturedTexts()[0].toDouble();
			offset = m.capturedEnd();
		}

		if(m_labels.contains(m_ltr))
			m_labels[m_ltr]->setText(m_ltr + QString::number(m_num));
		else
		{
			pincLabel* label = new pincLabel(m_ltr + QString::number(m_num));
			connect(label, &pincLabel::textChange, this, &gBlock::setArguments);
			connect(label, &pincLabel::selected, this, &gBlock::selected);
			connect(label, &pincLabel::remove, [this, label]() {clrArgument(m_labels.key(label));});
			m_labels.insert(m_ltr, label);
			m_layout->addWidget(label);
		}
	}

	auto iter = argstr.globalMatch(str, offset);
	while(iter.hasNext())
		addArgument(iter.next().captured());
}

void gBlock::addArgument(const QString& str)
{
	QChar	var;
	double	val;

	var	= str[0].toUpper();
	val	= str.rightRef(str.size()-1).toDouble();

	if(m_args.contains(var))
	{
		m_args[var] = val;
		m_labels[var]->setText(var + QString::number(val));
	}
	else
	{
		pincLabel* label = new pincLabel(var + QString::number(val));
		connect(label, &pincLabel::textChange, this, &gBlock::setArguments);
		connect(label, &pincLabel::selected, this, &gBlock::selected);
		connect(label, &pincLabel::remove, [this, label]() {clrArgument(m_labels.key(label));});
		m_args.insert(var, val);
		m_labels.insert(var, label);
		m_layout->setStretch(m_layout->count()-1, 0);
		m_layout->addWidget(label);
		m_layout->setStretchFactor(label, 1);
	}
}

void gBlock::clrArgument(QChar arg)
{
	if(m_args.contains(arg))
	{
		m_args.remove(arg);
		pincLabel* label = m_labels[arg];
		label->disconnect();
		if(m_layout->indexOf(label) == (m_layout->count()-1))
			m_layout->setStretch(m_layout->count()-2, 1);
		m_layout->removeWidget(label);
		delete label;
	}
}

std::ostream& operator<<(std::ostream& out, const gBlock* block)
{
	out << "\nBlock " << block->ltr().cell() << block->num() << ":\n";
	out << "\tArguments:";

	if(block->args().isEmpty())
		out << "\tNone\n";
	else
	{
		out << '\n';
		for(auto arg = block->args().begin(); arg != block->args().end(); arg++)
			out << "\t\t[" << arg.key().cell() << ", " << arg.value() << "]\n"; 
	}

	return out;
}