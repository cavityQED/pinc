#ifndef PINC_COLORS_H
#define PINC_COLORS_H

#include <QApplication>
#include <QPalette>
#include <QColor>
#include <QDebug>

#define WHITE			0xFFFFFFFF
#define BLACK			0xFF000000

#define BLUE_HIGHLIGHT	0xFFF4FBFF
#define BLUE_LIGHT		0xFFD0ECFF
#define BLUE_MIDLIGHT	0xFF72C4FF
#define BLUE			0xFF47B1FF
#define BLUE_MID		0xFF3F9BE0
#define BLUE_SHADOW		0xFF3685C0
#define BLUE_DARK		0xFF245980

#define GREY_HIGHLIGHT	0xFFF6F6F6
#define GREY_LIGHT		0xFF808080
#define GREY_MIDLIGHT	0xFF707070
#define GREY			0xFF606060
#define GREY_MID		0xFF505050
#define GREY_SHADOW		0xFF404040
#define GREY_DARK		0xFF181818


static void printPalette(const QPalette& pal)
{
	qDebug() << "Window:\t\t" << pal.color(QPalette::Window);
	qDebug() << "Highlight:\t" << pal.color(QPalette::Highlight);
	qDebug() << "Base:\t\t" << pal.color(QPalette::Base);
	qDebug() << "Text:\t\t" << pal.color(QPalette::Text);
}

static void getPincGroupBoxPalette(QPalette& pal)
{
	pal.setColor(QPalette::Window,			QColor(GREY));
	pal.setColor(QPalette::Highlight,		QColor(GREY_HIGHLIGHT));
	pal.setColor(QPalette::Light,			QColor(GREY_LIGHT));
	pal.setColor(QPalette::Midlight,		QColor(GREY_MIDLIGHT));
	pal.setColor(QPalette::Base,			QColor(GREY));
	pal.setColor(QPalette::Mid,				QColor(GREY_MID));
	pal.setColor(QPalette::Shadow,			QColor(GREY_SHADOW));
	pal.setColor(QPalette::Dark,			QColor(GREY_DARK));

	pal.setColor(QPalette::Text,			QColor(WHITE));
	pal.setColor(QPalette::WindowText,		QColor(WHITE));
	pal.setColor(QPalette::HighlightedText,	QColor(BLACK));
	pal.setColor(QPalette::BrightText,		QColor(BLACK));
}

static void getPincWindowPalette(QPalette& pal)
{
	getPincGroupBoxPalette(pal);

	pal.setColor(QPalette::Window,		QColor(BLUE));
	pal.setColor(QPalette::Base,		QColor(BLUE));
	pal.setColor(QPalette::Text,		QColor(BLACK));
	pal.setColor(QPalette::WindowText,	QColor(BLACK));
}

static void getPincLineEditPalette(QPalette& pal)
{
	pal.setColor(QPalette::Window,			QColor(GREY_SHADOW).darker(120));
	pal.setColor(QPalette::Highlight,		QColor(GREY_HIGHLIGHT));
	pal.setColor(QPalette::Light,			QColor(GREY_LIGHT));
	pal.setColor(QPalette::Midlight,		QColor(GREY_MIDLIGHT));
	pal.setColor(QPalette::Base,			QColor(GREY_SHADOW));
	pal.setColor(QPalette::Mid,				QColor(GREY_MID));
	pal.setColor(QPalette::Shadow,			QColor(GREY_SHADOW));
	pal.setColor(QPalette::Dark,			QColor(GREY_DARK));

	pal.setColor(QPalette::Text,			QColor(WHITE));
	pal.setColor(QPalette::WindowText,		QColor(WHITE));
	pal.setColor(QPalette::HighlightedText,	QColor(BLACK));
	pal.setColor(QPalette::BrightText,		QColor(BLACK));
}

static void getPincButtonPalette(QPalette& pal)
{
	pal.setColor(QPalette::Window,			QColor(GREY));
	pal.setColor(QPalette::Highlight,		QColor(BLUE_HIGHLIGHT));
	pal.setColor(QPalette::Light,			QColor(BLUE_LIGHT));
	pal.setColor(QPalette::Midlight,		QColor(BLUE_MIDLIGHT));
	pal.setColor(QPalette::Base,			QColor(BLUE));
	pal.setColor(QPalette::Button,			QColor(BLUE));
	pal.setColor(QPalette::Mid,				QColor(BLUE_MID));
	pal.setColor(QPalette::Shadow,			QColor(BLUE_SHADOW));
	pal.setColor(QPalette::Dark,			QColor(BLUE_DARK));

	pal.setColor(QPalette::Text,			QColor(BLACK));
	pal.setColor(QPalette::WindowText,		QColor(BLACK));
	pal.setColor(QPalette::ButtonText,		QColor(BLACK));
	pal.setColor(QPalette::HighlightedText,	QColor(BLACK));
	pal.setColor(QPalette::BrightText,		QColor(BLACK));
}

#endif