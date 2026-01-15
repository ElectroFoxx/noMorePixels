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
