#include "EditLine.h"

#include "../Settings.h"

using namespace noMoPi;

EditLine::EditLine(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	Unigine::WidgetEditLinePtr _editLine = Unigine::WidgetEditLine::create("test");
	_editLine->setLifetime(Unigine::Widget::LIFETIME_MANUAL);
	_editLine->setStyleTextureBackground(Settings::get().getWhiteBackground());
	_editLine->setBackgroundColor(Unigine::Math::vec4_zero);
	_editLine->setBorderColor(Unigine::Math::vec4_zero);

	_editLine->getEventChanged().connect(ec, this, &EditLine::_onTextChanged);

	_rootWidget = _editLine;
}

void EditLine::resize(int32_t width, int32_t height)
{
	// no way to remove the border, so this is a temporary fix
	_rootWidget->setWidth(width - 4);
	_rootWidget->setHeight(height - 4);

	_calculateMaxFontSize();

	_rootWidget->setFontSize(_maxFontSize);
}

EditLine* noMoPi::EditLine::setText(const Unigine::String& text)
{
	Unigine::WidgetEditLinePtr editLine = Unigine::dynamic_ptr_cast<Unigine::WidgetEditLine>(_rootWidget);
	
	editLine->setText(text);
	_previousText = text;
	
	return this;
}

EditLine* noMoPi::EditLine::setBackgroundEnabled(bool hasBackground)
{
	if (Unigine::WidgetEditLinePtr editLine = Unigine::dynamic_ptr_cast<Unigine::WidgetEditLine>(_rootWidget))
		editLine->setBackground(hasBackground);
	
	return this;
}

EditLine* noMoPi::EditLine::setBackgroundColor(float r, float g, float b, float a)
{
	if (Unigine::WidgetEditLinePtr editLine = Unigine::dynamic_ptr_cast<Unigine::WidgetEditLine>(_rootWidget))
		editLine->setBackgroundColor(Unigine::Math::vec4(r, g, b, a));

	return this;
}

EditLine* noMoPi::EditLine::setBackgroundColor(const Unigine::Math::vec4& color)
{
	if (Unigine::WidgetEditLinePtr editLine = Unigine::dynamic_ptr_cast<Unigine::WidgetEditLine>(_rootWidget))
		editLine->setBackgroundColor(color);

	return this;
}

EditLine* noMoPi::EditLine::setBackgroundColor(int32_t r, int32_t g, int32_t b, int32_t a)
{
	return setBackgroundColor(static_cast<float>(r) / 255, static_cast<float>(g) / 255, static_cast<float>(b) / 255, static_cast<float>(a) / 255);
}

EditLine* noMoPi::EditLine::setBackgroundTexture(const Unigine::String& texture)
{
	if (Unigine::WidgetEditLinePtr editLine = Unigine::dynamic_ptr_cast<Unigine::WidgetEditLine>(_rootWidget))
		editLine->setStyleTextureBackground(Settings::get().getTexturesPath(texture));

	return this;
}

void noMoPi::EditLine::_calculateMaxFontSize()
{
	int32_t height = _rootWidget->getHeight();

	_maxFontSize = static_cast<int32_t>((height - 5) * _magicMaxFontProportion + 2);
}

void noMoPi::EditLine::_onTextChanged(const Unigine::WidgetPtr& widget)
{
	Unigine::WidgetEditLinePtr editLine = Unigine::dynamic_ptr_cast<Unigine::WidgetEditLine>(widget);

	bool textChanged = false;
	if (editLine)
	{
		Unigine::String text = editLine->getText();

		if (_maxTextLength > 0 && text.size() > _maxTextLength)
		{
			text = text.substr(0, _maxTextLength);
			textChanged = true;
		}

		if (_isValidationEnabled)
		{
			bool isTextCorrect = regExpValidator->match(text);
			if (!isTextCorrect)
			{
				if (_isValidationStrictMode)
				{
					text = _previousText;
					textChanged = true;
				}
			}
		}

		if (textChanged)
		{
			editLine->setText(text);
		}
		_previousText = text;
		Unigine::Log::message("EditLine text changed: %s\n", text.get());
	}
}

EditLine* EditLine::setDefaultFont(int32_t fontIndex)
{
	_rootWidget->setFont(Settings::get().getDefaultFont(fontIndex));

	return this;
}

EditLine* noMoPi::EditLine::setMaxTextLength(int32_t maxLength)
{
	_maxTextLength = maxLength;

	return this;
}

EditLine* noMoPi::EditLine::setValidationPattern(const char* pattern)
{
	regExpValidator = Unigine::RegExp::create(pattern);

	return this;
}

EditLine* noMoPi::EditLine::setValidationEnabled(bool isEnabled)
{
	_isValidationEnabled = isEnabled;

	return this;
}

EditLine* noMoPi::EditLine::setValidationStrictMode(bool isStrictMode)
{
	_isValidationStrictMode = isStrictMode;

	return this;
}
