#pragma once

#include "ScaleType.h"

namespace noMoPi
{
	struct ScaleSettings
	{
		ScaleType scaleType = ScaleType::Fill;
		float scaleFactor = 1.f;
	};
}