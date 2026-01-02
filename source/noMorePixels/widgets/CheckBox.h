#pragma once

#include "../base/WidgetBase.h"

namespace noMoPi
{
	class CheckBox : public WidgetBase
	{
	public:
		CheckBox(const ScaleSettings& scaleSettings);

		static std::shared_ptr<CheckBox> create() { return std::make_shared<CheckBox>(ScaleSettings()); }
		static std::shared_ptr<CheckBox> create(const ScaleSettings& scaleSettings) { return std::make_shared<CheckBox>(scaleSettings); }

		Unigine::TexturePtr _backgroundTexture, _tickTexture;
	};
}