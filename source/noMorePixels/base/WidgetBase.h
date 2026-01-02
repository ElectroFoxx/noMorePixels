#pragma once

#include <memory>

#include <UnigineWidgets.h>

#include "../types/ScaleSettings.h"

namespace noMoPi
{
	class WidgetBase
	{
	public:
		WidgetBase(const ScaleSettings& scaleSettings) : _scaleSettings(scaleSettings) {}
		void setGui(const Unigine::GuiPtr& gui) { _widget->setGui(gui); }
		void setScaleSettings(const ScaleSettings& scaleSettings) { _scaleSettings = scaleSettings; }
		virtual void resize(int32_t width, int32_t height);
		virtual operator const Unigine::WidgetPtr& () const { return _widget; }
		const ScaleSettings& getScaleSettings() const { return _scaleSettings; }
		Unigine::WidgetPtr getWidget() { return _widget; }
		virtual void translate() {}
		virtual void tick(float deltaTime) {}
		virtual void addChild(const std::shared_ptr<WidgetBase>& widget) {}
		void setIsEnabled(bool isEnabled);
	protected:
		Unigine::WidgetPtr _widget;

		ScaleSettings _scaleSettings;
	};
}
