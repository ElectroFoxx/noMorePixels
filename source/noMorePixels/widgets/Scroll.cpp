#include "Scroll.h"

#include "../Settings.h"

using namespace noMoPi;

noMoPi::Scroll::Scroll(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	_rootWidget = Unigine::WidgetVBox::create();
	_rootWidget->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_rootWidget)->setBackgroundColor(Unigine::Math::vec4_blue);
	Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_rootWidget)->setBackground(1);

	_topSprite = Unigine::WidgetSprite::create();
	_topSprite->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	_topSprite->getEventClicked().connect(ec, this, &Scroll::_topOrLeftClicked);

	static Unigine::EventConnections econn;

	_bottomSprite = Unigine::WidgetSprite::create();
	_bottomSprite->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	_bottomSprite->getEventClicked().connect(ec, this, &Scroll::_leftOrBottomClicked);

	_topTexture = Unigine::Texture::create();
	_topTexture->load(Settings::get().getTexturesPath("topArrow.png"));

	_sliderBox = Unigine::WidgetHBox::create();
	_sliderBox->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	_sliderBox->setBackground(true);
	_sliderBox->setBackgroundTexture(Settings::get().getTexturesPath("sliderBackground.png"));

	_coreSlider = Unigine::WidgetSlider::create();
	_coreSlider->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	_applySliderProperties(_coreSlider);
	_sliderBox->addChild(_coreSlider);

	_bottomTexture = Unigine::Texture::create();
	_bottomTexture->load(Settings::get().getTexturesPath("bottomArrow.png"));

	_sliderSprite = Unigine::WidgetSprite::create();
	_sliderSprite->setLifetime(Unigine::Widget::LIFETIME_MANUAL);

	_sliderTexture = Unigine::Texture::create();
	_sliderTexture->load(Settings::get().getTexturesPath("bottomArrow.png"));

	_topSprite->setRender(_topTexture);

	_bottomSprite->setRender(_bottomTexture);
	_sliderSprite->setRender(_sliderTexture);

	_rootWidget->addChild(_topSprite);

	_rootWidget->addChild(_sliderSprite, Unigine::Gui::ALIGN_OVERLAP | Unigine::Gui::ALIGN_FIXED);

	_rootWidget->addChild(_sliderBox);
	_rootWidget->addChild(_bottomSprite);
}

void Scroll::resize(int32_t width, int32_t height)
{
	WidgetBase::resize(width, height);

	int32_t availableHeight = height;

	int32_t textureWidth = _topTexture->getWidth(), textureHeight = _topTexture->getHeight();

	float aspectRatio = static_cast<float>(textureWidth) / textureHeight;

	_topHeight = static_cast<int32_t>(_rootWidget->getWidth() * aspectRatio);
	availableHeight -= _topHeight;

	_topSprite->setWidth(_rootWidget->getWidth());
	_topSprite->setHeight(_topHeight);

	textureWidth = _bottomTexture->getWidth(), textureHeight = _bottomTexture->getHeight();

	aspectRatio = static_cast<float>(textureWidth) / textureHeight;

	_bottomHeight = static_cast<int32_t>(_rootWidget->getWidth() * aspectRatio);
	availableHeight -= _bottomHeight;

	_bottomSprite->setWidth(_rootWidget->getWidth());
	_bottomSprite->setHeight(_bottomHeight);

	_sliderSprite->setWidth(_rootWidget->getWidth());
	_sliderSprite->setHeight(_bottomHeight);

	for (int32_t i = _sliderBox->getNumChildren() - 1; i >= 1; i--)
	{
		Unigine::WidgetPtr child = _sliderBox->getChild(i);
		_sliderBox->removeChild(child);
		child.deleteLater();
	}

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

	_sliderBox->setWidth(_rootWidget->getWidth());
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

noMoPi::Scroll::~Scroll()
{
	_sliderBox.deleteLater();
	_coreSlider.deleteLater();
	_bottomSprite.deleteLater();
	_topSprite.deleteLater();
	_sliderSprite.deleteLater();
}

void noMoPi::Scroll::_sliderChanged(const Unigine::WidgetPtr& widget)
{
	auto widgetSlider = Unigine::dynamic_ptr_cast<Unigine::WidgetSlider>(widget);

	for (int32_t i = 0; i < _sliderBox->getNumChildren(); i++)
	{
		auto child = _sliderBox->getChild(i);
		auto slider = Unigine::dynamic_ptr_cast<Unigine::WidgetSlider>(child);
		if (slider)
			slider->setValue(widgetSlider->getValue());
	}

	const int32_t sliderRange = _maxValue - _minValue;

	int32_t position = static_cast<int32_t>(_topHeight + static_cast<float>(_sliderSpace) * (_isReversed ? widgetSlider->getValue() : _maxValue - widgetSlider->getValue()) / sliderRange);

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
	slider->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	slider->setOrientation(0);
	slider->setButtonHeight(100);
	slider->getEventChanged().connect(ec, this, &Scroll::_sliderChanged);
	// make the slider transparent
	slider->setBackgroundColor(Unigine::Math::vec4(0, 0, 0, 0));
	slider->setButtonColor(Unigine::Math::vec4(0, 0, 0, 0));
}

void noMoPi::Scroll::_setSliderRange()
{
	for (int32_t i = 0; i < _sliderBox->getNumChildren(); i++)
	{
		auto child = _sliderBox->getChild(i);
		auto slider = Unigine::dynamic_ptr_cast<Unigine::WidgetSlider>(child);

		if (slider)
		{
			slider->setMinValue(_isReversed ? _maxValue : _minValue);
			slider->setMaxValue(_isReversed ? _minValue : _maxValue);
		}
	}
}
