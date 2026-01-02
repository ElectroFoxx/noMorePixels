#pragma once

#include <memory>

#include <UnigineWidgets.h>

#include "base/WidgetBase.h"

namespace noMoPi
{
	class UI
	{
	public:
		UI() {}
		UI(const Unigine::GuiPtr& gui) : _gui(gui) {}
		void setRootWidget(const std::shared_ptr<WidgetBase>& widget);
		void updateLayout();
		void setDictionary(const char* dictionary);
		void setLanguage(const char* language);
		void translate();
		void tick();
		void addChild(const std::shared_ptr<WidgetBase>& widget);
		void setSize(const Unigine::Math::ivec2& size);
		void clear();
	private:
		Unigine::GuiPtr _gui;
		std::shared_ptr<WidgetBase> _rootWidget;
		const char* _currentDictionary = "";
	};
}
