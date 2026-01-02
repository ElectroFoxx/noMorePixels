#include "Settings.h"

using namespace noMoPi;

Unigine::String Settings::getLocalizationPath(const Unigine::String& file) const
{
	return _rootFolder + _localizationFolder + file;
}

Unigine::String Settings::getFontsPath(const Unigine::String& font) const
{
	return _rootFolder + _localizationFolder + font;
}

Unigine::String Settings::getTexturesPath(const Unigine::String& texture) const
{
	return _rootFolder + _texturesFolder + texture;
}

Unigine::String Settings::getWhiteBackground() const
{
	return _rootFolder + _texturesFolder + _whiteBackground;
}

int32_t Settings::addDefaultFont(const char* font)
{
	int32_t fontIndex = _defaultFonts.size();

	_defaultFonts.push_back(font);

	return fontIndex;
}

Unigine::String Settings::getDefaultFont(int32_t fontIndex)
{
	return _rootFolder + _fontsFolder + _defaultFonts[fontIndex];
}