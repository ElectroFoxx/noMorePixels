#pragma once

#include <UnigineGui.h>
#include <UnigineWidgets.h>
#include <vector>
#include <memory>

namespace noMoPi
{
	class Settings
	{
	public:
		static Settings& get()
		{
			static Settings instance;
			return instance;
		}

		Unigine::String getLocalizationPath(const Unigine::String& file) const;
		Unigine::String getWhiteBackground() const;
	private:
		Settings() = default;
		const Unigine::String _rootFolder = ".noMorePixels/";
		const Unigine::String _texturesFolder = "textures/";
		const Unigine::String _localizationFolder = "localization/";

		const Unigine::String _whiteBackground = "white.png";
	};


	enum class ScaleType : uint8_t
	{
		Fill,
		Proportional,
		PixelPerfect
	};


	enum class Align : uint8_t
	{
		Top,
		Bottom,
		Left,
		Right,
		Center
	};


	struct ScaleSettings
	{
		ScaleType scaleType;
		float scaleFactor = 1.f;
	};


	class WidgetBase
	{
	public:
		WidgetBase(const ScaleSettings& scaleSettings) : _scaleSettings(scaleSettings) {}
		void setGui(const Unigine::GuiPtr& gui) { _widget->setGui(gui); }
		virtual void resize(int32_t width, int32_t height);
		operator const Unigine::WidgetPtr& () const { return _widget; }
		const ScaleSettings& getScaleSettings() const { return _scaleSettings; }
		Unigine::WidgetPtr getWidget() { return _widget; }
		virtual void translate() {}
	protected:
		Unigine::WidgetPtr _widget;

		ScaleSettings _scaleSettings;
	};


	class WidgetContainer : public WidgetBase
	{
	public:
		WidgetContainer(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings) {}
		virtual void resize(int32_t width, int32_t height);
		virtual void translate();
		void _resizeChildren();
		int32_t getInnerHeight() const;
		int32_t getInnerWidth() const;

		void addChild(const std::shared_ptr<WidgetBase>& widget);

		WidgetContainer* setPadding(float top, float bottom, float left, float right);
		WidgetContainer* setSpacing(float spacing);
		WidgetContainer* setBackground(bool hasBackground);
		WidgetContainer* setBackgroundColor(float r, float g, float b, float a = 1.f);
		WidgetContainer* setBackgroundColor(int32_t r, int32_t g, int32_t b, int32_t a = 255);

		int32_t getWidth() const { return _widget->getWidth(); }
		int32_t getHeight() const { return _widget->getHeight(); }
	protected:
		enum class Padding : uint8_t
		{
			Top,
			Bottom,
			Left,
			Right
		};

		void _calculatePadding();
		void _calculateSpacing();

		std::vector<std::shared_ptr<WidgetBase>> _childWidgets;
		Unigine::Math::vec4 _padding;
		float _spacing = 0.f;
	};


	class UI
	{
	public:
		UI(const Unigine::GuiPtr& gui) : _gui(gui) {}
		void setRootWidget(const std::shared_ptr<WidgetBase>& widget);
		void updateLayout();
		void addDictionary(const char* dictionary);
		void translate();
	private:
		Unigine::GuiPtr _gui;
		std::shared_ptr<WidgetBase> _rootWidget;
	};


	class HBox : public WidgetContainer
	{
	public:
		HBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<WidgetContainer> create() { return std::make_shared<HBox>(ScaleSettings()); }
		static std::shared_ptr<WidgetContainer> create(const ScaleSettings& scaleSettings) { return std::make_shared<HBox>(scaleSettings); }
	};


	class VBox : public WidgetContainer
	{
	public:
		VBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<WidgetContainer> create() { return std::make_shared<VBox>(ScaleSettings()); }
		static std::shared_ptr<WidgetContainer> create(const ScaleSettings& scaleSettings) { return std::make_shared<VBox>(scaleSettings); }
	};


	class Label : public WidgetBase
	{
	public:
		Label(const ScaleSettings& scaleSettings);

		static std::shared_ptr<Label> create() { return std::make_shared<Label>(ScaleSettings()); }
		static std::shared_ptr<Label> create(const ScaleSettings& scaleSettings) { return std::make_shared<Label>(scaleSettings); }

		Label* setText(const char* text);
		Label* setFontSize(float fontSize);
		Label* setFontWrap(bool fontWrap);
		Label* setFontMaxHSpacing(float spacing);
		Label* setFontMaxVSpacing(float spacing);
		Label* setFontHSpacing(float spacing);
		Label* setFontVSpacing(float spacing);
		Label* setTextAlign(Align horizontal, Align vertical);

		virtual void resize(int32_t width, int32_t height);
		virtual void translate();

	protected:
		void _calculateMaxFontParams();

		Unigine::WidgetLabelPtr _label;

		bool _isTextTranslatable = false;
		Unigine::String _targetText, _keyText;

		int32_t _maxFontSize = 0;
		
		float _fontSize = 1.f;
		int32_t _newLineCount = 0;
		bool _fontWrap = false;

		float _fontMaxHSpacing = 0.f;
		int32_t _maxfontHSpacing = 0;
		float _fontMaxVSpacing = 0.f;
		int32_t _maxfontVSpacing = 0;
		
		float _fontHSpacing = 0.f;
		float _fontVSpacing = 0.f;
	};
}