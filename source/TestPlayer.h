#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineViewport.h>

class TestPlayer : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TestPlayer, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

private:
	Unigine::GuiPtr _gui;
	Unigine::RenderTargetPtr _renderTarget;
	Unigine::TexturePtr _texture;
	Unigine::Vector<Unigine::Math::ivec2> _resolutionsToTest;

	void init();
	void update();

	void _renderGuiToTexture();
	void _setGuiResolution(const Unigine::Math::ivec2& size);
};