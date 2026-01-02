#pragma once

#include "../base/WidgetContainerBase.h"

namespace noMoPi
{
	class VBox : public WidgetContainerBase
	{
	public:
		VBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<VBox> create() { return std::make_shared<VBox>(ScaleSettings()); }
		static std::shared_ptr<VBox> create(const ScaleSettings& scaleSettings) { return std::make_shared<VBox>(scaleSettings); }
	};
}
