#include <UnigineInit.h>
#include <UnigineEngine.h>

#include "AppEditorLogic.h"
#include "AppSystemLogic.h"
#include "AppWorldLogic.h"

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	// UnigineLogic
	AppSystemLogic system_logic;
	AppWorldLogic world_logic;
	AppEditorLogic editor_logic;

	// init engine
	Unigine::EnginePtr engine(argc, argv);

	// enter main loop
	engine->main(&system_logic, &world_logic, &editor_logic);

	return 0;
}
