#include "VBox.h"

#include "../Settings.h"

using namespace noMoPi;

VBox::VBox(const ScaleSettings& scaleSettings) : WidgetContainerBase(scaleSettings)
{
	_rootWidget = Unigine::WidgetVBox::create();
	_rootWidget->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	_containterWidget = _rootWidget;

	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_rootWidget))
	{
		vbox->setBackgroundTexture(Settings::get().getWhiteBackground());
	}

}