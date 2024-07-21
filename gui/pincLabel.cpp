#include "pincLabel.h"

pincLabel::pincLabel(const QString& str, QWidget* parent) : QLabel(str, parent)
{
	setPalette(pincStyle::pincLineEditPalette);

	QFont f(font());
	f.setWeight(QFont::Bold);
	setFont(f);

	setMouseTracking(true);

	m_selected = false;
}

void pincLabel::enterEvent(QEvent* event)
{
	setBackgroundRole(QPalette::Highlight);
	setForegroundRole(QPalette::BrightText);
}

void pincLabel::leaveEvent(QEvent* event)
{
	setBackgroundRole(m_selected? QPalette::Highlight : QPalette::Window);
	setForegroundRole(m_selected? QPalette::BrightText : QPalette::Text);
}

void pincLabel::mouseReleaseEvent(QMouseEvent* event)
{
	if(frameRect().contains(event->localPos().toPoint()))
	{
		m_selected = !m_selected;
		emit selected(m_selected? this : nullptr);
	}
}