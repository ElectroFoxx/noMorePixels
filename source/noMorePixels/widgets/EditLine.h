#pragma once

#include <UnigineRegExp.h>

#include "../base/WidgetBase.h"

namespace noMoPi
{
	class EditLine : public WidgetBase
	{
	public:
		EditLine(const ScaleSettings& scaleSettings);

		static std::shared_ptr<EditLine> create() { return std::make_shared<EditLine>(ScaleSettings()); }
		static std::shared_ptr<EditLine> create(const ScaleSettings& scaleSettings) { return std::make_shared<EditLine>(scaleSettings); }

		virtual void resize(int32_t width, int32_t height);
		EditLine* setDefaultFont(int32_t fontIndex);
		EditLine* setMaxTextLength(int32_t maxLength);
		EditLine* setValidationPattern(const char* pattern);
		EditLine* setValidationEnabled(bool isEnabled);
		EditLine* setValidationStrictMode(bool isStrictMode);

		const Unigine::String& getText() const { return _previousText; }
	private:
		void _calculateMaxFontSize();
		void _onTextChanged(const Unigine::WidgetPtr& widget);

		Unigine::EventConnections ec;

		Unigine::RegExpPtr regExpValidator;
		Unigine::String _previousText;
		bool _isValidationEnabled = false;
		bool _isValidationStrictMode = false;

		int32_t _maxFontSize = 0;
		int32_t _maxTextLength = 0;

		float _magicMaxFontProportion = static_cast<float>(999) / 1124;
	};
}
