#include "pincStyle.h"

QPalette	pincStyle::pincButtonPalette = QApplication::palette();
QPalette	pincStyle::pincLineEditPalette = QApplication::palette();
QPalette	pincStyle::pincWindowPalette = QApplication::palette();
QPalette	pincStyle::pincGroupBoxPalette = QApplication::palette();

pincStyle::pincStyle() : QProxyStyle()
{
	getPincGroupBoxPalette(pincGroupBoxPalette);
	getPincWindowPalette(pincWindowPalette);
	getPincButtonPalette(pincButtonPalette);
	getPincLineEditPalette(pincLineEditPalette);
}


QRect pincStyle::subControlRect(	ComplexControl control,
									const QStyleOptionComplex* option,
									SubControl subcontrol,
									const QWidget* widget) const
{
	switch(control)
	{
		case CC_GroupBox:
		{
			if(const QStyleOptionGroupBox* box = qstyleoption_cast<const QStyleOptionGroupBox*>(option))
			{
				switch(subcontrol)
				{
					case SC_GroupBoxFrame:
						return box->rect;

					case SC_GroupBoxContents:
					{
						QRect ret = box->rect;

						int height = box->text.isEmpty()? 0 : box->fontMetrics.height();
						int marg = box->lineWidth;

						return ret.adjusted(marg, marg + height, -marg, -marg);

						break;
					}

					case SC_GroupBoxCheckBox:
						return QRect(0,0,0,0);

					default:
						return QProxyStyle::subControlRect(control, option, subcontrol, widget);
				}
			}
		}

		default:
			return QProxyStyle::subControlRect(control, option, subcontrol, widget);
	}
}

int pincStyle::pixelMetric(	PixelMetric metric,
							const QStyleOption* option,
							const QWidget* widget) const
{
	switch(metric)
	{
		default:
			return QProxyStyle::pixelMetric(metric, option, widget);
	}
}

void pincStyle::drawPrimitive(	PrimitiveElement element,
								const QStyleOption* option,
								QPainter* painter,
								const QWidget* widget) const
{
	switch(element)
	{
		case PE_PanelButtonCommand:
		{
			QBrush fillbrush (option->palette.brush(QPalette::Button));

			if(option->state & State_On)
				fillbrush.setColor(option->palette.color(QPalette::Highlight));

			qDrawShadePanel(	painter,
								option->rect,
								option->palette,
								option->state & State_Sunken,
								5,
								&fillbrush);
			break;
		}

		case PE_PanelLineEdit:
		{
			QStyleOptionFrame	frame;
			frame.QStyleOption::operator=(*option);
			frame.lineWidth		= 2;
			frame.state			|= QStyle::State_Sunken;
			frame.frameShape	= QFrame::Panel;
			// frame.palette		= pincLineEditPalette;

			QFont				linefont;
			linefont.setWeight(QFont::Bold);
			linefont.setPixelSize(option->rect.height() - 12);
			painter->setFont(linefont);

			painter->fillRect(frame.rect, frame.palette.brush(QPalette::Window));
			proxy()->drawControl(QStyle::CE_ShapedFrame, &frame, painter, widget);

			break;
		}

		default:
			QProxyStyle::drawPrimitive(element, option, painter, widget);
			break;
	}
}

void pincStyle::drawControl(	ControlElement element,
								const QStyleOption* option,
								QPainter* painter,
								const QWidget* widget) const
{
	switch(element)
	{
		case CE_FocusFrame:
			break;

		case CE_PushButtonLabel:
		{
			QFont labelfont(painter->font());
			labelfont.setWeight(QFont::Bold);
			labelfont.setPixelSize(0.3*option->rect.height());
			painter->setFont(labelfont);
			QProxyStyle::drawControl(element, option, painter, widget);
			break;
		}

		case CE_ShapedFrame:
		{
			if(const pincLabel* label = qobject_cast<const pincLabel*>(widget))
				painter->fillRect(option->rect, option->palette.color(widget->backgroundRole()));					
			else
				QProxyStyle::drawControl(element, option, painter, widget);

			break;
		}

		default:
			QProxyStyle::drawControl(element, option, painter, widget);
			break;
	}
}

void pincStyle::drawComplexControl(	ComplexControl control,
									const QStyleOptionComplex* option,
									QPainter* painter,
									const QWidget* widget) const
{
	switch(control)
	{
		case CC_GroupBox:
			if(const QStyleOptionGroupBox* box = qstyleoption_cast<const QStyleOptionGroupBox*>(option))
			{
				QStyleOptionFrame frame;
				frame.QStyleOption::operator=(*box);
				frame.frameShape	= QFrame::Panel;
				frame.features		= QStyleOptionFrame::Rounded;
				frame.lineWidth		= 5;

				// draw background
				painter->fillRect(box->rect.adjusted(5,5,-5,-5), frame.palette.color(QPalette::Window));

				// only draw frame if sunken panel
				if(box->state & QStyle::State_Sunken)
					proxy()->drawControl(QStyle::CE_ShapedFrame, &frame, painter, widget);

				// draw title
				if(!box->text.isEmpty())
				{
					QRect textrect = proxy()->subControlRect(CC_GroupBox, box, SC_GroupBoxLabel, widget);
					textrect.translate(2*frame.lineWidth, frame.lineWidth);
					
					painter->setPen(frame.palette.color(QPalette::WindowText));
					
					int alignment = int(Qt::AlignLeft);
					alignment |= Qt::TextHideMnemonic;

					proxy()->drawItemText(	painter,
											textrect,
											alignment,
											box->palette,
											box->state & State_Enabled,
											box->text,
											QPalette::NoRole);
				}

			}
			break;

		default:
			QProxyStyle::drawComplexControl(control, option, painter, widget);
			break;
	}
}