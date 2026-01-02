#include "Interactive.h"

using namespace noMoPi;

void noMoPi::Interactive::setGui(Unigine::GuiPtr gui)
{
	_interactiveLayer->setGui(gui);
}

Interactive::Interactive()
{
	_interactiveLayer = Unigine::WidgetButton::create("");
	_interactiveLayer->setBackground(false);
}

void Interactive::resize(int32_t width, int32_t height)
{
	_interactiveLayer->setWidth(width);
	_interactiveLayer->setHeight(height);
}

void noMoPi::Interactive::attach(const std::shared_ptr<WidgetBase>& widget)
{
	widget->getWidget()->addChild(_interactiveLayer, Unigine::Gui::ALIGN_OVERLAP | Unigine::Gui::ALIGN_FIXED);
}