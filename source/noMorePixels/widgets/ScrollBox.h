#pragma once

#include "../base/WidgetContainerBase.h"
#include "Scroll.h"

namespace noMoPi
{
	class ScrollBox : public WidgetContainerBase
	{
	public:
		ScrollBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<ScrollBox> create() { return std::make_shared<ScrollBox>(ScaleSettings()); }
		static std::shared_ptr<ScrollBox> create(const ScaleSettings& scaleSettings) { return std::make_shared<ScrollBox>(scaleSettings); }

		virtual void resize(int32_t width, int32_t height);
		virtual void addChild(const std::shared_ptr<WidgetBase>& widget);
		ScrollBox* setVisibleItemCount(int32_t itemCount);
		ScrollBox* setScrollScale(float scale);
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

		float _scrollScale = 0.1f;
	};
}
