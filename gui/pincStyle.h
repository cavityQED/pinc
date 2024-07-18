#ifndef PINC_STYLE_H
#define PINC_STYLE_H

#include <QStyle>
#include <QProxyStyle>
#include <QStyleOptionGroupBox>
#include <QStyleOptionFrame>
#include <QPainter>

#include <qdrawutil.h>

#include "pincColors.h"
#include "pincPanel.h"
#include "pincLabel.h"

class pincStyle : public QProxyStyle
{
	Q_OBJECT

public:

	pincStyle();
	~pincStyle() {}

public:

	virtual QRect	subControlRect(			ComplexControl control,
											const QStyleOptionComplex* option,
											SubControl subcontrol,
											const QWidget* widget = nullptr) const override;

	virtual int		pixelMetric(			PixelMetric metric,
											const QStyleOption* option,
											const QWidget* widget = nullptr) const override;

	virtual void	drawPrimitive(			PrimitiveElement element,
											const QStyleOption* option,
											QPainter* painter,
											const QWidget* widget = nullptr) const override;

	virtual void	drawControl(			ControlElement element,
											const QStyleOption* option,
											QPainter* painter,
											const QWidget* widget = nullptr) const override;

	virtual void	drawComplexControl(		ComplexControl control,
											const QStyleOptionComplex* option,
											QPainter* painter,
											const QWidget* widget = nullptr) const override;

public:

	static QPalette pincGroupBoxPalette;
	static QPalette pincWindowPalette;
	static QPalette pincButtonPalette;
	static QPalette pincLineEditPalette;
};

#endif