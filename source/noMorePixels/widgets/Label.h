#pragma once

#include "../base/WidgetBase.h"
#include "../types/Align.h"

namespace noMoPi
{
	class Label : public WidgetBase
	{
	public:
		Label(const ScaleSettings& scaleSettings);

		static std::shared_ptr<Label> create() { return std::make_shared<Label>(ScaleSettings()); }
		static std::shared_ptr<Label> create(const ScaleSettings& scaleSettings) { return std::make_shared<Label>(scaleSettings); }

		Label* setText(const char* text, bool isTranslatable = true);
		void _countTextLines(const char* text);
		Label* setFontSize(float fontSize);
		Label* setFontWrap(bool fontWrap);
		Label* setFontMaxHSpacing(float spacing);
		Label* setFontMaxVSpacing(float spacing);
		Label* setFontHSpacing(float spacing);
		Label* setFontVSpacing(float spacing);
		Label* setTextAlign(Align horizontal, Align vertical);
		Label* setDefaultFont(int32_t fontIndex);
		Label* setTextTypingAnimationCompletion(float completion);

		virtual void resize(int32_t width, int32_t height);
		void _updateFont(int32_t width);
		virtual void translate();

	protected:
		void _calculateMaxFontParams();

		Unigine::WidgetLabelPtr _label;

		bool _isTextTranslatable = false;
		Unigine::String _targetText, _keyText;

		int32_t _maxFontSize = 0;

		float _fontSize = 1.f;
		int32_t _newLineCount = 0;
		bool _fontWrap = false;

		float _fontMaxHSpacing = 0.f;
		int32_t _maxfontHSpacing = 0;
		float _fontMaxVSpacing = 0.f;
		int32_t _maxfontVSpacing = 0;

		float _fontHSpacing = 0.f;
		float _fontVSpacing = 0.f;
	};
}
