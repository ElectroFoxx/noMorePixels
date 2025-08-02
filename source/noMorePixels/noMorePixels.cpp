#include "noMorePixels.h"
#include <UnigineEngine.h>

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

void WidgetBase::resize(int32_t width, int32_t height)
{
	_widget->setWidth(width);
	_widget->setHeight(height);
}

Unigine::Event<const Unigine::WidgetPtr&>& noMoPi::WidgetBase::getEventEnter()
{
	return _widget->getEventEnter();
}

Unigine::Event<const Unigine::WidgetPtr&>& noMoPi::WidgetBase::getEventLeave()
{
	return _widget->getEventLeave();
}

Unigine::Event<const Unigine::WidgetPtr&, int>& noMoPi::WidgetBase::getEventPressed()
{
	return _widget->getEventPressed();
}

Unigine::Event<const Unigine::WidgetPtr&, int>& noMoPi::WidgetBase::getEventClicked()
{
	return _widget->getEventClicked();
}

void WidgetContainer::resize(int32_t width, int32_t height)
{
	WidgetBase::resize(width, height);

	_calculatePadding();
	_calculateSpacing();

	_resizeChildren();
}

void noMoPi::WidgetContainer::_resizeChildren()
{
	float totalWidgetsWeight = 0.f;
	int32_t fillWidgetsCount = 0;
	int32_t otherWidgetsCount = 0;

	for (auto& child : _childWidgets)
	{
		if (child->getScaleSettings().scaleType == ScaleType::Fill)
		{
			totalWidgetsWeight += child->getScaleSettings().scaleFactor;
			fillWidgetsCount++;
		}
		else
			otherWidgetsCount++;
	}

	int32_t parentWidth = getInnerWidth();
	int32_t parentHeight = getInnerHeight();

	bool isHorizontal = _widget->getType() == Unigine::Widget::TYPE::WIDGET_HBOX;
	Unigine::Log::message("%s\n", isHorizontal ? "TRUE" : "FALSE");

	if (isHorizontal)
	{
		uint32_t spacing = static_cast<int32_t>(_spacing * parentWidth);
		parentWidth -= spacing * (static_cast<int32_t>(_childWidgets.size()) - 1);
	}
	else
	{
		uint32_t spacing = static_cast<int32_t>(_spacing * parentHeight);
		parentHeight -= spacing * (static_cast<int32_t>(_childWidgets.size()) - 1);
	}

	int32_t spaceLeft = isHorizontal ? parentWidth : parentHeight;
	for (auto& child : _childWidgets)
	{
		const ScaleType& scaleType = child->getScaleSettings().scaleType;
		if (scaleType == ScaleType::Fill)
		{
			int32_t childSize = isHorizontal ?
				fillWidgetsCount > 1 ? static_cast<int32_t>(parentWidth * (child->getScaleSettings().scaleFactor / totalWidgetsWeight)) : spaceLeft :
				fillWidgetsCount > 1 ? static_cast<int32_t>(parentHeight * (child->getScaleSettings().scaleFactor / totalWidgetsWeight)) : spaceLeft;

			if (isHorizontal)
				child->resize(childSize, parentHeight);
			else
				child->resize(parentWidth, childSize);

			spaceLeft -= childSize;
			fillWidgetsCount--;
		}
		else if (scaleType == ScaleType::Proportional)
		{
			int32_t childSize = isHorizontal ?
				static_cast<int32_t>(parentHeight * child->getScaleSettings().scaleFactor) :
				static_cast<int32_t>(parentWidth * child->getScaleSettings().scaleFactor);

			if (isHorizontal)
				child->resize(childSize, parentHeight);
			else
				child->resize(parentWidth, childSize);

			spaceLeft -= childSize;
		}
	}
}

int32_t noMoPi::WidgetContainer::getInnerHeight() const
{
	int32_t verticalPadding = _isPaddingEqual ? _paddingInPixels.min() * 2 : _paddingInPixels[std::to_underlying(Padding::Top)] + _paddingInPixels[std::to_underlying(Padding::Bottom)];

	return getHeight() - verticalPadding;
}

int32_t noMoPi::WidgetContainer::getInnerWidth() const
{
	int32_t horizontalPadding = _isPaddingEqual ? _paddingInPixels.min() * 2 : _paddingInPixels[std::to_underlying(Padding::Left)] + _paddingInPixels[std::to_underlying(Padding::Right)];

	return getWidth() - horizontalPadding;
}

WidgetContainer* WidgetContainer::setPadding(float top, float bottom, float left, float right)
{
	_padding = Unigine::Math::vec4(top, bottom, left, right);
	return this;
}

WidgetContainer* noMoPi::WidgetContainer::setPaddingEqual(bool isPaddingEqual)
{
	_isPaddingEqual = isPaddingEqual;
	return this;
}

WidgetContainer* WidgetContainer::setSpacing(float spacing)
{
	_spacing = spacing;
	return this;
}

void noMoPi::WidgetContainer::_calculatePadding()
{
	const int32_t width = getWidth();
	const int32_t height = getHeight();

	_paddingInPixels[std::to_underlying(Padding::Left)] = static_cast<int32_t>(width * _padding[std::to_underlying(Padding::Left)]);
	_paddingInPixels[std::to_underlying(Padding::Right)] = static_cast<int32_t>(width * _padding[std::to_underlying(Padding::Right)]);
	_paddingInPixels[std::to_underlying(Padding::Top)] = static_cast<int32_t>(height * _padding[std::to_underlying(Padding::Top)]);
	_paddingInPixels[std::to_underlying(Padding::Bottom)] = static_cast<int32_t>(height * _padding[std::to_underlying(Padding::Bottom)]);

	if (Unigine::WidgetVBoxPtr box = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
	{
		if (_isPaddingEqual)
		{
			int32_t smallestPadding = _paddingInPixels.min();
			box->setPadding(smallestPadding, smallestPadding, smallestPadding, smallestPadding);
		}
		else
			box->setPadding(
				_paddingInPixels[std::to_underlying(Padding::Left)],
				_paddingInPixels[std::to_underlying(Padding::Right)],
				_paddingInPixels[std::to_underlying(Padding::Top)],
				_paddingInPixels[std::to_underlying(Padding::Bottom)]);
	}
}

void noMoPi::WidgetContainer::_calculateSpacing()
{
	const int32_t width = getInnerWidth();
	const int32_t height = getInnerHeight();

	if (Unigine::WidgetHBoxPtr hbox = Unigine::dynamic_ptr_cast<Unigine::WidgetHBox>(_widget))
	{
		int32_t scaledSpacing = static_cast<int32_t>(width * _spacing);
		//hbox->setSpace(scaledSpacing, 0);
		for (auto& spacer : _spacers)
		{
			spacer->setWidth(scaledSpacing);
			spacer->setHeight(height);
		}
			
	}
	else if (Unigine::WidgetVBoxPtr vbox = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget))
	{
		int32_t scaledSpacing = static_cast<int32_t>(height * _spacing);
		//vbox->setSpace(0, scaledSpacing);
		for (auto& spacer : _spacers)
		{
			spacer->setWidth(width);
			spacer->setHeight(scaledSpacing);
		}
	}
}

HBox::HBox(const ScaleSettings& scaleSettings) : WidgetContainer(scaleSettings)
{
	_widget = Unigine::WidgetHBox::create();

	if (Unigine::WidgetHBoxPtr hbox = Unigine::static_ptr_cast<Unigine::WidgetHBox>(_widget))
	{
		hbox->setBackgroundTexture(".noMorePixels/textures/white.png");
	}
}

void noMoPi::WidgetContainer::addChild(const std::shared_ptr<WidgetBase>& widget)
{
	Unigine::GuiPtr gui = _widget->getGui();
	widget->setGui(gui);

	if (_childWidgets.size() >= 1)
	{
		Unigine::WidgetVBoxPtr spacer = Unigine::WidgetVBox::create();
		_spacers.push_back(spacer);
		_widget->addChild(spacer);
	}

	_childWidgets.push_back(widget);

	if (_widget && widget->getWidget())
		_widget->addChild(widget->getWidget());
}

WidgetContainer* WidgetContainer::setBackgroundEnabled(bool hasBackground)
{
	if (Unigine::WidgetVBoxPtr box = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget))
		box->setBackground(hasBackground);
	
	return this;
}

WidgetContainer* WidgetContainer::setBackgroundColor(float r, float g, float b, float a)
{
	if (Unigine::WidgetVBoxPtr box = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget))
		box->setBackgroundColor(Unigine::Math::vec4(r, b, g, a));
	
	return this;
}

WidgetContainer* WidgetContainer::setBackgroundColor(int32_t r, int32_t g, int32_t b, int32_t a)
{
	return setBackgroundColor(static_cast<float>(r) / 255, static_cast<float>(g) / 255, static_cast<float>(b) / 255, static_cast<float>(a) / 255);
}

WidgetContainer* noMoPi::WidgetContainer::setBackgroundTexture(const Unigine::String& texture)
{
	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
	{
		vbox->setBackgroundTexture(Settings::get().getTexturesPath(texture));
	}
	
	return this;
}

WidgetContainer* noMoPi::WidgetContainer::setBackgroundTextureFiltering(int32_t filtering)
{
	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
	{
		vbox->setBackgroundCustomFilterEnabled(true);
		vbox->setBackgroundCustomFilter(filtering);
	}
	
	return this;
}

VBox::VBox(const ScaleSettings& scaleSettings) : WidgetContainer(scaleSettings)
{
	_widget = Unigine::WidgetVBox::create();

	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
	{
		vbox->setBackgroundTexture(Settings::get().getWhiteBackground());
		vbox->setStencil(true);
	}
		
}

Label::Label(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	_widget = Unigine::WidgetHBox::create();
	_label = Unigine::WidgetLabel::create();
	_widget->addChild(_label);
}

Label* Label::setText(const char* text, bool isTranslatable)
{
	if (isTranslatable)
	{
		Unigine::GuiPtr gui = _widget->getGui();
		_keyText = text;
		_targetText = gui->translate(text);
	}
	else
		_targetText = text;

	_countTextLines(_targetText);
	
	_label->setText(_targetText);
	
	return this;
}

void noMoPi::Label::_countTextLines(const char* text)
{
	_newLineCount = 0;
	for (const char* c = text; *c; c++)
	{
		if (*c == '\n')
			_newLineCount++;
	}
}

Label* Label::setFontSize(float fontSize)
{
	_fontSize = std::clamp(fontSize, 0.f, 1.f);
	
	return this;
}

Label* Label::setFontWrap(bool fontWrap)
{
	_fontWrap = fontWrap;
	_label->setFontWrap(_fontWrap);

	return this;
}

Label* Label::setFontMaxHSpacing(float spacing)
{
	_fontMaxHSpacing = spacing;
	
	return this;
}

Label* Label::setFontMaxVSpacing(float spacing)
{
	_fontMaxVSpacing = spacing;

	return this;
}

Label* Label::setFontHSpacing(float spacing)
{
	_fontHSpacing = spacing;
	_label->setFontHSpacing(static_cast<int32_t>(_maxfontHSpacing * _fontHSpacing));
	
	return this;
}

Label* Label::setFontVSpacing(float spacing)
{
	_fontVSpacing = spacing;
	_label->setFontVSpacing(static_cast<int32_t>(_maxfontVSpacing * _fontVSpacing));
	
	return this;
}

Label* Label::setTextAlign(Align horizontal, Align vertical)
{
	if (_label)
	{
		if (horizontal == Align::Left)
			_label->setTextAlign(Unigine::Gui::ALIGN_LEFT);
		else if (horizontal == Align::Right)
			_label->setTextAlign(Unigine::Gui::ALIGN_RIGHT);
		else
			_label->setTextAlign(Unigine::Gui::ALIGN_CENTER);

		if (vertical == Align::Top)
			_label->setFlags(Unigine::Gui::ALIGN_TOP);
		else if (vertical == Align::Bottom)
			_label->setFlags(Unigine::Gui::ALIGN_BOTTOM);
		else
			_label->setFlags(Unigine::Gui::ALIGN_CENTER);
	}

	return this;
}

Label* noMoPi::Label::setDefaultFont(int32_t fontIndex)
{
	_label->setFont(Settings::get().getDefaultFont(fontIndex));

	_calculateMaxFontParams();

	_updateFont(_widget->getWidth());
	
	return this;
}

Label* noMoPi::Label::setTextTypingAnimationCompletion(float completion)
{
	if (!_newLineCount && !_fontWrap)
	{
		int32_t currentTextSize = static_cast<int32_t>(Unigine::Math::roundFast(_targetText.size() * completion));

		_label->setText(_targetText.substr(0, currentTextSize));
	}
	else
	{
		Unigine::StringArray<> lines = _targetText.split(_targetText, "\n");

		Unigine::String targetText = "";
		for (int32_t i = 0; i < lines.size(); i++)
		{
			const Unigine::String& line = lines[i];

			int32_t currentTextSize = static_cast<int32_t>(Unigine::Math::roundFast(line.size() * completion));

			Unigine::String text = line.substr(0, currentTextSize);

			targetText += text;
			if (i < lines.size() - 1)
				targetText += "\n";
		}

		_label->setText(targetText);
	}
	
	return this;
}

void Label::resize(int32_t width, int32_t height)
{
	WidgetBase::resize(width, height);

	_calculateMaxFontParams();

	_updateFont(width);
}

void noMoPi::Label::_updateFont(int32_t width)
{
	if (_label)
	{
		_label->setWidth(width);

		if (!_fontWrap)
			_label->setFontSize(static_cast<int32_t>(_maxFontSize * _fontSize));
		else
			_label->setFontSize(_maxFontSize);

		_label->setFontHSpacing(static_cast<int32_t>(_maxfontHSpacing * _fontHSpacing));
		_label->setFontVSpacing(static_cast<int32_t>(_maxfontVSpacing * _fontVSpacing));
	}
}

void noMoPi::Label::_calculateMaxFontParams()
{
	const int32_t height = _widget->getHeight();
	const int32_t width = _widget->getWidth();

	if (!_fontWrap)
	{
		_label->setFontSize(height);
		const Unigine::Math::ivec2 textRenderSize = _label->getTextRenderSize(_targetText);
		
		_label->setFontHSpacing(static_cast<int32_t>(height * _fontMaxHSpacing));
		_label->setFontVSpacing(static_cast<int32_t>(height * _fontMaxVSpacing));

		const Unigine::Math::ivec2 textRenderSizeSpacing = _label->getTextRenderSize(_targetText);

		const float hScaleProportion = static_cast<float>(textRenderSizeSpacing.x) / textRenderSize.x;
		const float vScaleProportion = 1.f + _fontMaxVSpacing;

		_maxFontSize = height / (_newLineCount + 1);
		if (textRenderSize.x > width)
		{
			float maxBaseWidth = static_cast<float>(width) / hScaleProportion;

			_maxFontSize = static_cast<int32_t>(height * (maxBaseWidth / textRenderSize.x));
		}
		if (textRenderSize.y > height)
		{
			float maxBaseHeight = static_cast<float>(height) / vScaleProportion;

			_maxFontSize = std::min(static_cast<int32_t>(height * (maxBaseHeight / textRenderSize.y)), _maxFontSize);
		}
	}
	else
		_maxFontSize = static_cast<int32_t>(height * _fontSize);

	_maxfontHSpacing = static_cast<int32_t>(_maxFontSize * _fontMaxHSpacing);
	_maxfontVSpacing = static_cast<int32_t>(_maxFontSize * _fontMaxVSpacing);
}

Unigine::String noMoPi::Settings::getLocalizationPath(const Unigine::String& file) const
{
	return _rootFolder + _localizationFolder + file;
}

Unigine::String noMoPi::Settings::getFontsPath(const Unigine::String& font) const
{
	return _rootFolder + _localizationFolder + font;
}

Unigine::String noMoPi::Settings::getTexturesPath(const Unigine::String& texture) const
{
	return _rootFolder + _texturesFolder + texture;
}

Unigine::String Settings::getWhiteBackground() const
{
	return _rootFolder + _texturesFolder + _whiteBackground;
}

int32_t Settings::addDefaultFont(const char* font)
{
	int32_t fontIndex = _defaultFonts.size();

	_defaultFonts.push_back(font);

	return fontIndex;
}

Unigine::String noMoPi::Settings::getDefaultFont(int32_t fontIndex)
{
	return _rootFolder + _fontsFolder + _defaultFonts[fontIndex];
}

void UI::translate()
{
	_rootWidget->translate();
}

void noMoPi::UI::tick()
{
	_rootWidget->tick(Unigine::Engine::get()->getIFps());
}

void WidgetContainer::translate()
{
	for (auto& child : _childWidgets)
		child->translate();
}

void WidgetContainer::tick(float deltaTime)
{
	for (auto& child : _childWidgets)
		child->tick(deltaTime);
}

void Label::translate()
{
	Unigine::GuiPtr gui = _widget->getGui();

	setText(gui->translate(_keyText));


	_calculateMaxFontParams();

	_updateFont(_widget->getWidth());
}

ScrollBox::ScrollBox(const ScaleSettings& scaleSettings) : WidgetContainer(scaleSettings)
{
	Unigine::WidgetScrollBoxPtr scroll = Unigine::WidgetScrollBox::create();

	scroll->setVScrollEnabled(true);
	scroll->setHScrollEnabled(false);
	Unigine::WidgetScrollPtr scr = scroll->getVScroll();
	scr->setSliderButton(false);
	scroll->setBorder(0);
	static Unigine::EventConnection ec;
	scroll->getEventChanged().connect(ec, [](const Unigine::WidgetPtr& widget) {
		Unigine::WidgetScrollBoxPtr scr = Unigine::dynamic_ptr_cast<Unigine::WidgetScrollBox>(widget);
		Unigine::WidgetScrollPtr scroll = scr->getVScroll();
		Unigine::Log::message("%d %d\n", scr->getVScrollValue() / scr->getScrollScale(), (scr->getVScrollObjectSize() - scr->getVScrollFrameSize()) / scr->getScrollScale());
		});
	for (int i = 0; i < 100; i++)
	{
		scroll->addChild(Unigine::WidgetButton::create("test"));
	}
	
	_widget = scroll;
}

void ScrollBox::resize(int32_t width, int32_t height)
{
	WidgetContainer::resize(width, height);

	Unigine::WidgetScrollBoxPtr scr = Unigine::dynamic_ptr_cast<Unigine::WidgetScrollBox>(_widget);

	_widget->setWidth(width - 16);
}

EditLine::EditLine(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	Unigine::WidgetEditLinePtr _editLine = Unigine::WidgetEditLine::create("test");
	_editLine->setStyleTextureBackground(".noMorePixels/textures/white.png");
	_editLine->setBackgroundColor(Unigine::Math::vec4_green);
	_editLine->setBorderColor(Unigine::Math::vec4_zero);

	_widget = _editLine;
}

void EditLine::resize(int32_t width, int32_t height)
{
	// no way to remove the border, so this is a temporary fix
	_widget->setWidth(width - 4);
	_widget->setHeight(height - 4);

	_calculateMaxFontSize();

	_widget->setFontSize(_maxFontSize);
}

void noMoPi::EditLine::_calculateMaxFontSize()
{
	int32_t height = _widget->getHeight();
	
	_maxFontSize = static_cast<int32_t>((height - 5) * _magicMaxFontProportion + 2);
}

EditLine* EditLine::setDefaultFont(int32_t fontIndex)
{
	_widget->setFont(Settings::get().getDefaultFont(fontIndex));

	return this;
}

noMoPi::CheckBox::CheckBox(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	Unigine::WidgetSpritePtr sprite = Unigine::WidgetSprite::create();

	_backgroundTexture = Unigine::Texture::create();
	_backgroundTexture->load(Settings::get().getTexturesPath("border.png"));
	//_backgroundTexture->setSamplerFlags(Unigine::Texture::SAMPLER_FILTER_POINT);

	sprite->setRender(_backgroundTexture);

	_tickTexture = Unigine::Texture::create();
	_tickTexture->load(Settings::get().getTexturesPath("tick.png"));
	//_tickTexture->setSamplerFlags(Unigine::Texture::SAMPLER_FILTER_POINT);

	sprite->setLayerRender(sprite->addLayer(), _tickTexture);

	static Unigine::EventConnection ec;

	sprite->getEventClicked().connect(ec, [](const Unigine::WidgetPtr& widget, int mouse) {
		Unigine::WidgetSpritePtr sprite = Unigine::dynamic_ptr_cast<Unigine::WidgetSprite>(widget);
		sprite->setLayerEnabled(1, !sprite->isLayerEnabled(1));
		});

	_widget = sprite;
}

