#include "WidgetContainerBase.h"

#include "../Settings.h"
#include "Interactive.h"

using namespace noMoPi;

void noMoPi::WidgetContainerBase::addChild(const std::shared_ptr<WidgetBase>& widget)
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

const std::shared_ptr<WidgetBase>& noMoPi::WidgetContainerBase::getChild(int32_t index) const
{
	return _childWidgets.at(index);
}

void noMoPi::WidgetContainerBase::clear()
{
	for (auto& child : _childWidgets)
	{
		if (WidgetContainerBase* widget = dynamic_cast<WidgetContainerBase*>(child.get()))
			widget->clear();
	}

	_childWidgets.clear();
	_spacers.clear();

	while (_widget->getNumChildren() > 0)
	{
		Unigine::WidgetPtr widget = _widget->getChild(0);
		_widget->removeChild(widget);
	}
}

WidgetContainerBase* WidgetContainerBase::setBackgroundEnabled(bool hasBackground)
{
	if (Unigine::WidgetVBoxPtr box = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget))
		box->setBackground(hasBackground);

	return this;
}

WidgetContainerBase* WidgetContainerBase::setBackgroundColor(float r, float g, float b, float a)
{
	if (Unigine::WidgetVBoxPtr box = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget))
		box->setBackgroundColor(Unigine::Math::vec4(r, b, g, a));

	return this;
}

WidgetContainerBase* noMoPi::WidgetContainerBase::setBackgroundColor(const Unigine::Math::vec4& color)
{
	if (Unigine::WidgetVBoxPtr box = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(_widget))
		box->setBackgroundColor(color);

	return this;
}

WidgetContainerBase* WidgetContainerBase::setBackgroundColor(int32_t r, int32_t g, int32_t b, int32_t a)
{
	return setBackgroundColor(static_cast<float>(r) / 255, static_cast<float>(g) / 255, static_cast<float>(b) / 255, static_cast<float>(a) / 255);
}

WidgetContainerBase* noMoPi::WidgetContainerBase::setBackgroundTexture(const Unigine::String& texture)
{
	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
	{
		vbox->setBackgroundTexture(Settings::get().getTexturesPath(texture));
	}

	return this;
}

WidgetContainerBase* noMoPi::WidgetContainerBase::setBackgroundTextureFiltering(int32_t filtering)
{
	if (Unigine::WidgetVBoxPtr vbox = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
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

void noMoPi::WidgetContainerBase::_resizeChildren()
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

int32_t noMoPi::WidgetContainerBase::getInnerHeight() const
{
	int32_t verticalPadding = _isPaddingEqual ? _paddingInPixels.min() * 2 : _paddingInPixels[std::to_underlying(PaddingIndex::Top)] + _paddingInPixels[std::to_underlying(PaddingIndex::Bottom)];

	return getHeight() - verticalPadding;
}

int32_t noMoPi::WidgetContainerBase::getInnerWidth() const
{
	int32_t horizontalPadding = _isPaddingEqual ? _paddingInPixels.min() * 2 : _paddingInPixels[std::to_underlying(PaddingIndex::Left)] + _paddingInPixels[std::to_underlying(PaddingIndex::Right)];

	return getWidth() - horizontalPadding;
}

WidgetContainerBase* WidgetContainerBase::setPadding(float top, float bottom, float left, float right)
{
	_padding = Unigine::Math::vec4(top, bottom, left, right);
	return this;
}

WidgetContainerBase* noMoPi::WidgetContainerBase::setPaddingEqual(bool isPaddingEqual)
{
	_isPaddingEqual = isPaddingEqual;
	return this;
}

WidgetContainerBase* WidgetContainerBase::setSpacing(float spacing, bool ignorePadding)
{
	_spacing = spacing;
	_ignorePadding = ignorePadding;
	return this;
}

void noMoPi::WidgetContainerBase::_calculatePadding()
{
	const int32_t width = getWidth();
	const int32_t height = getHeight();

	_paddingInPixels[std::to_underlying(PaddingIndex::Left)] = static_cast<int32_t>(width * _padding[std::to_underlying(PaddingIndex::Left)]);
	_paddingInPixels[std::to_underlying(PaddingIndex::Right)] = static_cast<int32_t>(width * _padding[std::to_underlying(PaddingIndex::Right)]);
	_paddingInPixels[std::to_underlying(PaddingIndex::Top)] = static_cast<int32_t>(height * _padding[std::to_underlying(PaddingIndex::Top)]);
	_paddingInPixels[std::to_underlying(PaddingIndex::Bottom)] = static_cast<int32_t>(height * _padding[std::to_underlying(PaddingIndex::Bottom)]);

	if (Unigine::WidgetVBoxPtr box = Unigine::static_ptr_cast<Unigine::WidgetVBox>(_widget))
	{
		if (_isPaddingEqual)
		{
			int32_t smallestPadding = _paddingInPixels.min();
			box->setPadding(smallestPadding, smallestPadding, smallestPadding, smallestPadding);
		}
		else
			box->setPadding(
				_paddingInPixels[std::to_underlying(PaddingIndex::Left)],
				_paddingInPixels[std::to_underlying(PaddingIndex::Right)],
				_paddingInPixels[std::to_underlying(PaddingIndex::Top)],
				_paddingInPixels[std::to_underlying(PaddingIndex::Bottom)]);
	}
}

void noMoPi::WidgetContainerBase::_calculateSpacing()
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