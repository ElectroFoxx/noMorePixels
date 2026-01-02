#include "WidgetBase.h"

#include "Interactive.h"

using namespace noMoPi;

void WidgetBase::resize(int32_t width, int32_t height)
{
	_widget->setWidth(width);
	_widget->setHeight(height);

	if (Interactive* interactive = dynamic_cast<Interactive*>(this))
		interactive->resize(width, height);
}

void noMoPi::WidgetBase::setIsEnabled(bool isEnabled)
{
	_widget->setEnabled(isEnabled);

	if (Interactive* interactive = dynamic_cast<Interactive*>(this))
		interactive->_setIsEnabled(isEnabled);
}