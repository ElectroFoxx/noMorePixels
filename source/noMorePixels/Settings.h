#pragma once

#include <UnigineString.h>

namespace noMoPi
{
	class Settings
	{
	public:
		static Settings& get()
		{
			static Settings instance;
			return instance;
		}

		Unigine::String getLocalizationPath(const Unigine::String& file) const;
		Unigine::String getFontsPath(const Unigine::String& font) const;
		Unigine::String getTexturesPath(const Unigine::String& texture) const;
		Unigine::String getWhiteBackground() const;

		int32_t addDefaultFont(const char* font);
		Unigine::String getDefaultFont(int32_t fontIndex);
	private:
		Settings() = default;
		const Unigine::String _rootFolder = ".noMorePixels/";
		const Unigine::String _texturesFolder = "textures/";
		const Unigine::String _localizationFolder = "localization/";
		const Unigine::String _fontsFolder = "fonts/";

		Unigine::Vector<Unigine::String> _defaultFonts;

		const Unigine::String _whiteBackground = "white.png";
	};
}
