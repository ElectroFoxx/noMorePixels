#include "VBox.h"

#include "../Settings.h"

using namespace noMoPi;

VBox::VBox(const ScaleSettings& scaleSettings) : WidgetContainerBase(scaleSettings)
{
	_widget = Unigine::WidgetVBox::create();
	_containterWidget = _widget;

	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
	{
		vbox->setBackgroundTexture(Settings::get().getWhiteBackground());
		vbox->setStencil(true);
	}

}