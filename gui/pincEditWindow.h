#ifndef PINC_EDIT_WINDOW_H
#define PINC_EDIT_WINDOW_H

#include <iostream>

#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>

#include "pincPanel.h"
#include "pincStyle.h"
#include "pincLabel.h"

#include "gcode/gBlock.h"

class pincEditWindow : public pincPanel
{
	Q_OBJECT

public:

	pincEditWindow(QWidget* parent = nullptr);
	~pincEditWindow() {}

public:

	virtual bool eventFilter(QObject* obj, QEvent* evt);

public:

	void setCurLabel(pincLabel* label);
	void addLine(const QString& str);

protected:

	pincPanel*		m_text_panel;
	pincPanel*		m_line_panel;
	pincLabel*		m_cur_label;

	QLineEdit*		m_line_input;
	QScrollArea*	m_scroll;
	QVBoxLayout*	m_scroll_layout;
};

#endif