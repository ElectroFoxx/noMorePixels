#pragma once

#include "../base/WidgetContainerBase.h"

namespace noMoPi
{
	class HBox : public WidgetContainerBase
	{
	public:
		HBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<HBox> create() { return std::make_shared<HBox>(ScaleSettings()); }
		static std::shared_ptr<HBox> create(const ScaleSettings& scaleSettings) { return std::make_shared<HBox>(scaleSettings); }

		// friend a class for testing purposes
		friend class TestPlayer;
	};
}