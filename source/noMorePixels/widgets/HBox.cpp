#include "HBox.h"

#include "../Settings.h"

using namespace noMoPi;

HBox::HBox(const ScaleSettings& scaleSettings) : WidgetContainerBase(scaleSettings)
{
	_rootWidget = Unigine::WidgetHBox::create();
	_rootWidget->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	_containterWidget = _rootWidget;

	if (Unigine::WidgetHBoxPtr hbox = Unigine::static_ptr_cast<Unigine::WidgetHBox>(_rootWidget))
	{
		hbox->setBackgroundTexture(Settings::get().getWhiteBackground());
	}
}