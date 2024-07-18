#include "pincButton.h"


pincButton::pincButton(const QString& str, QWidget* parent) : QPushButton(str, parent)
{
	setPalette(pincStyle::pincButtonPalette);

	setCheckable(true);
	setChecked(false);

	setMinimumSize(60,60);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}