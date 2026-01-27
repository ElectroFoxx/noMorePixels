#pragma once

#include "WidgetBase.h"

namespace noMoPi
{
	class WidgetContainerBase : public WidgetBase
	{
	public:
		WidgetContainerBase(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings) {}

		virtual void resize(int32_t width, int32_t height);
		virtual void translate();
		virtual void tick(float deltaTime);
		virtual int32_t getInnerHeight() const;
		virtual int32_t getInnerWidth() const;

		virtual void addChild(const std::shared_ptr<WidgetBase>& widget);
		const std::shared_ptr<WidgetBase>& getChild(int32_t index) const;
		const int32_t getNumChildren() const { return static_cast<int32_t>(_childWidgets.size()); }
		virtual void clear();

		WidgetContainerBase* setPadding(float top, float bottom, float left, float right);
		WidgetContainerBase* setPaddingEqual(bool isPaddingEqual, bool useSmallestPadding = true);
		WidgetContainerBase* setSpacing(float spacing, bool ignorePadding = false);
		WidgetContainerBase* setBackgroundEnabled(bool hasBackground);
		WidgetContainerBase* setBackgroundColor(float r, float g, float b, float a = 1.f);
		WidgetContainerBase* setBackgroundColor(const Unigine::Math::vec4& color);
		WidgetContainerBase* setBackgroundColor(int32_t r, int32_t g, int32_t b, int32_t a = 255);
		WidgetContainerBase* setBackgroundTexture(const Unigine::String& texture);
		WidgetContainerBase* setBackgroundTextureFiltering(int32_t filtering);

		int32_t getContainerWidth() const { return _containterWidget ? _containterWidget->getWidth() : _rootWidget->getWidth(); }
		int32_t getContainerHeight() const { return _containterWidget ? _containterWidget->getHeight() : _rootWidget->getHeight(); }
	protected:
		enum class PaddingIndex : uint8_t
		{
			Top,
			Bottom,
			Left,
			Right
		};

		void _calculatePadding();
		virtual void _calculateSpacing();
		virtual void _resizeChildren();
		virtual void _setGui(const Unigine::Ptr<Unigine::Gui>& gui);

		std::vector<std::shared_ptr<WidgetBase>> _childWidgets;
		std::vector<Unigine::WidgetVBoxPtr> _spacers;
		Unigine::Math::vec4 _padding;
		Unigine::Math::ivec4 _paddingInPixels;
		bool _isPaddingEqual = false;
		bool _useSmallestPadding = false;
		float _spacing = 0.f;
		bool _ignorePadding = false;
		int32_t _scaledSpacing = 0;
		Unigine::WidgetPtr _containterWidget;
	};
}
