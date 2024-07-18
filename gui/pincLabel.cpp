#include "pincLabel.h"

pincLabel::pincLabel(const QString& str, QWidget* parent) : QLabel(str, parent)
{
	setPalette(pincStyle::pincLineEditPalette);

	QFont f(font());
	f.setWeight(QFont::Bold);
	setFont(f);

	setMouseTracking(true);
}

void pincLabel::enterEvent(QEvent* event)
{
	setBackgroundRole(QPalette::Highlight);
	setForegroundRole(QPalette::BrightText);
}

void pincLabel::leaveEvent(QEvent* event)
{
	setBackgroundRole(QPalette::Window);
	setForegroundRole(QPalette::Text);
}