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

	Unigine::Vector<Unigine::Math::ivec2> _resolutionsToTest;
	Unigine::Vector<Unigine::TexturePtr> _textures;
	Unigine::Vector<Unigine::ImagePtr> _images;
	Unigine::Vector<Unigine::AtomicBool> _isImageReady;
	int32_t _currentResolutionIndex = -1;

	void init();
	void update();

	void _assert(bool condition, const char* message);

	bool _isAllImagesReady();

	void _renderGuiToTexture();
	void _setGuiResolution(int32_t index);

	void _testSettings();
	void _testUI();

	////////////////
	// VBOX TESTS //
	////////////////

	void _testVBoxUnit();
	bool _testVBoxUnit_test1createDelete();
	bool _testVBoxUnit_test2resize();

	void _testVBoxVisual();


	void _testHBox();
};