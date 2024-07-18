#ifndef PINC_BUTTON_H
#define PINC_BUTTON_H

#include <QPushButton>

#include "pincColors.h"
#include "pincStyle.h"

class pincButton : public QPushButton
{
	Q_OBJECT

public:

	pincButton(const QString& str, QWidget* parent = nullptr);
	~pincButton() {}

protected:

};

#endif