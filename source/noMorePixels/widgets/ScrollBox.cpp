#include "ScrollBox.h"

using namespace noMoPi;

noMoPi::ScrollBox::ScrollBox(const ScaleSettings& scaleSettings) : WidgetContainerBase(scaleSettings)
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
	WidgetContainerBase::addChild(widget);

	_resizeChildren();
}

ScrollBox* ScrollBox::setVisibleItemCount(int32_t itemCount)
{
	_itemCount = itemCount;

	return this;
}

void ScrollBox::_resizeChildren()
{
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

