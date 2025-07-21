#include "noMorePixels.h"

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

void noMoPi::UI::addDictionary(const char* dictionary)
{
	_gui->addDictionary(Settings::get().getLocalizationPath(dictionary), "en");
}

void WidgetBase::resize(int32_t width, int32_t height)
{
	_widget->setWidth(width);
	_widget->setHeight(height);
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
		parentWidth -= spacing * (static_cast<int32_t>(_childWidgets.size()) + 1);
	}
	else
	{
		uint32_t spacing = static_cast<int32_t>(_spacing * parentHeight);
		parentHeight -= spacing * (static_cast<int32_t>(_childWidgets.size()) + 1);
	}

	int32_t spaceLeft = isHorizontal ? parentWidth : parentHeight;
	for (auto& child : _childWidgets)
	{
		if (child->getScaleSettings().scaleType == ScaleType::Fill)
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
	}
}

int32_t noMoPi::WidgetContainer::getInnerHeight() const
{
	float verticalPadding = _padding[0] + _padding[1];

	return static_cast<int32_t>(getHeight() * (1.f - verticalPadding));
}

int32_t noMoPi::WidgetContainer::getInnerWidth() const
{
	float horizontalPadding = _padding[2] + _padding[3];

	return static_cast<int32_t>(getWidth() * (1.f - horizontalPadding));
}

WidgetContainer* WidgetContainer::setPadding(float top, float bottom, float left, float right)
{
	_padding = Unigine::Math::vec4(top, bottom, left, right);
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

	const int32_t leftPadding = static_cast<int32_t>(width * _padding[std::to_underlying(Padding::Left)]);
	const int32_t rightPadding = static_cast<int32_t>(width * _padding[std::to_underlying(Padding::Right)]);
	const int32_t topPadding = static_cast<int32_t>(height * _padding[std::to_underlying(Padding::Top)]);
	const int32_t bottomPadding = static_cast<int32_t>(height * _padding[std::to_underlying(Padding::Bottom)]);

	if (Unigine::WidgetVBoxPtr box = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
		box->setPadding(leftPadding, rightPadding, topPadding, bottomPadding);
}

void noMoPi::WidgetContainer::_calculateSpacing()
{
	const int32_t width = getInnerWidth();
	const int32_t height = getInnerHeight();

	if (Unigine::WidgetHBoxPtr hbox = Unigine::dynamic_ptr_cast<Unigine::WidgetHBox>(_widget))
	{
		int32_t scaledSpacing = static_cast<int32_t>(width * _spacing);
		hbox->setSpace(scaledSpacing, 0);
	}
	else if (Unigine::WidgetVBoxPtr vbox = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget))
	{
		int32_t scaledSpacing = static_cast<int32_t>(height * _spacing);
		vbox->setSpace(0, scaledSpacing);
	}
}

HBox::HBox(const ScaleSettings& scaleSettings) : WidgetContainer(scaleSettings)
{
	_widget = Unigine::WidgetHBox::create();

	if (Unigine::WidgetHBoxPtr hbox = Unigine::static_ptr_cast<Unigine::WidgetHBox>(_widget))
		hbox->setBackgroundTexture(".noMorePixels/textures/white.png");
}

void noMoPi::WidgetContainer::addChild(const std::shared_ptr<WidgetBase>& widget)
{
	Unigine::GuiPtr gui = _widget->getGui();
	widget->setGui(gui);

	_childWidgets.push_back(widget);

	if (_widget && widget->getWidget())
		_widget->addChild(widget->getWidget());
}

WidgetContainer* WidgetContainer::setBackground(bool hasBackground)
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

VBox::VBox(const ScaleSettings& scaleSettings) : WidgetContainer(scaleSettings)
{
	_widget = Unigine::WidgetHBox::create();

	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
		vbox->setBackgroundTexture(Settings::get().getWhiteBackground());
}

Label::Label(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	_widget = Unigine::WidgetHBox::create();
	_label = Unigine::WidgetLabel::create();
	_widget->addChild(_label);
}

Label* Label::setText(const char* text)
{
	_newLineCount = 0;
	for (const char* c = text; *c; c++)
	{
		if (*c == '\n')
			_newLineCount++;
	}
	
	_targetText = text;
	
	_label->setText(_targetText);
	
	return this;
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

void Label::resize(int32_t width, int32_t height)
{
	WidgetBase::resize(width, height);

	_calculateMaxFontParams();

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

Unigine::String Settings::getWhiteBackground() const
{
	return _rootFolder + _texturesFolder + _whiteBackground;
}

void UI::translate()
{
	_rootWidget->translate();
}

void WidgetContainer::translate()
{
	for (auto& child : _childWidgets)
		child->translate();
}

void Label::translate()
{
	Unigine::GuiPtr gui = _widget->getGui();

	Unigine::Log::message(gui->translate("Source string"));


}