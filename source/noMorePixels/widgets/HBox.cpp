#include "HBox.h"

using namespace noMoPi;

HBox::HBox(const ScaleSettings& scaleSettings) : WidgetContainerBase(scaleSettings)
{
	_widget = Unigine::WidgetHBox::create();
	_containterWidget = _widget;

	if (Unigine::WidgetHBoxPtr hbox = Unigine::static_ptr_cast<Unigine::WidgetHBox>(_widget))
	{
		hbox->setBackgroundTexture(".noMorePixels/textures/white.png");
	}
}