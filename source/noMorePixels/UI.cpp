#include "UI.h"

#include <UnigineEngine.h>

#include "Settings.h"
#include "base/WidgetContainerBase.h"

using namespace noMoPi;

void UI::setRootWidget(const std::shared_ptr<WidgetBase>& widget)
{
	_rootWidget = widget;
	_rootWidget->setGui(_gui);
	_gui->addChild(*_rootWidget);
}

void UI::updateLayout()
{
	Unigine::Math::ivec2 guiSize = _gui->getSize();
	_rootWidget->resize(guiSize.x, guiSize.y);
}

void noMoPi::UI::setDictionary(const char* dictionary)
{
	_currentDictionary = Settings::get().getLocalizationPath(dictionary);
}

void noMoPi::UI::setLanguage(const char* language)
{
	_gui->clearDictionaries();
	_gui->addDictionary(_currentDictionary, language);
}

void UI::translate()
{
	_rootWidget->translate();
}

void noMoPi::UI::tick()
{
	_rootWidget->tick(Unigine::Engine::get()->getIFps());
}

void UI::addChild(const std::shared_ptr<WidgetBase>& widget)
{
	_rootWidget->addChild(widget);
}

void noMoPi::UI::setSize(const Unigine::Math::ivec2& size)
{
	_gui->setSize(size);
	_rootWidget->resize(size.x, size.y);
}

void noMoPi::UI::clear()
{
	if (WidgetContainerBase* container = dynamic_cast<WidgetContainerBase*>(_rootWidget.get()))
		container->clear();



	//_rootWidget = nullptr;
}