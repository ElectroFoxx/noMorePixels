#include "CheckBox.h"

#include "../Settings.h"

using namespace noMoPi;

noMoPi::CheckBox::CheckBox(const ScaleSettings& scaleSettings) : WidgetBase(scaleSettings)
{
	Unigine::WidgetSpritePtr sprite = Unigine::WidgetSprite::create();

	_backgroundTexture = Unigine::Texture::create();
	_backgroundTexture->load(Settings::get().getTexturesPath("border.png"));
	//_backgroundTexture->setSamplerFlags(Unigine::Texture::SAMPLER_FILTER_POINT);

	sprite->setRender(_backgroundTexture);

	_tickTexture = Unigine::Texture::create();
	_tickTexture->load(Settings::get().getTexturesPath("tick.png"));
	//_tickTexture->setSamplerFlags(Unigine::Texture::SAMPLER_FILTER_POINT);

	sprite->setLayerRender(sprite->addLayer(), _tickTexture);

	static Unigine::EventConnection ec;

	sprite->getEventClicked().connect(ec, [](const Unigine::WidgetPtr& widget, int mouse) {
		Unigine::WidgetSpritePtr sprite = Unigine::dynamic_ptr_cast<Unigine::WidgetSprite>(widget);
		sprite->setLayerEnabled(1, !sprite->isLayerEnabled(1));
		});

	_rootWidget = sprite;
}
