#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

template <typename T>
struct gvec
{
	std::vector<T>	points;

	gvec(size_t count = 3)
	{
		points.resize(count);
		std::memset(points.data(), 0, points.size() * sizeof(T));
	}

	size_t size() const {return points.size();}

	T length()
	{	
		T len = 0;

		for(auto it = points.begin(); it != points.end(); it++)
			len += *it * (*it);

		return std::sqrt(len);						
	}

	const T& operator[](size_t idx) const {return points[idx];}
	T& operator[](size_t idx) {return points[idx];}

	friend bool operator==(const gvec& a, const gvec& b)
	{
		if(a.size() != b.size())
			return false;

		for(int i = 0; i < a.size(); i++)
		{
			if(a[i] != b[i])
				return false;
		}

		return true;
	}
};

struct gBlock
{
	int						start;
	char					ltr;
	double					num;
	std::map<char, double>	arg;
	QString					str = "";

	friend std::ostream& operator<<(std::ostream& out, const gBlock& blk)
	{
		out << '\n' << blk.ltr << blk.num << " Arguments:\n";
		for(auto a : blk.arg)
			out << "\t[" << a.first << ", " << a.second << "]\n";

		return out;
	}
};

void getLines(const QString& file, QStringList& lines)
{
	lines.clear();

	auto ch = file.begin();

	QString* line = new QString();

	while(ch != file.end())
	{
		if(*ch == '\n')
		{
			line->append(*ch);
			lines.push_back(*line);
			line = new QString();
		}

		else
			line->append(*ch);

		ch++;
	}

	lines.push_back(*line);
}

void getArgs(std::shared_ptr<gBlock> blk)
{
	qDebug() << "\nGetting arguments from string: " << blk->str;

	int offset = 0;
	QRegularExpression ltrregex("[GMLgml]");
	auto ltrmatch = ltrregex.match(blk->str, offset);
	if(ltrmatch.hasMatch())
	{
		blk->ltr = ltrmatch.captured()[0].unicode();
		offset == ltrmatch.capturedEnd();
	}

	QRegularExpression numregex("[0-9]+\\.?[0-9]?");
	auto nummatch = numregex.match(blk->str, offset);
	if(nummatch.hasMatch())
	{
		blk->num = nummatch.capturedTexts()[0].toDouble();
		offset = nummatch.capturedEnd();
	}

	QRegularExpression regex("[A-Za-z]-?[0-9]*\\.?[0-9]*");	
	auto it = regex.globalMatch(blk->str, offset);
	if(!it.hasNext())
	{
		qDebug() << "\tNo Arg Strings Found";
		return;
	}
	while(it.hasNext())
	{
		QString str = it.next().captured(0);
		qDebug() << "\tFound Arg String: " << str;
		char var = str[0].unicode();
		str.remove(0,1);
		double val = str.toDouble();
		blk->arg.insert(std::make_pair(var, val));
	}
}

void getBlocks(const QString& str, std::vector<std::shared_ptr<gBlock>>& blocks)
{
	blocks.clear();

	QRegularExpressionMatch	gMatch;
	QRegularExpression		gRegex("[GMLgml]");
	auto					iter = gRegex.globalMatch(str);
	while(iter.hasNext())
	{
		gMatch = iter.next();
		blocks.push_back(std::make_shared<gBlock>());
		blocks.back()->start = gMatch.capturedStart();
	}

	int end = str.size();
	auto blk = blocks.rbegin();
	while(blk != blocks.rend())
	{
		int length = end - (*blk)->start;
		(*blk)->str = QStringRef(&str, (*blk)->start, length).toString();
		getArgs(*blk);
		end = (*blk)->start;
		blk++;
	}
	
	for(auto b : blocks)
		std::cout << *b;
}

int main(int argc, char *argv[])
{
	gvec<double> vec(3);
	vec.points[0] = 3.0f;
	vec.points[1] = 4.0f;
	vec.points[2] = 5.0f;

	gvec<double> vec2(4);

	gvec<double> vec3(3);
	vec3.points[0] = 3.0f;
	vec3.points[1] = 4.0f;
	vec3.points[2] = 5.0f;

	std::cout << "Length:\t" << vec.length() << '\n';
	std::cout << "Vec == Vec2:\t" << (vec==vec2) << '\n';
	std::cout << "Vec == Vec3:\t" << (vec==vec3) << '\n';

	vec.points[0] *= 1.2398467098234f;
	vec3.points[0] *= 1.2398467098234f;
	std::cout << "Vec == Vec3:\t" << (vec==vec3) << '\n';

	std::fstream file;
	file.open("gcode.nc");

	std::stringstream buf;
	buf << file.rdbuf();

	QString str;
	str.append(buf.str().c_str());

	QStringList lines;
	getLines(str, lines);

	std::cout << "Got Lines:\n";
	for(int i = 0; i < lines.size(); i++)
		qDebug() << '\t' << lines.at(i);

	std::vector<std::shared_ptr<gBlock>> blocks;
	getBlocks(str, blocks);

	return 0;
}