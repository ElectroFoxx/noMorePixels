#include "WidgetContainerBase.h"

#include "../Settings.h"
#include "Interactive.h"

using namespace noMoPi;

void WidgetContainerBase::addChild(const std::shared_ptr<WidgetBase>& widget)
{
	widget->_setGui(_gui);

	// Interactive vidgets support events like onClicked, onEnter, ect
	if (Interactive* interactive = dynamic_cast<Interactive*>(widget.get()))
	{
		interactive->setGui(_gui);
		interactive->attach(widget);
	}

	if (_childWidgets.size() >= 1)
	{
		Unigine::WidgetVBoxPtr spacer = Unigine::WidgetVBox::create();
		spacer->setLifetime(Unigine::Widget::LIFETIME::LIFETIME_MANUAL);
		_spacers.push_back(spacer);
		_rootWidget->addChild(spacer);
	}

	_childWidgets.push_back(widget);

	if (_containterWidget && widget->getWidget())
		_containterWidget->addChild(widget->getWidget());
}

const std::shared_ptr<WidgetBase>& WidgetContainerBase::getChild(int32_t index) const
{
	return _childWidgets.at(index);
}

void WidgetContainerBase::clear()
{
	for (std::shared_ptr<WidgetBase>& child : _childWidgets)
	{
		if (WidgetContainerBase* widget = dynamic_cast<WidgetContainerBase*>(child.get()))
			widget->clear();
	}

	_childWidgets.clear();
	_spacers.clear();

	while (_containterWidget->getNumChildren() > 0)
	{
		Unigine::WidgetPtr widget = _containterWidget->getChild(0);
		_containterWidget->removeChild(widget);
	}
}

WidgetContainerBase* WidgetContainerBase::setBackgroundEnabled(bool hasBackground)
{
	if (Unigine::WidgetVBoxPtr box = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_rootWidget))
		box->setBackground(hasBackground);

	return this;
}

WidgetContainerBase* WidgetContainerBase::setBackgroundColor(float r, float g, float b, float a)
{
	if (Unigine::WidgetVBoxPtr box = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_rootWidget))
		box->setBackgroundColor(Unigine::Math::vec4(r, b, g, a));

	return this;
}

WidgetContainerBase* WidgetContainerBase::setBackgroundColor(const Unigine::Math::vec4& color)
{
	if (Unigine::WidgetVBoxPtr box = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_rootWidget))
		box->setBackgroundColor(color);

	return this;
}

WidgetContainerBase* WidgetContainerBase::setBackgroundColor(int32_t r, int32_t g, int32_t b, int32_t a)
{
	return setBackgroundColor(static_cast<float>(r) / 255, static_cast<float>(g) / 255, static_cast<float>(b) / 255, static_cast<float>(a) / 255);
}

WidgetContainerBase* WidgetContainerBase::setBackgroundTexture(const Unigine::String& texture)
{
	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_rootWidget))
	{
		vbox->setBackgroundTexture(Settings::get().getTexturesPath(texture));
	}

	return this;
}

WidgetContainerBase* WidgetContainerBase::setBackgroundTextureFiltering(int32_t filtering)
{
	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_rootWidget))
	{
		vbox->setBackgroundCustomFilterEnabled(true);
		vbox->setBackgroundCustomFilter(filtering);
	}

	return this;
}

void WidgetContainerBase::resize(int32_t width, int32_t height)
{
	WidgetBase::resize(width, height);

	_calculatePadding();
	_calculateSpacing();

	_resizeChildren();
}

void WidgetContainerBase::_resizeChildren()
{
	float totalFillWeight = 0.f;
	float totalProportionalWeight = 0.f;
	float totalRatioWeight = 0.f;

	// Calculate total weights
	for (auto& child : _childWidgets)
	{
		const ScaleSettings& scaleSettings = child->getScaleSettings();
		
		if (scaleSettings.scaleType == ScaleType::Fill)
			totalFillWeight += scaleSettings.scaleFactor;
		else if (scaleSettings.scaleType == ScaleType::Proportional)
			totalProportionalWeight += scaleSettings.scaleFactor;
		else if (scaleSettings.scaleType == ScaleType::Ratio)
			totalRatioWeight += scaleSettings.scaleFactor;
	}

	int32_t parentWidth = getInnerWidth();
	int32_t parentHeight = getInnerHeight();

	const bool isHorizontal = _rootWidget->getType() == Unigine::Widget::TYPE::WIDGET_HBOX;

	// Calculate available space after spacing
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

	const int32_t freeSpace = isHorizontal ? parentWidth : parentHeight;
	const int32_t oppositeSize = isHorizontal ? parentHeight : parentWidth;

	int32_t freeSpaceForProportionalWidgets = static_cast<int32_t>(freeSpace * totalProportionalWeight);
	int32_t freeSpaceForRatioWidgets = static_cast<int32_t>(oppositeSize * totalRatioWeight);
	int32_t freeSpaceForFillWidgets = freeSpace - freeSpaceForProportionalWidgets - freeSpaceForRatioWidgets;

	float fillWidgetsRemainder = 0.f, proportionalWidgetsRemainder = 0.f, ratioWidgetsRemainder = 0.f;
	
	// Resize all Fill and Proportional widgets
	for (auto& child : _childWidgets)
	{
		const ScaleType& scaleType = child->getScaleSettings().scaleType;
		const float scaleFactor = child->getScaleSettings().scaleFactor;

		int32_t childSize = 0;

		if (scaleType == ScaleType::Fill)
		{
			const float exactSize = freeSpaceForFillWidgets * (scaleFactor / totalFillWeight);
			const int32_t roundedSize = static_cast<int32_t>(exactSize);

			fillWidgetsRemainder += exactSize - roundedSize;

			int32_t adjustment = 0;

			if (fillWidgetsRemainder >= 0.99975f)
			{
				adjustment = 1;
				fillWidgetsRemainder -= 0.99975f;
			}

			childSize = roundedSize + adjustment;
		}
		else if (scaleType == ScaleType::Proportional)
		{
			const float exactSize = freeSpace * scaleFactor;
			const int32_t roundedSize = static_cast<int32_t>(exactSize);

			proportionalWidgetsRemainder += exactSize - roundedSize;

			int32_t adjustment = 0;

			if (proportionalWidgetsRemainder >= 0.99975f)
			{
				adjustment = 1;
				proportionalWidgetsRemainder -= 1.f;
			}

			childSize = roundedSize + adjustment;
		}
		else if (scaleType == ScaleType::Ratio)
		{
			const float exactSize = freeSpaceForRatioWidgets * (scaleFactor / totalRatioWeight);
			const int32_t roundedSize = static_cast<int32_t>(exactSize);

			ratioWidgetsRemainder += exactSize - roundedSize;

			int32_t adjustment = 0;

			if (ratioWidgetsRemainder >= 0.99975f)
			{
				adjustment = 1;
				ratioWidgetsRemainder -= 1.f;
			}

			childSize = roundedSize + adjustment;
		}

		if (isHorizontal)
			child->resize(childSize, parentHeight);
		else
			child->resize(parentWidth, childSize);
	}
}

void noMoPi::WidgetContainerBase::_setGui(const Unigine::Ptr<Unigine::Gui>& gui)
{
	WidgetBase::_setGui(gui);
	for (auto& child : _childWidgets)
		child->_setGui(gui);
}

int32_t WidgetContainerBase::getInnerHeight() const
{
	int32_t verticalPadding = _isPaddingEqual ? _paddingInPixels.min() * 2 : _paddingInPixels[std::to_underlying(PaddingIndex::Top)] + _paddingInPixels[std::to_underlying(PaddingIndex::Bottom)];

	return getHeight() - verticalPadding;
}

int32_t WidgetContainerBase::getInnerWidth() const
{
	int32_t horizontalPadding = _isPaddingEqual ? _paddingInPixels.min() * 2 : _paddingInPixels[std::to_underlying(PaddingIndex::Left)] + _paddingInPixels[std::to_underlying(PaddingIndex::Right)];

	return getWidth() - horizontalPadding;
}

WidgetContainerBase* WidgetContainerBase::setPadding(float top, float bottom, float left, float right)
{
	_padding = Unigine::Math::vec4(top, bottom, left, right);
	return this;
}

WidgetContainerBase* WidgetContainerBase::setPaddingEqual(bool isPaddingEqual, bool useSmallestPadding)
{
	_isPaddingEqual = isPaddingEqual;
	_useSmallestPadding = useSmallestPadding;
	return this;
}

WidgetContainerBase* WidgetContainerBase::setSpacing(float spacing, bool ignorePadding)
{
	_spacing = spacing;
	_ignorePadding = ignorePadding;
	return this;
}

void WidgetContainerBase::_calculatePadding()
{
	const int32_t width = getWidth();
	const int32_t height = getHeight();

	_paddingInPixels[std::to_underlying(PaddingIndex::Left)] = static_cast<int32_t>(width * _padding[std::to_underlying(PaddingIndex::Left)]);
	_paddingInPixels[std::to_underlying(PaddingIndex::Right)] = static_cast<int32_t>(width * _padding[std::to_underlying(PaddingIndex::Right)]);
	_paddingInPixels[std::to_underlying(PaddingIndex::Top)] = static_cast<int32_t>(height * _padding[std::to_underlying(PaddingIndex::Top)]);
	_paddingInPixels[std::to_underlying(PaddingIndex::Bottom)] = static_cast<int32_t>(height * _padding[std::to_underlying(PaddingIndex::Bottom)]);

	if (Unigine::WidgetVBoxPtr box = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_rootWidget))
	{
		if (_isPaddingEqual)
		{
			const int32_t equalPadding = _useSmallestPadding ? _paddingInPixels.min() : _paddingInPixels.max();
			box->setPadding(equalPadding, equalPadding, equalPadding, equalPadding);
		}
		else
			box->setPadding(
				_paddingInPixels[std::to_underlying(PaddingIndex::Left)],
				_paddingInPixels[std::to_underlying(PaddingIndex::Right)],
				_paddingInPixels[std::to_underlying(PaddingIndex::Top)],
				_paddingInPixels[std::to_underlying(PaddingIndex::Bottom)]);
	}
}

void WidgetContainerBase::_calculateSpacing()
{
	const int32_t width = _ignorePadding ? getWidth() : getInnerWidth();
	const int32_t height = _ignorePadding ? getHeight() : getInnerHeight();

	if (_rootWidget->getType() == Unigine::Widget::TYPE::WIDGET_HBOX)
	{
		_scaledSpacing = static_cast<int32_t>(width * _spacing);
		for (auto& spacer : _spacers)
		{
			spacer->setWidth(_scaledSpacing);
			spacer->setHeight(height);
		}

	}
	else if (_rootWidget->getType() == Unigine::Widget::TYPE::WIDGET_VBOX || _rootWidget->getType() == Unigine::Widget::TYPE::WIDGET_SCROLL_BOX)
	{
		_scaledSpacing = static_cast<int32_t>(height * _spacing);
		for (auto& spacer : _spacers)
		{
			spacer->setWidth(width);
			spacer->setHeight(_scaledSpacing);
		}
	}
}

void WidgetContainerBase::translate()
{
	for (auto& child : _childWidgets)
		child->translate();
}

void WidgetContainerBase::tick(float deltaTime)
{
	for (auto& child : _childWidgets)
		child->tick(deltaTime);
}