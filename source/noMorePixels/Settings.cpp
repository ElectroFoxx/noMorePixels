#include "Settings.h"

using namespace noMoPi;

Unigine::String Settings::getLocalizationPath(const Unigine::String& file) const
{
	return _rootFolder + _localizationFolder + file;
}

Unigine::String Settings::getFontsPath(const Unigine::String& font) const
{
	return _rootFolder + _fontsFolder + font;
}

Unigine::String Settings::getTexturesPath(const Unigine::String& texture) const
{
	return _rootFolder + _texturesFolder + texture;
}

Unigine::String Settings::getWhiteBackground() const
{
	return _rootFolder + _texturesFolder + _whiteTextureName;
}

int32_t Settings::addDefaultFont(const Unigine::String& font)
{
	int32_t fontIndex = _defaultFonts.size();

	_defaultFonts.push_back(font);

	return fontIndex;
}

void noMoPi::Settings::removeDefaultFont(int32_t fontIndex)
{
	_defaultFonts.remove(fontIndex);
}

Unigine::String Settings::getDefaultFont(int32_t fontIndex)
{
	return getFontsPath(_defaultFonts[fontIndex]);
}