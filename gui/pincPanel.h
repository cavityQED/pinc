#ifndef PINC_PANEL_H
#define PINC_PANEL_H

#include <QGroupBox>
#include <QStyle>
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOption>
#include <QStyleOptionGroupBox>

#include "pincColors.h"
#include "pincStyle.h"

class pincPanel : public QGroupBox
{
	Q_OBJECT

public:

	pincPanel(const QString& title, QWidget* parent = nullptr);
	~pincPanel() {}

public:

	void setFrameState(QStyle::State s) {m_frameState |= s;}
	void clrFrameState(QStyle::State s) {m_frameState &= ~s;}

	QStyle::State frameState() const {return m_frameState;}


protected:

	virtual void paintEvent(QPaintEvent* event) override;

protected:

	QStyle::State	m_frameState;

};

#endif