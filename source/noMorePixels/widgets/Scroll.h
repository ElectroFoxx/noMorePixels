#pragma once

#include "../base/WidgetBase.h"

namespace noMoPi
{
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
}
