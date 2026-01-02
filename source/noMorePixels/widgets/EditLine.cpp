#include "EditLine.h"

#include "../Settings.h"

using namespace noMoPi;

EditLine::EditLine(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	Unigine::WidgetEditLinePtr _editLine = Unigine::WidgetEditLine::create("test");
	_editLine->setStyleTextureBackground(".noMorePixels/textures/white.png");
	_editLine->setBackgroundColor(Unigine::Math::vec4_green);
	_editLine->setBorderColor(Unigine::Math::vec4_zero);

	_editLine->getEventChanged().connect(ec, this, &EditLine::_onTextChanged);

	_widget = _editLine;
}

void EditLine::resize(int32_t width, int32_t height)
{
	// no way to remove the border, so this is a temporary fix
	_widget->setWidth(width - 4);
	_widget->setHeight(height - 4);

	_calculateMaxFontSize();

	_widget->setFontSize(_maxFontSize);
}

void noMoPi::EditLine::_calculateMaxFontSize()
{
	int32_t height = _widget->getHeight();

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
	_widget->setFont(Settings::get().getDefaultFont(fontIndex));

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
