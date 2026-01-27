#include "Label.h"

#include "../Settings.h"

using namespace noMoPi;

Label::Label(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	_rootWidget = Unigine::WidgetHBox::create();
	_rootWidget->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	_label = Unigine::WidgetLabel::create();
	_label->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	_rootWidget->addChild(_label);
}

Label* Label::setText(const char* text, bool isTranslatable)
{
	if (isTranslatable)
	{
		_keyText = text;
		//_targetText = _gui->translate(text);
	}
	else
		_targetText = text;

	_isTextTranslatable = isTranslatable;

	_countTextLines(_targetText);

	_label->setText(_targetText);

	return this;
}

void noMoPi::Label::_countTextLines(const char* text)
{
	_newLineCount = 0;
	for (const char* c = text; *c; c++)
	{
		if (*c == '\n')
			_newLineCount++;
	}
}

Label* Label::setFontSize(float fontSize)
{
	_fontSize = std::clamp(fontSize, 0.f, 1.f);

	_label->setFontSize(static_cast<int32_t>(_maxFontSize * fontSize));

	return this;
}

Label* Label::setFontWrap(bool fontWrap)
{
	_fontWrap = fontWrap;
	_label->setFontWrap(_fontWrap);

	return this;
}

Label* Label::setFontMaxHSpacing(float spacing)
{
	_fontMaxHSpacing = spacing;

	return this;
}

Label* Label::setFontMaxVSpacing(float spacing)
{
	_fontMaxVSpacing = spacing;

	return this;
}

Label* Label::setFontHSpacing(float spacing)
{
	_fontHSpacing = spacing;
	_label->setFontHSpacing(static_cast<int32_t>(_maxfontHSpacing * _fontHSpacing));

	return this;
}

Label* Label::setFontVSpacing(float spacing)
{
	_fontVSpacing = spacing;
	_label->setFontVSpacing(static_cast<int32_t>(_maxfontVSpacing * _fontVSpacing));

	return this;
}

Label* Label::setTextAlign(Align horizontal, Align vertical)
{
	if (_label)
	{
		if (horizontal == Align::Left)
			_label->setTextAlign(Unigine::Gui::ALIGN_LEFT);
		else if (horizontal == Align::Right)
			_label->setTextAlign(Unigine::Gui::ALIGN_RIGHT);
		else
			_label->setTextAlign(Unigine::Gui::ALIGN_CENTER);

		if (vertical == Align::Top)
			_label->setFlags(Unigine::Gui::ALIGN_TOP);
		else if (vertical == Align::Bottom)
			_label->setFlags(Unigine::Gui::ALIGN_BOTTOM);
		else
			_label->setFlags(Unigine::Gui::ALIGN_CENTER);
	}

	return this;
}

Label* noMoPi::Label::setDefaultFont(int32_t fontIndex)
{
	_label->setFont(Settings::get().getDefaultFont(fontIndex));

	_calculateMaxFontParams();

	_updateFont(_rootWidget->getWidth());

	return this;
}

Label* noMoPi::Label::setTextTypingAnimationCompletion(float completion)
{
	if (!_newLineCount && !_fontWrap)
	{
		int32_t currentTextSize = static_cast<int32_t>(Unigine::Math::roundFast(_targetText.size() * completion));

		_label->setText(_targetText.substr(0, currentTextSize));
	}
	else
	{
		Unigine::StringArray<> lines = _targetText.split(_targetText, "\n");

		Unigine::String targetText = "";
		for (int32_t i = 0; i < lines.size(); i++)
		{
			const Unigine::String& line = lines[i];

			int32_t currentTextSize = static_cast<int32_t>(Unigine::Math::roundFast(line.size() * completion));

			Unigine::String text = line.substr(0, currentTextSize);

			targetText += text;
			if (i < lines.size() - 1)
				targetText += "\n";
		}

		_label->setText(targetText);
	}

	return this;
}

void Label::resize(int32_t width, int32_t height)
{
	WidgetBase::resize(width, height);

	_calculateMaxFontParams();

	_updateFont(width);
}

void noMoPi::Label::_updateFont(int32_t width)
{
	if (_label)
	{
		_label->setWidth(width);

		if (!_fontWrap)
			_label->setFontSize(static_cast<int32_t>(_maxFontSize * _fontSize));
		else
			_label->setFontSize(_maxFontSize);

		_label->setFontHSpacing(static_cast<int32_t>(_maxfontHSpacing * _fontHSpacing));
		_label->setFontVSpacing(static_cast<int32_t>(_maxfontVSpacing * _fontVSpacing));
	}
}

void noMoPi::Label::_calculateMaxFontParams()
{
	const int32_t height = _rootWidget->getHeight();
	const int32_t width = _rootWidget->getWidth();

	if (!_fontWrap)
	{
		_label->setFontSize(height);
		const Unigine::Math::ivec2 textRawRenderSize = _label->getTextRenderSize(_targetText);

		_label->setFontHSpacing(static_cast<int32_t>(height * _fontMaxHSpacing));
		_label->setFontVSpacing(static_cast<int32_t>(height * _fontMaxVSpacing));

		const Unigine::Math::ivec2 textRenderSizeWithSpacing = _label->getTextRenderSize(_targetText);

		const float hScaleProportion = static_cast<float>(textRenderSizeWithSpacing.x) / textRawRenderSize.x;
		const float vScaleProportion = 1.f + _fontMaxVSpacing;

		_maxFontSize = height / (_newLineCount + 1);
		if (textRenderSizeWithSpacing.x > width)
		{
			float maxBaseWidth = static_cast<float>(width) / hScaleProportion;

			_maxFontSize = static_cast<int32_t>(height * (maxBaseWidth / textRawRenderSize.x));
		}
		if (textRenderSizeWithSpacing.y > height)
		{
			float maxBaseHeight = static_cast<float>(height) / vScaleProportion;

			_maxFontSize = std::min(static_cast<int32_t>(height * (maxBaseHeight / textRawRenderSize.y)), _maxFontSize);
		}
	}
	else
		_maxFontSize = static_cast<int32_t>(height * _fontSize);

	_maxfontHSpacing = static_cast<int32_t>(_maxFontSize * _fontMaxHSpacing);
	_maxfontVSpacing = static_cast<int32_t>(_maxFontSize * _fontMaxVSpacing);
}

void noMoPi::Label::_setGui(const Unigine::GuiPtr& gui)
{
	WidgetBase::_setGui(gui);
	_label->setGui(_gui);
}

noMoPi::Label::~Label()
{
	_label.deleteLater();
}

void Label::translate()
{
	if (!_isTextTranslatable)
		return;

	_targetText = _gui->translate(_keyText);
	_countTextLines(_targetText);
	_label->setText(_targetText);

	_calculateMaxFontParams();

	_updateFont(_rootWidget->getWidth());
}