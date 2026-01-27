#include "TestPlayer.h"
#include <UnigineConsole.h>
#include <UnigineInput.h>
#include <UnigineGame.h>
#include <UnigineAsyncQueue.h>

#include "noMorePixels/Settings.h"
#include "noMorePixels/UI.h"
#include "noMorePixels/widgets/HBox.h"
#include "noMorePixels/widgets/VBox.h"

REGISTER_COMPONENT(TestPlayer);

using namespace noMoPi;

void TestPlayer::init()
{
	_gui = Unigine::Gui::create();

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

	for (int32_t i = 0; i < _resolutionsToTest.size(); i++)
	{
		_textures.push_back(Unigine::Texture::create());
		_images.push_back(Unigine::Image::create());
		_isImageReady.push_back(false);
	}
}

void TestPlayer::update()
{
	static bool isFirstFrame = true;

	if (isFirstFrame)
	{
		_testSettings();

		_testUI();

		_testVBoxUnit();

		isFirstFrame = false;
	}

	_testVBoxVisual();
}

void TestPlayer::_assert(bool condition, const Unigine::String& message)
{
	if (condition)
		Unigine::Log::message("%s: PASSED\n", message.get());
	else 
		Unigine::Log::error("%s: FAILED\n", message.get());
}

bool TestPlayer::_isAllImagesReady()
{
	for (int32_t i = 0; i < _isImageReady.size(); i++)
	{
		if (!_isImageReady[i].fetch())
			return false;
	}

	return true;
}

void TestPlayer::_renderGuiToTexture()
{
	Unigine::RenderState::saveState();

	Unigine::RenderState::clearStates();

	Unigine::RenderState::setViewport(0, 0, _gui->getSize().x, _gui->getSize().y);

	Unigine::TexturePtr currentTexture = _textures[_currentResolutionIndex];

	_renderTarget->bindColorTexture(0, currentTexture);

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

	_isImageReady[_currentResolutionIndex] = false;

	int32_t previousResolutionIndex = _currentResolutionIndex;

	Unigine::Render::transferTextureToImage(Unigine::MakeCallback([this, previousResolutionIndex](Unigine::ImagePtr image) {
		if (!Unigine::Render::isFlipped())
		{
			image->flipY();
		}
		_images[previousResolutionIndex] = Unigine::Image::create(image);
		_isImageReady[previousResolutionIndex] = true;
		}),
		currentTexture);
}

void TestPlayer::_setGuiResolution(int32_t index)
{
	const Unigine::Math::ivec2& size = _resolutionsToTest[index];
	
	Unigine::TexturePtr current_texture = _textures[index];
	current_texture->clear();
	current_texture->create2D(size.x, size.y, Unigine::Texture::FORMAT_RGBA8, Unigine::Texture::FORMAT_USAGE_RENDER);
	_gui->setSize(size);

	_currentResolutionIndex = index;
}

void TestPlayer::_testSettings()
{
	Settings& settings = Settings::get();

	// TEST 1
	// assert that folders have '/' at the end

	const Unigine::String& _rootFolder = settings._rootFolder;

	assert(_rootFolder.size() > 0 && _rootFolder.get()[_rootFolder.size() - 1] == '/');
	
	const Unigine::String& _texturesFolder = settings._texturesFolder;

	assert(_texturesFolder.size() > 0 && _texturesFolder.get()[_texturesFolder.size() - 1] == '/');

	const Unigine::String& _localizationFolder = settings._localizationFolder;

	assert(_localizationFolder.size() > 0 && _localizationFolder.get()[_localizationFolder.size() - 1] == '/');

	const Unigine::String& _fontsFolder = settings._fontsFolder;

	assert(_fontsFolder.size() > 0 && _fontsFolder.get()[_fontsFolder.size() - 1] == '/');


	// TEST 2
	// test if path getters work correctly
	
	const Unigine::String localizationPath = settings.getLocalizationPath("test.loc");

	int32_t position = localizationPath.find(_rootFolder);
	assert(position == 0);
	position = localizationPath.find(_localizationFolder);
	assert(position == _rootFolder.size());
	position = localizationPath.find("test.loc");
	assert(position == _rootFolder.size() + _localizationFolder.size());

	const Unigine::String fontsPath = settings.getFontsPath("test.font");
	position = fontsPath.find(_rootFolder);
	assert(position == 0);
	position = fontsPath.find(_fontsFolder);
	assert(position == _rootFolder.size());
	position = fontsPath.find("test.font");
	assert(position == _rootFolder.size() + _fontsFolder.size());

	const Unigine::String texturesPath = settings.getTexturesPath("test.texture");
	position = texturesPath.find(_rootFolder);
	assert(position == 0);
	position = texturesPath.find(_texturesFolder);
	assert(position == _rootFolder.size());
	position = texturesPath.find("test.texture");
	assert(position == _rootFolder.size() + _texturesFolder.size());

	// TEST 3
	// test default fonts management

	const Unigine::String font1 = "default1.font";
	const Unigine::String font2 = "default2.font";

	int32_t font1Index = settings.addDefaultFont(font1);
	int32_t font2Index = settings.addDefaultFont(font2);

	Unigine::String defaultFontPath = settings.getDefaultFont(font1Index);
	assert(defaultFontPath.find(font1) != -1);

	defaultFontPath = settings.getDefaultFont(font2Index);
	assert(defaultFontPath.find(font2) != -1);

	settings.removeDefaultFont(font1Index);

	defaultFontPath = settings.getDefaultFont(font2Index - 1);
	assert(defaultFontPath.find(font2) != -1);
}

void TestPlayer::_testUI()
{
	// TEST 1
	// test UI class constructors and setGui method
	
	Unigine::GuiPtr gui = Unigine::Gui::create();

	UI testUI;
	assert(testUI._gui == nullptr);
	testUI.setGui(gui);
	assert(testUI._gui == gui);
	testUI = UI();
	assert(testUI._gui == nullptr);
	testUI = UI(gui);
	assert(testUI._gui == gui);

	// TEST 2
	// test setting root widget

	auto testHBox = HBox::create();
	
	assert(testUI._rootWidget == nullptr);
	assert(testHBox->getWidget()->getGui() != gui);
	assert(gui->getNumChildren() == 0);
	
	testUI.setRootWidget(testHBox);

	assert(testUI._rootWidget == testHBox);
	assert(testHBox->getWidget()->getGui() == gui);
	assert(gui->getNumChildren() == 1);
	assert(gui->getChild(0) == testHBox->getWidget());

	testHBox = HBox::create();
	testUI.setRootWidget(testHBox);

	assert(testUI._rootWidget == testHBox);
	assert(testHBox->getWidget()->getGui() == gui);
	assert(gui->getNumChildren() == 1);
	assert(gui->getChild(0) == testHBox->getWidget());

	testUI.clear(true);

	assert(testUI._rootWidget == nullptr);
	assert(gui->getNumChildren() == 0);

	// TEST 3
	// test setSize and resize of root widget

	testHBox = HBox::create();

	assert(testHBox->getWidget()->getWidth() == 0);
	assert(testHBox->getWidget()->getHeight() == 0);
	
	testUI.setRootWidget(testHBox);
	testUI.setSize(Unigine::Math::ivec2(800, 600));
	assert(testUI._gui->getWidth() == 800);
	assert(testUI._gui->getHeight() == 600);


	// TEST 4
	// test dictionary management

	assert(testUI._currentDictionary.size() == 0);

	const Unigine::String testDictionary = "testDictionary.loc";

	testUI.setDictionary(testDictionary);
	assert(testUI._currentDictionary.find(testDictionary) != -1);

	assert(testUI._currentLanguage.size() == 0);

	const Unigine::String testLanguage = "en";

	testUI.setLanguage(testLanguage);
	assert(testUI._currentLanguage == testLanguage);
}

void TestPlayer::_testVBoxUnit()
{
	Unigine::String outFunctionName;
	
	bool (TestPlayer::*tests[])(Unigine::String&) = {
		&TestPlayer::_testVBoxUnit_test1createDelete,
		&TestPlayer::_testVBoxUnit_test2resize,
		&TestPlayer::_testVBoxUnit_test3addChild,
		&TestPlayer::_testVBoxUnit_test4removeChild,
		&TestPlayer::_testVBoxUnit_test5getChild,
		&TestPlayer::_testVBoxUnit_test6clear
	};

	const int32_t numTests = sizeof(tests) / sizeof(tests[0]);

	for (int32_t i = 0; i < numTests; i++)
	{
		_assert((this->*tests[i])(outFunctionName), outFunctionName);
	}
}

bool TestPlayer::_testVBoxUnit_test1createDelete(Unigine::String& outFunctionName)
{
	outFunctionName = __FUNCTION__;
	
	auto rootVBox = VBox::create();

	Unigine::WidgetVBoxPtr rootWidget = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(rootVBox->getWidget());

	bool isZeroSize = rootVBox->getSize() == Unigine::Math::ivec2(0, 0) && rootWidget->getWidth() == 0 && rootWidget->getHeight() == 0;
	bool isManualLifetime = rootWidget->getLifetime() == Unigine::Widget::LIFETIME_MANUAL;
	bool isNotDeleted = rootWidget.isDeleted() == false;

	rootVBox = nullptr;
	
	bool isDeleted = rootWidget.isDeleted() == true;

	return isZeroSize && isManualLifetime && isNotDeleted && isDeleted;
}

#define VBOXTESTBEGIN				\
outFunctionName = __FUNCTION__;		\
_gui = Unigine::Gui::create();		\
UI testUI(_gui);					\
auto rootVBox = VBox::create();		\
testUI.setRootWidget(rootVBox);		\
Unigine::WidgetVBoxPtr rootWidget = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(rootVBox->getWidget());

bool TestPlayer::_testVBoxUnit_test2resize(Unigine::String& outFunctionName)
{
	VBOXTESTBEGIN;

	std::shared_ptr<WidgetBase> childWidgets[] = {
		VBox::create(),
		VBox::create(),
		VBox::create(),
		VBox::create(),
		VBox::create(),
	};

	for (auto& child : childWidgets)
	{
		rootVBox->addChild(child);
	}

	ScaleSettings scaleSettings[][5] = {
		{
		{ ScaleType::Proportional, 1.f / 6.f },
		{ ScaleType::Proportional, 1.f / 6.f },
		{ ScaleType::Proportional, 1.f / 6.f },
		{ ScaleType::Proportional, 1.f / 4.f },
		{ ScaleType::Proportional, 1.f / 4.f }
		},

		{
		{ ScaleType::Proportional, 0.1f },
		{ ScaleType::Proportional, 0.05f },
		{ ScaleType::Proportional, 0.2f },
		{ ScaleType::Proportional, 0.1f },
		{ ScaleType::Proportional, 0.15f }
		},

		{
		{ ScaleType::Fill, 1.f },
		{ ScaleType::Fill, 5.f },
		{ ScaleType::Fill, 2.f },
		{ ScaleType::Fill, .5f },
		{ ScaleType::Fill, 4.f }
		},

		{
		{ ScaleType::Proportional, 0.2f },
		{ ScaleType::Fill, 1.f },
		{ ScaleType::Fill, 2.f },
		{ ScaleType::Fill, 4.f },
		{ ScaleType::Proportional, 0.1f }
		},

		{
		{ ScaleType::Fill, 4.f },
		{ ScaleType::Proportional, 0.2f },
		{ ScaleType::Fill, 1.f },
		{ ScaleType::Proportional, 0.1f },
		{ ScaleType::Fill, 2.f }
		},

		{
		{ ScaleType::Fill, 2.f },
		{ ScaleType::Proportional, 0.2f },
		{ ScaleType::Fill, 4.f },
		{ ScaleType::Fill, 1.f },
		{ ScaleType::Proportional, 0.1f }
		},

		{
		{ ScaleType::Proportional, 0.2f },
		{ ScaleType::Fill, 4.f },
		{ ScaleType::Proportional, 0.1f },
		{ ScaleType::Fill, 1.f },
		{ ScaleType::Fill, 2.f }
		},

		{
		{ ScaleType::Ratio, 1.f / 3.f },
		{ ScaleType::Fill, 4.f },
		{ ScaleType::Proportional, 0.1f },
		{ ScaleType::Fill, 1.f },
		{ ScaleType::Fill, 2.f }
		}
	};

	int32_t numTests = sizeof(scaleSettings) / sizeof(scaleSettings[0]);

	for (const Unigine::Math::ivec2& resolution : _resolutionsToTest)
	{
		//Unigine::Log::message("Testing resolution: %dx%d\n", resolution.x, resolution.y);
		
		for (int32_t testIndex = 0; testIndex < numTests; testIndex++)
		{
			//Unigine::Log::message("  Test case %d\n", testIndex + 1);
			
			for (int32_t i = 0; i < 5; i++)
				childWidgets[i]->setScaleSettings(scaleSettings[testIndex][i]);

			testUI.setSize(resolution);
			testUI.updateLayout();

			float totalFillWeight = 0.f;
			float totalProportionalWeight = 0.f;

			for (int32_t i = 0; i < 5; i++)
			{
				const ScaleSettings& settings = childWidgets[i]->getScaleSettings();
				
				if (settings.scaleType == ScaleType::Fill)
					totalFillWeight += settings.scaleFactor;
				else if (settings.scaleType == ScaleType::Proportional)
					totalProportionalWeight += settings.scaleFactor;
			}

			//Unigine::Log::message("    Total Fill weight: %f, Total Proportional weight: %f\n", totalFillWeight, totalProportionalWeight);

			const int32_t maxSize = resolution.y;
			const int32_t opposizeSize = resolution.x;
			const int32_t expectedProportionalSize = static_cast<int32_t>(totalProportionalWeight * maxSize);
			int32_t usedSize = 0;

			float remainderWeight = 0.f;

			for (int32_t i = 0; i < 5; i++)
			{
				const ScaleSettings& settings = childWidgets[i]->getScaleSettings();

				if (settings.scaleType == ScaleType::Proportional)
				{
					float trueExpectedSize = maxSize * settings.scaleFactor;
					remainderWeight += trueExpectedSize - static_cast<int32_t>(trueExpectedSize);

					int32_t correction = 0;

					if (remainderWeight >= 0.99975f)
					{
						correction = 1;
						remainderWeight -= 0.99975f;
					}
					
					const int32_t expectedSize = static_cast<int32_t>(trueExpectedSize + correction);

					//Unigine::Log::message("Remainder weight: %f\n", remainderWeight);

					const int32_t actualSizeMethod = childWidgets[i]->getHeight();
					const int32_t actualSizeWidget = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(childWidgets[i]->getWidget())->getHeight();

					//Unigine::Log::message("Expected size: %d, Actual size (method): %d, Actual size (widget): %d\n", expectedSize, actualSizeMethod, actualSizeWidget);

					bool sizesMatch = (expectedSize == actualSizeMethod) && (expectedSize == actualSizeWidget);

					if (!sizesMatch)
						return false;

					usedSize += actualSizeMethod;
				}
			}

			if (usedSize != expectedProportionalSize)
				return false;

			remainderWeight = 0.f;

			for (int32_t i = 0; i < 5; i++)
			{
				const ScaleSettings& settings = childWidgets[i]->getScaleSettings();

				if (settings.scaleType == ScaleType::Ratio)
				{
					float trueExpectedSize = opposizeSize * settings.scaleFactor;

					remainderWeight += trueExpectedSize - static_cast<int32_t>(trueExpectedSize);

					int32_t correction = 0;
					if (remainderWeight >= 0.99975f)
					{
						correction = 1;
						remainderWeight -= 0.99975f;
					}

					const int32_t expectedSize = static_cast<int32_t>(trueExpectedSize + correction);
					//Unigine::Log::message("Remainder weight: %f\n", remainderWeight);
					const int32_t actualSizeMethod = childWidgets[i]->getHeight();
					const int32_t actualSizeWidget = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(childWidgets[i]->getWidget())->getHeight();
					//Unigine::Log::message("Expected size: %d, Actual size (method): %d, Actual size (widget): %d\n", expectedSize, actualSizeMethod, actualSizeWidget);
					bool sizesMatch = (expectedSize == actualSizeMethod) && (expectedSize == actualSizeWidget);
					if (!sizesMatch)
						return false;
					usedSize += actualSizeMethod;
				}
			}

			const int32_t remainingSize = maxSize - usedSize;
			remainderWeight = 0.f;

			for (int32_t i = 0; i < 5; i++)
			{
				const ScaleSettings& settings = childWidgets[i]->getScaleSettings();

				if (settings.scaleType == ScaleType::Fill)
				{
					float trueExpectedSize = remainingSize * (settings.scaleFactor / totalFillWeight);
					remainderWeight += trueExpectedSize - static_cast<int32_t>(trueExpectedSize);

					//Unigine::Log::message("Remainder weight: %f\n", remainderWeight);

					int32_t additionalPixel = 0;
					if (remainderWeight >= 0.9995f)
					{
						additionalPixel = 1;
						remainderWeight -= 1.f;
					}
					
					const int32_t expectedSize = static_cast<int32_t>(trueExpectedSize) + additionalPixel;

					const int32_t actualSizeMethod = childWidgets[i]->getHeight();
					const int32_t actualSizeWidget = Unigine::dynamic_ptr_cast<Unigine::WidgetVBox>(childWidgets[i]->getWidget())->getHeight();

					//Unigine::Log::message("Expected size: %d, Actual size (method): %d, Actual size (widget): %d\n", expectedSize, actualSizeMethod, actualSizeWidget);

					bool sizesMatch = (expectedSize == actualSizeMethod) && (expectedSize == actualSizeWidget);

					if (!sizesMatch)
						return false;

					usedSize += expectedSize;
				}
			}

			//Unigine::Log::message("    Used size: %d, Max size: %d\n", usedSize, maxSize);

			if (usedSize != maxSize && totalProportionalWeight >= 0.9995f)
				return false;
		}
	}

	return true;
}

bool TestPlayer::_testVBoxUnit_test3addChild(Unigine::String& outFunctionName)
{
	VBOXTESTBEGIN;

	auto firstChild = VBox::create();
	auto secondChild = VBox::create();
	auto thirdChild = VBox::create();

	const Unigine::WidgetPtr firstChildWidget = firstChild->getWidget();
	const Unigine::WidgetPtr secondChildWidget = secondChild->getWidget();
	const Unigine::WidgetPtr thirdChildWidget = thirdChild->getWidget();

	rootVBox->addChild(firstChild);

	bool isFirstGuiSet = firstChildWidget->getGui() == _gui;
	bool isChildWidgetSizeOne = rootVBox->_childWidgets.size() == 1 && rootWidget->getNumChildren() == 1;
	bool isSpacersSizeZero = rootVBox->_spacers.size() == 0;

	if (!isFirstGuiSet || !isChildWidgetSizeOne || !isSpacersSizeZero)
		return false;

	rootVBox->addChild(secondChild);

	bool isSecondGuiSet = secondChildWidget->getGui() == _gui;
	bool isChildWidgetSizeThree = rootVBox->_childWidgets.size() == 2 && rootWidget->getNumChildren() == 3;
	bool isSpacersSizeOne = rootVBox->_spacers.size() == 1;

	if (!isSecondGuiSet || !isChildWidgetSizeThree || !isSpacersSizeOne)
		return false;

	rootVBox->addChild(thirdChild);

	bool isThirdGuiSet = thirdChildWidget->getGui() == _gui;
	bool isChildWidgetSizeFive = rootVBox->_childWidgets.size() == 3 && rootWidget->getNumChildren() == 5;
	bool isSpacersSizeTwo = rootVBox->_spacers.size() == 2;

	if (!isThirdGuiSet || !isChildWidgetSizeFive || !isSpacersSizeTwo)
		return false;

	bool isSpacerLifetimeManual = rootVBox->_spacers[0]->getLifetime() == Unigine::Widget::LIFETIME_MANUAL &&
		rootVBox->_spacers[1]->getLifetime() == Unigine::Widget::LIFETIME_MANUAL;

	if (!isSpacerLifetimeManual)
		return false;
	
	return true;
}

bool TestPlayer::_testVBoxUnit_test4removeChild(Unigine::String& outFunctionName)
{
	VBOXTESTBEGIN;
	
	return false;
}

bool TestPlayer::_testVBoxUnit_test5getChild(Unigine::String& outFunctionName)
{
	VBOXTESTBEGIN;
	
	return false;
}

bool TestPlayer::_testVBoxUnit_test6clear(Unigine::String& outFunctionName)
{
	VBOXTESTBEGIN;
	
	return false;
}

void TestPlayer::_testVBoxVisual()
{
	static bool isFirstCall = true;
	static UI testUI;

	if (isFirstCall)
	{
		_gui = Unigine::Gui::create();
		testUI.setGui(_gui);
		isFirstCall = false;
	}

	static Unigine::Vector<int32_t> testsState;
	int32_t* lastTestState = nullptr;

	// TEST 1

	testsState.append(0);
	lastTestState = &testsState[testsState[testsState.size() - 1]];

	if (*lastTestState == 0)
	{
		*lastTestState = 1;

		for (int32_t i = 0; i < _resolutionsToTest.size(); i++)
		{
			_setGuiResolution(i);

			auto rootVBox = VBox::create();
			rootVBox->setBackgroundEnabled(true);

			testUI.setRootWidget(rootVBox);

			testUI.updateLayout();

			_renderGuiToTexture();
		}
	}

	if (*lastTestState == 1)
	{
		if (!_isAllImagesReady())
			return;

		*lastTestState = 2;
		
		assert(_images[0]->get2D(0, 0).i.r == 255);
	}

	// TEST 2
}

void TestPlayer::_testHBox()
{
}
