#include "TestPlayer.h"
#include <UnigineConsole.h>
#include <UnigineInput.h>
#include <UnigineGame.h>
#include "noMorePixels/noMorePixels.h"

REGISTER_COMPONENT(TestPlayer);

using namespace noMoPi;

void TestPlayer::init()
{
	_gui = Unigine::Gui::create();

	_texture = Unigine::Texture::create();

	_renderTarget = Unigine::RenderTarget::create();

	_resolutionsToTest.push_back(Unigine::Math::ivec2(800, 600));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1024, 768));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1152, 864));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1176, 664));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1280, 720));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1280, 768));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1280, 800));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1280, 960));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1280, 1024));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1360, 768));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1366, 768));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1440, 900));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1440, 1080));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1600, 900));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1680, 1024));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1680, 1050));
	_resolutionsToTest.push_back(Unigine::Math::ivec2(1920, 1080));
}

void TestPlayer::update()
{
	static bool isFirstFrame = true;

	if (isFirstFrame)
	{

	}
}

void TestPlayer::_renderGuiToTexture()
{
	Unigine::RenderState::saveState();

	Unigine::RenderState::clearStates();

	Unigine::RenderState::setViewport(0, 0, _gui->getSize().x, _gui->getSize().y);

	_renderTarget->bindColorTexture(0, _texture);

	_renderTarget->enable();

	Unigine::RenderState::clearBuffer(Unigine::RenderState::BUFFER_COLOR, Unigine::Math::vec4_black);

	_gui->enable();
	_gui->update();
	_gui->preRender();
	_gui->render();
	_gui->disable();

	_renderTarget->disable();
	_renderTarget->unbindColorTexture(0);

	Unigine::RenderState::restoreState();

	Unigine::Render::asyncTransferTextureToImage(
		nullptr,
		Unigine::MakeCallback([this](Unigine::ImagePtr image)
			{
				if (image->getFormat() == Unigine::Image::FORMAT_RGBA8)
					image->convertToFormat(Unigine::Image::FORMAT_RGB8);
				else if (image->getFormat() == Unigine::Image::FORMAT_RGBA16F)
					image->convertToFormat(Unigine::Image::FORMAT_RGB16F);

				//if (!Unigine::Render::isFlipped())
				//	image->flipY();

				image->save("test.jpg");
				Unigine::Console::onscreenMessageLine(Unigine::Math::vec4_green, "test.jpg saved.");
			}),
		_texture);
}

void TestPlayer::_setGuiResolution(const Unigine::Math::ivec2& size)
{
	_texture->clear();
	_texture->create2D(size.x, size.y, Unigine::Texture::FORMAT_RGBA8, Unigine::Texture::FORMAT_USAGE_RENDER);
	_gui->setSize(size);
}
