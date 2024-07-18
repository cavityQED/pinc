#ifndef PINC_EDIT_WINDOW_H
#define PINC_EDIT_WINDOW_H

#include <QGroupBox>
#include <QVBoxLayout>
#include <QList>
#include <QLineEdit>
#include <QLabel>
#include <QAbstractScrollArea>
#include <QScrollArea>
#include <QScrollBar>
#include <QSize>
#include <QPaintEvent>

#include "pincPanel.h"
#include "pincStyle.h"
#include "pincLabel.h"

class pincLabelScroll : public QScrollArea
{
	Q_OBJECT

public:

	pincLabelScroll(QWidget* parent = nullptr);
	~pincLabelScroll() {}

	void addLabel(QLabel* label);
	void addLine(const QList<QLabel*>& labels);

protected:


};



class pincEditWindow : public pincPanel
{
	Q_OBJECT

public:

	pincEditWindow(QWidget* parent = nullptr);
	~pincEditWindow() {}

protected:

	pincPanel*			m_text_panel;
	pincPanel*			m_line_panel;
	QLineEdit*			m_line_input;
	pincLabelScroll*	m_scroll;


};

#endif