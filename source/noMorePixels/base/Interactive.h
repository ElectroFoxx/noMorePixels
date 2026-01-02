#pragma once

#include <memory>

#include <UnigineWidgets.h>

#include "WidgetBase.h"

namespace noMoPi
{
	class Interactive
	{
	public:
		void setGui(Unigine::GuiPtr gui);
		void resize(int32_t width, int32_t height);
		void attach(const std::shared_ptr<WidgetBase>& widget);

		Unigine::Event<const Unigine::WidgetPtr&>& getEventEnter() { return _interactiveLayer->getEventEnter(); }
		Unigine::Event<const Unigine::WidgetPtr&>& getEventLeave() { return _interactiveLayer->getEventLeave(); }
		Unigine::Event<const Unigine::WidgetPtr&, int>& getEventClicked() { return _interactiveLayer->getEventClicked(); }
		void _setIsEnabled(bool isEnabled) { _interactiveLayer->setEnabled(isEnabled); }
	protected:
		Interactive();

		Unigine::WidgetButtonPtr _interactiveLayer;

	};
}
