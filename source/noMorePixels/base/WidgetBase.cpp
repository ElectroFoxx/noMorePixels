#include "WidgetBase.h"

#include "Interactive.h"

using namespace noMoPi;

void WidgetBase::resize(int32_t width, int32_t height)
{
	_size = Unigine::Math::ivec2(width, height);
	
	_rootWidget->setWidth(width);
	_rootWidget->setHeight(height);

	if (Interactive* interactive = dynamic_cast<Interactive*>(this))
		interactive->resize(width, height);
}

void WidgetBase::setIsEnabled(bool isEnabled)
{
	_isEnabled = isEnabled;
	
	_rootWidget->setEnabled(isEnabled);

	if (Interactive* interactive = dynamic_cast<Interactive*>(this))
		interactive->_setIsEnabled(isEnabled);
}

void WidgetBase::updateLayout()
{
	resize(_size.x, _size.y);
}

void WidgetBase::_setGui(const Unigine::GuiPtr& gui)
{
	_gui = gui;
	_rootWidget->setGui(_gui);
}

WidgetBase::~WidgetBase()
{
	_rootWidget.deleteLater();
}

