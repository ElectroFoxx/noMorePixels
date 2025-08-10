#include "AppWorldLogic.h"
#include "noMorePixels/noMorePixels.h"
#include <UnigineUserInterface.h>

AppWorldLogic::AppWorldLogic()
{}

AppWorldLogic::~AppWorldLogic()
{}

using namespace noMoPi;

int AppWorldLogic::init()
{
	Unigine::GuiPtr gui = Unigine::Gui::getCurrent();

	if (gui)
	{
		static UI ui(gui);

		Settings::get().addDefaultFont("Roboto-Regular.ttf");

		auto hbox = HBox::create();
		hbox->setBackgroundEnabled(true)
			->setBackgroundTexture("3dMenuBackground.png")
			->setBackgroundTextureFiltering(Unigine::Texture::SAMPLER_FILTER_POINT)
			->setBackgroundColor(50, 50, 50)
			->setPadding(0.05f, 0.05f, 0.05f, 0.05f)
			->setPaddingEqual(true)
			->setSpacing(0.05f);

		if (hbox)
			ui.setRootWidget(hbox);

		auto vbox1 = VBox::create();
		vbox1->setBackgroundEnabled(true)
			->setBackgroundColor(255, 0, 0);


		auto vbox2 = VBox::create();
		vbox2->setBackgroundEnabled(true)
			->setBackgroundColor(0, 255, 0);

		auto vboxsquare = VBox::create();
		vbox2->setBackgroundEnabled(true)
			->setBackgroundColor(0, 0, 255);

		auto label = Label::create(ScaleSettings(ScaleType::Proportional, 0.1f));
		label->setText("Source string")
			->setDefaultFont(0)
			->setTextAlign(Align::Center, Align::Top);

		auto edit = EditLine::create();
		edit->setDefaultFont(0);

		auto check = CheckBox::create();
		
		auto scrollbox = ScrollBox::create();
		scrollbox->setVisibleItemCount(2);

		auto vbox1scroll = VBox::create();
		vbox1scroll->setBackgroundEnabled(true)
			->setBackgroundColor(255, 0, 0);

		auto vbox2scroll = VBox::create();
		vbox2scroll->setBackgroundEnabled(true)
			->setBackgroundColor(0, 255, 0);

		auto vbox3scroll = VBox::create();
		vbox3scroll->setBackgroundEnabled(true)
			->setBackgroundColor(255, 0, 0);

		scrollbox->addChild(vbox1scroll);
		scrollbox->addChild(vbox2scroll);
		scrollbox->addChild(vbox3scroll);

		vbox1->addChild(vboxsquare);
		vbox1->addChild(edit);
		vbox1->addChild(check);
		
		vbox2->addChild(label);
		vbox2->addChild(scrollbox);

		hbox->addChild(vbox1);
		hbox->addChild(vbox2);

		ui.updateLayout();
		//ui.setDictionary("test.xml");
		//ui.setLanguage("en");
		//ui.translate();
		
	}

	return 1;
}

int AppWorldLogic::update()
{
	return 1;
}

int AppWorldLogic::postUpdate()
{
	return 1;
}

int AppWorldLogic::updatePhysics()
{
	return 1;
}

int AppWorldLogic::shutdown()
{
	return 1;
}

int AppWorldLogic::save(const Unigine::StreamPtr &stream)
{
	UNIGINE_UNUSED(stream);
	return 1;
}

int AppWorldLogic::restore(const Unigine::StreamPtr &stream)
{
	UNIGINE_UNUSED(stream);
	return 1;
}
