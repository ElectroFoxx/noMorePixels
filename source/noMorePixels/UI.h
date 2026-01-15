#pragma once

#include <memory>

#include <UnigineWidgets.h>

#include "base/WidgetBase.h"

namespace noMoPi
{
	class UI
	{
	public:
		/**
		 * Default constructor.
		 * 
		 */
		UI() {}

		/**
		 * Constructor with parameter to set gui.
		 * 
		 * \param gui GUI to be used in the UI.
		 */
		UI(const Unigine::GuiPtr& gui) : _gui(gui) {}

		/**
		 * GUI setter.
		 * 
		 * \param gui
		 */
		void setGui(const Unigine::GuiPtr& gui) { _gui = gui; }

		/**
		 * Root widget setter.
		 * 
		 * \param widget To work correctly, widget needs to support adding children.
		 */
		void setRootWidget(const std::shared_ptr<WidgetBase>& widget);

		/**
		 * Set the dictionary name, that will be used for the translation of the text widgets.
		 * 
		 * \param dictionary Relative path/name of the dictionary file, from the localization folder from Settings.
		 */
		void setDictionary(const Unigine::String& dictionary);

		/**
		 * Set the current language, and try to load dictionary translations.
		 * 
		 * \param language Language code from the dictionary file.
		 */
		void setLanguage(const Unigine::String& language);

		/**
		 * Run the translation for all widgets.
		 * 
		 */
		void translate();

		/**
		 * Set the size of the GUI, and the root widget.
		 * 
		 * \param size
		 */
		void setSize(const Unigine::Math::ivec2& size);

		/**
		 * Add child widget to the root widget.
		 * 
		 * \param widget
		 */
		void addChild(const std::shared_ptr<WidgetBase>& widget);

		/**
		 * Update the layout, starting from the root widget.
		 * 
		 */
		void updateLayout();
		
		/**
		 * Tick function to update widgets logic, starting from the root widget.
		 * 
		 */
		void tick();

		/**
		 * Clear all child widgets from the root widget.
		 * 
		 * \param deleteRoot If true, delete also the root widget.
		 */
		void clear(bool deleteRoot);

	private:
		void _clearGuiChildren();

		Unigine::GuiPtr _gui;
		std::shared_ptr<WidgetBase> _rootWidget;
		Unigine::String _currentDictionary, _currentLanguage;

		friend class TestPlayer;
	};
}
