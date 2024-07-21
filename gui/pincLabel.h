#ifndef PINC_LABEL_H
#define PINC_LABEL_H

#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

#include "pincColors.h"
#include "pincStyle.h"

class pincLabel : public QLabel
{
	Q_OBJECT

public:

	pincLabel(const QString& str, QWidget* parent = nullptr);
	~pincLabel() {}

public:

	void setSelected(bool sel) {m_selected = sel;}

signals:

	void selected(pincLabel* label);
	void textChange(const QString& text);
	void remove();

protected:

	virtual void enterEvent(QEvent* event) override;
	virtual void leaveEvent(QEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;

protected:

	bool	m_selected;

};

#endif