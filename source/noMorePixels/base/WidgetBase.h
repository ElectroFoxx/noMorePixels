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

		void setScaleSettings(const ScaleSettings& scaleSettings) { _scaleSettings = scaleSettings; }
		const ScaleSettings& getScaleSettings() const { return _scaleSettings; }

		int32_t getWidth() const { return _size.x; }
		int32_t getHeight() const { return _size.y; }
		const Unigine::Math::ivec2& getSize() const { return _size; }
		bool getIsEnabled() const { return _isEnabled; }
		
		// Allow implicit conversion to Unigine::WidgetPtr
		virtual operator const Unigine::WidgetPtr& () const { return _rootWidget; }
		
		const Unigine::WidgetPtr& getWidget() { return _rootWidget; }
		
		// Virtual translate method to be overridden in derived classes
		virtual void translate() {}
		// Virtual tick method to be overridden in derived classes
		virtual void tick(float deltaTime) {}
		// Virtual addChild method to be overridden in derived classes
		virtual void addChild(const std::shared_ptr<WidgetBase>& widget) {}
		
		// Virtual resize method to be overridden in derived classes, used to resize the root widget to specified dimensions
		virtual void resize(int32_t width, int32_t height);
		// Virtual method to set the enabled state of the widget
		virtual void setIsEnabled(bool isEnabled);
		// Method to update the layout of the widget (apply new modifications, without resizing)
		void updateLayout();
	protected:
		virtual void _setGui(const Unigine::GuiPtr& gui);

		~WidgetBase();

		// The root widget of this UI component, needs to be initialized in the constructor of derived classes
		Unigine::WidgetPtr _rootWidget;

		// Current GUI instance
		Unigine::GuiPtr _gui;

		// Current scale settings
		ScaleSettings _scaleSettings;

		// Current size of the widget
		Unigine::Math::ivec2 _size;

		// Enabled state of the widget
		bool _isEnabled = true;

		friend class WidgetContainerBase;
		friend class UI;
	};
}
