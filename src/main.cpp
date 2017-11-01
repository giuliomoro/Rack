#include "engine.hpp"
#include "app.hpp"
#ifndef RACK_NOGUI
#include "gui.hpp"
#endif /*RACK_NOGUI*/
#include "plugin.hpp"
#include "settings.hpp"
#include "asset.hpp"
#include <unistd.h>


using namespace rack;

int main(int argc, char* argv[]) {
	char *cwd = getcwd(NULL, 0);
	printf("Current working directory is %s\n", cwd);
	free(cwd);

	pluginInit();
	engineInit();
#ifndef RACK_NOGUI
	guiInit();
#endif /*RACK_NOGUI*/
	sceneInit();
	if (argc >= 2) {
		// TODO Set gRackWidget->lastPath
		gRackWidget->loadPatch(argv[1]);
	}
	else {
		gRackWidget->loadPatch(assetLocal("autosave.vcv"));
	}
	settingsLoad(assetLocal("settings.json"));

	engineStart();
#ifndef RACK_NOGUI
	guiRun();
#endif /*RACK_NOGUI*/
	engineStop();

	settingsSave(assetLocal("settings.json"));
	gRackWidget->savePatch(assetLocal("autosave.vcv"));
	sceneDestroy();
#ifndef RACK_NOGUI
	guiDestroy();
#endif /*RACK_NOGUI*/
	engineDestroy();
	pluginDestroy();
	return 0;
}
