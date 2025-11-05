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

	if (Interactive* interactive = dynamic_cast<Interactive*>(this))
		interactive->resize(width, height);
}

void noMoPi::WidgetBase::setIsEnabled(bool isEnabled)
{
	_widget->setEnabled(isEnabled);

	if (Interactive* interactive = dynamic_cast<Interactive*>(this))
		interactive->_setIsEnabled(isEnabled);
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

WidgetContainer* WidgetContainer::setSpacing(float spacing, bool ignorePadding)
{
	_spacing = spacing;
	_ignorePadding = ignorePadding;
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
	const int32_t width = _ignorePadding ? getWidth() : getInnerWidth();
	const int32_t height = _ignorePadding ? getHeight() : getInnerHeight();

	if (_widget->getType() == Unigine::Widget::TYPE::WIDGET_HBOX)
	{
		_scaledSpacing = static_cast<int32_t>(width * _spacing);
		for (auto& spacer : _spacers)
		{
			spacer->setWidth(_scaledSpacing);
			spacer->setHeight(height);
		}
			
	}
	else if (_widget->getType() == Unigine::Widget::TYPE::WIDGET_VBOX || _widget->getType() == Unigine::Widget::TYPE::WIDGET_SCROLL_BOX)
	{
		_scaledSpacing = static_cast<int32_t>(height * _spacing);
		for (auto& spacer : _spacers)
		{
			spacer->setWidth(width);
			spacer->setHeight(_scaledSpacing);
		}
	}
}

HBox::HBox(const ScaleSettings& scaleSettings) : WidgetContainer(scaleSettings)
{
	_widget = Unigine::WidgetHBox::create();
	_containterWidget = _widget;

	if (Unigine::WidgetHBoxPtr hbox = Unigine::static_ptr_cast<Unigine::WidgetHBox>(_widget))
	{
		hbox->setBackgroundTexture(".noMorePixels/textures/white.png");
	}
}

void noMoPi::WidgetContainer::addChild(const std::shared_ptr<WidgetBase>& widget)
{
	Unigine::GuiPtr gui = _widget->getGui();
	widget->setGui(gui);

	// Interactive vidgets support events like onClicked, onEnter, ect
	if (Interactive* interactive = dynamic_cast<Interactive*>(widget.get()))
	{
		interactive->setGui(gui);
		interactive->attach(widget);
	}

	if (_childWidgets.size() >= 1)
	{
		Unigine::WidgetVBoxPtr spacer = Unigine::WidgetVBox::create();
		_spacers.push_back(spacer);
		_containterWidget->addChild(spacer);
	}

	_childWidgets.push_back(widget);

	if (_containterWidget && widget->getWidget())
		_containterWidget->addChild(widget->getWidget());
}

const std::shared_ptr<WidgetBase>& noMoPi::WidgetContainer::getChild(int32_t index) const
{
	return _childWidgets.at(index);
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

WidgetContainer* noMoPi::WidgetContainer::setBackgroundColor(const Unigine::Math::vec4& color)
{
	if (Unigine::WidgetVBoxPtr box = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget))
		box->setBackgroundColor(color);

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
	_containterWidget = _widget;

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

	_label->setFontSize(static_cast<int32_t>(_maxFontSize * fontSize));
	
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
		const Unigine::Math::ivec2 textRawRenderSize = _label->getTextRenderSize(_targetText);
		
		_label->setFontHSpacing(static_cast<int32_t>(height * _fontMaxHSpacing));
		_label->setFontVSpacing(static_cast<int32_t>(height * _fontMaxVSpacing));

		const Unigine::Math::ivec2 textRenderSizeWithSpacing = _label->getTextRenderSize(_targetText);

		const float hScaleProportion = static_cast<float>(textRenderSizeWithSpacing.x) / textRawRenderSize.x;
		const float vScaleProportion = 1.f + _fontMaxVSpacing;

		_maxFontSize = height / (_newLineCount + 1);
		if (textRenderSizeWithSpacing.x > width)
		{
			float maxBaseWidth = static_cast<float>(width) / hScaleProportion;

			_maxFontSize = static_cast<int32_t>(height * (maxBaseWidth / textRawRenderSize.x));
		}
		if (textRenderSizeWithSpacing.y > height)
		{
			float maxBaseHeight = static_cast<float>(height) / vScaleProportion;

			_maxFontSize = std::min(static_cast<int32_t>(height * (maxBaseHeight / textRawRenderSize.y)), _maxFontSize);
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

void UI::addChild(const std::shared_ptr<WidgetBase>& widget)
{
	_rootWidget->addChild(widget);
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

EditLine::EditLine(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	Unigine::WidgetEditLinePtr _editLine = Unigine::WidgetEditLine::create("test");
	_editLine->setStyleTextureBackground(".noMorePixels/textures/white.png");
	_editLine->setBackgroundColor(Unigine::Math::vec4_green);
	_editLine->setBorderColor(Unigine::Math::vec4_zero);

	_editLine->getEventChanged().connect(ec, this, &EditLine::_onTextChanged);

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

void noMoPi::EditLine::_onTextChanged(const Unigine::WidgetPtr& widget)
{
	Unigine::WidgetEditLinePtr editLine = Unigine::dynamic_ptr_cast<Unigine::WidgetEditLine>(widget);

	bool textChanged = false;
	if (editLine)
	{
		Unigine::String text = editLine->getText();

		if (_maxTextLength > 0 && text.size() > _maxTextLength)
		{
			text = text.substr(0, _maxTextLength);
			textChanged = true;
		}

		if (_isValidationEnabled)
		{
			bool isTextCorrect = regExpValidator->match(text);
			if (!isTextCorrect)
			{
				if (_isValidationStrictMode)
				{
					text = _previousText;
					textChanged = true;
				}
			}
		}

		if (textChanged)
		{
			editLine->setText(text);
		}
		_previousText = text;
		Unigine::Log::message("EditLine text changed: %s\n", text.get());
	}
}

EditLine* EditLine::setDefaultFont(int32_t fontIndex)
{
	_widget->setFont(Settings::get().getDefaultFont(fontIndex));

	return this;
}

EditLine* noMoPi::EditLine::setMaxTextLength(int32_t maxLength)
{
	_maxTextLength = maxLength;
	
	return this;
}

EditLine* noMoPi::EditLine::setValidationPattern(const char* pattern)
{
	regExpValidator = Unigine::RegExp::create(pattern);
	
	return this;
}

EditLine* noMoPi::EditLine::setValidationEnabled(bool isEnabled)
{
	_isValidationEnabled = isEnabled;
	
	return this;
}

EditLine* noMoPi::EditLine::setValidationStrictMode(bool isStrictMode)
{
	_isValidationStrictMode = isStrictMode;
	
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

void noMoPi::Interactive::setGui(Unigine::GuiPtr gui)
{
	_interactiveLayer->setGui(gui);
}

Interactive::Interactive()
{
	_interactiveLayer = Unigine::WidgetButton::create("");
	_interactiveLayer->setBackground(false);
}

void Interactive::resize(int32_t width, int32_t height)
{
	_interactiveLayer->setWidth(width);
	_interactiveLayer->setHeight(height);
}

void noMoPi::Interactive::attach(const std::shared_ptr<WidgetBase>& widget)
{
	widget->getWidget()->addChild(_interactiveLayer, Unigine::Gui::ALIGN_OVERLAP | Unigine::Gui::ALIGN_FIXED);
}

noMoPi::Scroll::Scroll(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	_widget = Unigine::WidgetVBox::create();
	Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget)->setBackgroundColor(Unigine::Math::vec4_blue);
	Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget)->setBackground(1);

	_topSprite = Unigine::WidgetSprite::create();
	_topSprite->getEventClicked().connect(ec, this, &Scroll::_topOrLeftClicked);

	static Unigine::EventConnections econn;

	_bottomSprite = Unigine::WidgetSprite::create();
	_bottomSprite->getEventClicked().connect(ec, this, &Scroll::_leftOrBottomClicked);

	_topTexture = Unigine::Texture::create();
	_topTexture->load(Settings::get().getTexturesPath("topArrow.png"));

	_sliderBox = Unigine::WidgetHBox::create();
	_sliderBox->setBackground(true);
	_sliderBox->setBackgroundTexture(Settings::get().getTexturesPath("sliderBackground.png"));

	_coreSlider = Unigine::WidgetSlider::create();
	_applySliderProperties(_coreSlider);
	_sliderBox->addChild(_coreSlider);

	_bottomTexture = Unigine::Texture::create();
	_bottomTexture->load(Settings::get().getTexturesPath("bottomArrow.png"));

	_sliderSprite = Unigine::WidgetSprite::create();

	_sliderTexture = Unigine::Texture::create();
	_sliderTexture->load(Settings::get().getTexturesPath("bottomArrow.png"));

	_topSprite->setRender(_topTexture);

	_bottomSprite->setRender(_bottomTexture);
	_sliderSprite->setRender(_sliderTexture);

	_widget->addChild(_topSprite);
	
	_widget->addChild(_sliderSprite, Unigine::Gui::ALIGN_OVERLAP | Unigine::Gui::ALIGN_FIXED);

	_widget->addChild(_sliderBox);
	_widget->addChild(_bottomSprite);
}

void Scroll::resize(int32_t width, int32_t height)
{
	WidgetBase::resize(width, height);

	int32_t availableHeight = height;

	int32_t textureWidth = _topTexture->getWidth(), textureHeight = _topTexture->getHeight();

	float aspectRatio = static_cast<float>(textureWidth) / textureHeight;

	_topHeight = static_cast<int32_t>(_widget->getWidth() * aspectRatio);
	availableHeight -= _topHeight;

	_topSprite->setWidth(_widget->getWidth());
	_topSprite->setHeight(_topHeight);

	textureWidth = _bottomTexture->getWidth(), textureHeight = _bottomTexture->getHeight();

	aspectRatio = static_cast<float>(textureWidth) / textureHeight;

	_bottomHeight = static_cast<int32_t>(_widget->getWidth() * aspectRatio);
	availableHeight -= _bottomHeight;

	_bottomSprite->setWidth(_widget->getWidth());
	_bottomSprite->setHeight(_bottomHeight);

	_sliderSprite->setWidth(_widget->getWidth());
	_sliderSprite->setHeight(_bottomHeight);

	for (int32_t i = _sliderBox->getNumChildren() - 1; i >= 1; i--)
		_sliderBox->removeChild(_sliderBox->getChild(i));

	_coreSlider->setHeight(availableHeight);
	_coreSlider->setButtonHeight(_bottomHeight);

	for (int32_t i = 1; i < ceil(static_cast<float>(width) / 15); i++)
	{
		auto slider = Unigine::WidgetSlider::create();
		_applySliderProperties(slider);
		slider->setHeight(availableHeight);
		slider->setPositionX(i * 16);
		slider->setButtonHeight(_bottomHeight);
		_sliderBox->addChild(slider, Unigine::Gui::ALIGN_OVERLAP);
	}

	_setSliderRange();

	_sliderBox->setWidth(_widget->getWidth());
	_sliderBox->setHeight(availableHeight);

	_sliderSize = _bottomHeight;
	_sliderSpace = availableHeight - _sliderSize;

	_sliderChanged(_coreSlider);
}

void noMoPi::Scroll::setValue(int32_t value)
{
	_coreSlider->setValue(value);
}

Scroll* noMoPi::Scroll::setRange(int32_t minValue, int32_t maxValue)
{
	_minValue = minValue;
	_maxValue = maxValue;

	_setSliderRange();
	
	return this;
}

Scroll* noMoPi::Scroll::setIsReversed(bool isReversed)
{
	_isReversed = isReversed;
	_setSliderRange();
	
	return this;
}

void noMoPi::Scroll::_sliderChanged(const Unigine::WidgetPtr& widget)
{
	auto widgetSlider = Unigine::dynamic_ptr_cast<Unigine::WidgetSlider>(widget);

	Unigine::Log::message("Slider changed %d\n", widgetSlider->getValue());
	
	for (int32_t i = 0; i < _sliderBox->getNumChildren(); i++)
	{
		auto child = _sliderBox->getChild(i);
		auto slider = Unigine::dynamic_ptr_cast<Unigine::WidgetSlider>(child);
		if (slider)
			slider->setValue(widgetSlider->getValue());
	}

	const int32_t sliderRange = _maxValue - _minValue;
	Unigine::Log::message("Slider range, slider value, slider space: %d %d %d\n", sliderRange, widgetSlider->getValue(), _sliderSpace);
	int32_t position = _topHeight + static_cast<float>(_sliderSpace) * (_isReversed ? widgetSlider->getValue() : _maxValue - widgetSlider->getValue()) / sliderRange;

	_sliderSprite->setPositionY(position);
}

void noMoPi::Scroll::_topOrLeftClicked(const Unigine::WidgetPtr& widget, int32_t mouse)
{
	for (int32_t i = 0; i < _sliderBox->getNumChildren(); i++)
	{
		auto child = _sliderBox->getChild(i);
		auto slider = Unigine::dynamic_ptr_cast<Unigine::WidgetSlider>(child);
		if (slider)
		{
			slider->setValue(slider->getValue() + (slider->getMaxValue() > slider->getMinValue() ? 1 : -1));
			break;
		}
	}
}

void noMoPi::Scroll::_leftOrBottomClicked(const Unigine::WidgetPtr& widget, int32_t mouse)
{
	for (int32_t i = 0; i < _sliderBox->getNumChildren(); i++)
	{
		auto child = _sliderBox->getChild(i);
		auto slider = Unigine::dynamic_ptr_cast<Unigine::WidgetSlider>(child);
		if (slider)
		{
			slider->setValue(slider->getValue() + (slider->getMaxValue() > slider->getMinValue() ? -1 : 1));
			break;
		}
	}
}

void noMoPi::Scroll::_applySliderProperties(const Unigine::WidgetSliderPtr& slider)
{
	slider->setOrientation(0);
	slider->setButtonHeight(100);
	slider->getEventChanged().connect(ec, this, &Scroll::_sliderChanged);
	// make the slider transparent
	//slider->setBackgroundColor(Unigine::Math::vec4(0, 0, 0, 0));
	//slider->setButtonColor(Unigine::Math::vec4(0, 0, 0, 0));
}

void noMoPi::Scroll::_setSliderRange()
{
	for (int32_t i = 0; i < _sliderBox->getNumChildren(); i++)
	{
		auto child = _sliderBox->getChild(i);
		auto slider = Unigine::dynamic_ptr_cast<Unigine::WidgetSlider>(child);
		Unigine::Log::message("chuj\n");
		if (slider)
		{
			slider->setMinValue(_isReversed ? _maxValue : _minValue);
			slider->setMaxValue(_isReversed ? _minValue : _maxValue);
		}
	}
}

noMoPi::ScrollBox::ScrollBox(const ScaleSettings& scaleSettings) : WidgetContainer(scaleSettings)
{
	Unigine::WidgetHBoxPtr hbox = Unigine::WidgetHBox::create();
	hbox->setBackground(true);
	hbox->setBackgroundColor(Unigine::Math::vec4(0.5f, 1.f, 1.f, 1.f));
	
	_widget = hbox;

	Unigine::WidgetScrollBoxPtr scrollbox = Unigine::WidgetScrollBox::create();
	scrollbox->setBorder(0);
	scrollbox->setHScrollHidden(Unigine::WidgetScrollBox::ALWAYS_HIDE_NO_BOUNDS);
	scrollbox->setVScrollHidden(Unigine::WidgetScrollBox::ALWAYS_HIDE_NO_BOUNDS);
	scrollbox->setBackground(true);
	scrollbox->setBackgroundColor(Unigine::Math::vec4(0.5f, 0.5f, 1.f, 1.f));

	_containterWidget = scrollbox;

	hbox->addChild(scrollbox);

	_verticalScroll = Scroll::create();
	_verticalScroll->setIsReversed(true);
	hbox->addChild(*_verticalScroll);

	_containterWidget->getEventChanged().connect(ec, this, &ScrollBox::_onVerticalScrollBoxChanged);
	_verticalScroll->getEventChanged().connect(ec, this, &ScrollBox::_onVerticalScrollChanged);
}

void ScrollBox::resize(int32_t width, int32_t height)
{
	WidgetBase::resize(width, height);
	
	_containterWidget->setWidth(width - 50);
	_containterWidget->setHeight(height);

	_verticalScroll->resize(50, height);

	_resizeChildren();
}

void ScrollBox::addChild(const std::shared_ptr<WidgetBase>& widget)
{
	WidgetContainer::addChild(widget);

	_resizeChildren();
}

ScrollBox* ScrollBox::setVisibleItemCount(int32_t itemCount)
{
	_itemCount = itemCount;

	return this;
}

void ScrollBox::_resizeChildren()
{
	Unigine::Log::message("height: %d\n", getHeight());
	
	int32_t height = getInnerHeight();
	int32_t scaledSpacing = static_cast<int32_t>(height * _spacing);
	height -= (_itemCount - 1) * scaledSpacing;
	_scaledItemHeight = height / _itemCount;

	for (auto& child : _childWidgets)
		child->resize(_containterWidget->getWidth(), _scaledItemHeight);

	for (auto& child : _spacers)
		child->setHeight(scaledSpacing);
}

void noMoPi::ScrollBox::_onVerticalScrollBoxChanged(const Unigine::WidgetPtr& widget)
{
	Unigine::WidgetScrollBoxPtr scrollBox = Unigine::dynamic_ptr_cast<Unigine::WidgetScrollBox>(_containterWidget);

	//scrollBox->getVScrollObjectSize() == scrollBox->getVScrollValue() + scrollBox->getVScrollFrameSize();

	int32_t scrollPercent = scrollBox->getVScrollValue() * 100 / (scrollBox->getVScrollObjectSize() - scrollBox->getVScrollFrameSize());

	_verticalScroll->setValue(scrollPercent);
}

void noMoPi::ScrollBox::_onVerticalScrollChanged(const Unigine::WidgetPtr& widget)
{
	Unigine::WidgetScrollBoxPtr scrollBox = Unigine::dynamic_ptr_cast<Unigine::WidgetScrollBox>(_containterWidget);

	int32_t scrollValue = (scrollBox->getVScrollObjectSize() - scrollBox->getVScrollFrameSize()) * _verticalScroll->getValue() / 100;

	scrollBox->setVScrollValue(scrollValue);
}
