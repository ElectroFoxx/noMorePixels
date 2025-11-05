#pragma once

#include <UnigineGui.h>
#include <UnigineWidgets.h>
#include <UnigineRegExp.h>
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
		Unigine::String getFontsPath(const Unigine::String& font) const;
		Unigine::String getTexturesPath(const Unigine::String& texture) const;
		Unigine::String getWhiteBackground() const;

		int32_t addDefaultFont(const char* font);
		Unigine::String getDefaultFont(int32_t fontIndex);
	private:
		Settings() = default;
		const Unigine::String _rootFolder = ".noMorePixels/";
		const Unigine::String _texturesFolder = "textures/";
		const Unigine::String _localizationFolder = "localization/";
		const Unigine::String _fontsFolder = "fonts/";

		std::vector<Unigine::String> _defaultFonts;

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
		ScaleType scaleType = ScaleType::Fill;
		float scaleFactor = 1.f;
	};


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


	class WidgetContainer : public WidgetBase
	{
	public:
		WidgetContainer(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings) {}
		virtual void resize(int32_t width, int32_t height);
		virtual void translate();
		virtual void tick(float deltaTime);
		virtual int32_t getInnerHeight() const;
		virtual int32_t getInnerWidth() const;

		virtual void addChild(const std::shared_ptr<WidgetBase>& widget);
		const std::shared_ptr<WidgetBase>& getChild(int32_t index) const;
		const int32_t getNumChildren() const { return static_cast<int32_t>(_childWidgets.size()); }

		WidgetContainer* setPadding(float top, float bottom, float left, float right);
		WidgetContainer* setPaddingEqual(bool isPaddingEqual);
		WidgetContainer* setSpacing(float spacing, bool ignorePadding = false);
		WidgetContainer* setBackgroundEnabled(bool hasBackground);
		WidgetContainer* setBackgroundColor(float r, float g, float b, float a = 1.f);
		WidgetContainer* setBackgroundColor(const Unigine::Math::vec4& color);
		WidgetContainer* setBackgroundColor(int32_t r, int32_t g, int32_t b, int32_t a = 255);
		WidgetContainer* setBackgroundTexture(const Unigine::String& texture);
		WidgetContainer* setBackgroundTextureFiltering(int32_t filtering);

		int32_t getWidth() const { return _widget->getWidth(); }
		int32_t getHeight() const { return _widget->getHeight(); }
		int32_t getContainerWidth() const { return _containterWidget ? _containterWidget->getWidth() : _widget->getWidth(); }
		int32_t getContainerHeight() const { return _containterWidget ? _containterWidget->getHeight() : _widget->getHeight(); }
	protected:
		enum class Padding : uint8_t
		{
			Top,
			Bottom,
			Left,
			Right
		};

		void _calculatePadding();
		virtual void _calculateSpacing();
		virtual void _resizeChildren();

		std::vector<std::shared_ptr<WidgetBase>> _childWidgets;
		std::vector<Unigine::WidgetVBoxPtr> _spacers;
		Unigine::Math::vec4 _padding;
		Unigine::Math::ivec4 _paddingInPixels;
		bool _isPaddingEqual = false;
		float _spacing = 0.f;
		bool _ignorePadding = false;
		int32_t _scaledSpacing = 0;
		Unigine::WidgetPtr _containterWidget;
	};


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
	private:
		Unigine::GuiPtr _gui;
		std::shared_ptr<WidgetBase> _rootWidget;
		const char* _currentDictionary = "";
	};


	class HBox : public WidgetContainer
	{
	public:
		HBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<HBox> create() { return std::make_shared<HBox>(ScaleSettings()); }
		static std::shared_ptr<HBox> create(const ScaleSettings& scaleSettings) { return std::make_shared<HBox>(scaleSettings); }
	};


	class VBox : public WidgetContainer
	{
	public:
		VBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<VBox> create() { return std::make_shared<VBox>(ScaleSettings()); }
		static std::shared_ptr<VBox> create(const ScaleSettings& scaleSettings) { return std::make_shared<VBox>(scaleSettings); }
	};


	class Label : public WidgetBase
	{
	public:
		Label(const ScaleSettings& scaleSettings);

		static std::shared_ptr<Label> create() { return std::make_shared<Label>(ScaleSettings()); }
		static std::shared_ptr<Label> create(const ScaleSettings& scaleSettings) { return std::make_shared<Label>(scaleSettings); }

		Label* setText(const char* text, bool isTranslatable = true);
		void _countTextLines(const char* text);
		Label* setFontSize(float fontSize);
		Label* setFontWrap(bool fontWrap);
		Label* setFontMaxHSpacing(float spacing);
		Label* setFontMaxVSpacing(float spacing);
		Label* setFontHSpacing(float spacing);
		Label* setFontVSpacing(float spacing);
		Label* setTextAlign(Align horizontal, Align vertical);
		Label* setDefaultFont(int32_t fontIndex);
		Label* setTextTypingAnimationCompletion(float completion);

		virtual void resize(int32_t width, int32_t height);
		void _updateFont(int32_t width);
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

	class Scroll : public WidgetBase
	{
	public:
		Scroll(const ScaleSettings& scaleSettings);

		static std::shared_ptr<Scroll> create() { return std::make_shared<Scroll>(ScaleSettings()); }
		static std::shared_ptr<Scroll> create(const ScaleSettings& scaleSettings) { return std::make_shared<Scroll>(scaleSettings); }
		virtual void resize(int32_t width, int32_t height);
		int32_t getValue() const { return _coreSlider->getValue(); }
		void setValue(int32_t value);
		Scroll* setRange(int32_t minValue, int32_t maxValue);
		Scroll* setIsReversed(bool isReversed);
		Unigine::Event<const Unigine::WidgetPtr&>& getEventChanged() { return _coreSlider->getEventChanged(); }
	private:
		void _sliderChanged(const Unigine::WidgetPtr& widget);
		void _topOrLeftClicked(const Unigine::WidgetPtr& widget, int32_t mouse);
		void _leftOrBottomClicked(const Unigine::WidgetPtr& widget, int32_t mouse);
		void _applySliderProperties(const Unigine::WidgetSliderPtr& slider);
		void _setSliderRange();
		Unigine::EventConnections ec;
		Unigine::WidgetHBoxPtr _sliderBox;
		Unigine::WidgetSliderPtr _coreSlider;
		Unigine::WidgetSpritePtr _bottomSprite, _topSprite, _sliderSprite;
		Unigine::TexturePtr _bottomTexture, _topTexture, _sliderTexture;
		int32_t _sliderSpace, _sliderSize;
		int32_t _topHeight, _bottomHeight;
		int32_t _minValue = 0, _maxValue = 100;
		bool _isReversed = false, _isVertical = true;
	};

	class ScrollBox : public WidgetContainer
	{
	public:
		ScrollBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<ScrollBox> create() { return std::make_shared<ScrollBox>(ScaleSettings()); }
		static std::shared_ptr<ScrollBox> create(const ScaleSettings& scaleSettings) { return std::make_shared<ScrollBox>(scaleSettings); }

		virtual void resize(int32_t width, int32_t height);
		virtual void addChild(const std::shared_ptr<WidgetBase>& widget);
		ScrollBox* setVisibleItemCount(int32_t itemCount);
	protected:
		virtual void _resizeChildren();
	private:
		std::shared_ptr<Scroll> _verticalScroll;
		std::shared_ptr<Scroll> _horizontalScroll;

		Unigine::EventConnections ec;

		void _onVerticalScrollBoxChanged(const Unigine::WidgetPtr& widget);
		void _onVerticalScrollChanged(const Unigine::WidgetPtr& widget);

		int32_t _itemCount = 0;
		int32_t _scaledItemHeight = 0;
	};


	class EditLine : public WidgetBase
	{
	public:
		EditLine(const ScaleSettings& scaleSettings);

		static std::shared_ptr<EditLine> create() { return std::make_shared<EditLine>(ScaleSettings()); }
		static std::shared_ptr<EditLine> create(const ScaleSettings& scaleSettings) { return std::make_shared<EditLine>(scaleSettings); }

		virtual void resize(int32_t width, int32_t height);
		EditLine* setDefaultFont(int32_t fontIndex);
		EditLine* setMaxTextLength(int32_t maxLength);
		EditLine* setValidationPattern(const char* pattern);
		EditLine* setValidationEnabled(bool isEnabled);
		EditLine* setValidationStrictMode(bool isStrictMode);

		const Unigine::String& getText() const { return _previousText; }
	private:
		void _calculateMaxFontSize();
		void _onTextChanged(const Unigine::WidgetPtr& widget);

		Unigine::EventConnections ec;

		Unigine::RegExpPtr regExpValidator;
		Unigine::String _previousText;
		bool _isValidationEnabled = false;
		bool _isValidationStrictMode = false;

		int32_t _maxFontSize = 0;
		int32_t _maxTextLength = 0;

		float _magicMaxFontProportion = static_cast<float>(999) / 1124;
	};

	class CheckBox : public WidgetBase
	{
	public:
		CheckBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<CheckBox> create() { return std::make_shared<CheckBox>(ScaleSettings()); }
		static std::shared_ptr<CheckBox> create(const ScaleSettings& scaleSettings) { return std::make_shared<CheckBox>(scaleSettings); }

		Unigine::TexturePtr _backgroundTexture, _tickTexture;
	};

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