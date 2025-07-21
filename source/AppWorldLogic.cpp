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
		UI ui(gui);

		ui.addDictionary("test.xml");

		auto hbox = VBox::create();
		hbox->setBackground(true)
			->setBackgroundColor(50, 50, 50)
			->setPadding(0.05f, 0.05f, 0.05f, 0.05f)
			->setSpacing(0.05f);

		if (hbox)
			ui.setRootWidget(hbox);

		auto vbox1 = VBox::create();
		vbox1->setBackground(true)
			->setBackgroundColor(255, 0, 0);


		auto vbox2 = VBox::create();
		vbox2->setBackground(true)
			->setBackgroundColor(0, 255, 0);

		auto label = Label::create();
		label->setText("Source string")
			->setFontMaxHSpacing(0.15f)
			->setFontHSpacing(1.f)
			->setFontSize(0.9f)
			->setTextAlign(Align::Center, Align::Top);
		vbox2->addChild(label);

		hbox->addChild(vbox1);
		hbox->addChild(vbox2);

		ui.updateLayout();
		ui.translate();
		
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
