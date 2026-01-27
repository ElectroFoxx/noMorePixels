#include "UI.h"

#include <UnigineEngine.h>

#include "Settings.h"
#include "base/WidgetContainerBase.h"

using namespace noMoPi;

void UI::setRootWidget(const std::shared_ptr<WidgetBase>& widget)
{
	_rootWidget = widget;

	if (_gui)
	{
		_clearGuiChildren();

		_rootWidget->_setGui(_gui);
		_gui->addChild(*_rootWidget);
	}
}

void UI::updateLayout()
{
	if (!_rootWidget || !_gui)
		return;

	_rootWidget->_setGui(_gui);

	translate();

	Unigine::Math::ivec2 guiSize = _gui->getSize();
	_rootWidget->resize(guiSize.x, guiSize.y);
}

void noMoPi::UI::setDictionary(const Unigine::String& dictionary)
{
	_currentDictionary = Settings::get().getLocalizationPath(dictionary);
}

void noMoPi::UI::setLanguage(const Unigine::String& language)
{
	_currentLanguage = language;
}

void UI::translate()
{
	if (!_rootWidget)
		return;

	if (!_gui)
		return;

	if (_currentDictionary.size() == 0 || _currentLanguage.size() == 0)
		return;

	_gui->clearDictionaries();
	_gui->addDictionary(_currentDictionary, _currentLanguage);
	
	_rootWidget->translate();
}

void noMoPi::UI::tick()
{
	if (!_rootWidget)
		return;
	
	_rootWidget->tick(Unigine::Engine::get()->getIFps());
}

void UI::addChild(const std::shared_ptr<WidgetBase>& widget)
{
	if (!_rootWidget)
		return;
	
	_rootWidget->addChild(widget);
}

void noMoPi::UI::setSize(const Unigine::Math::ivec2& size)
{
	if (_gui)
		_gui->setSize(size);

	//if (_rootWidget)
		//_rootWidget->resize(size.x, size.y);
}

void noMoPi::UI::clear(bool deleteRoot = false)
{
	if (!_rootWidget)
		return;
	
	if (WidgetContainerBase* container = dynamic_cast<WidgetContainerBase*>(_rootWidget.get()))
		container->clear();

	_clearGuiChildren();

	if (deleteRoot)
		_rootWidget = nullptr;
}

void noMoPi::UI::_clearGuiChildren()
{
	if (_gui)
	{
		while (_gui->getNumChildren() > 0)
		{
			Unigine::WidgetPtr child = _gui->getChild(0);
			_gui->removeChild(child);
		}
	}
}
