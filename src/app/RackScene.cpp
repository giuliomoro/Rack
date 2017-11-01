#include "app.hpp"
#ifndef RACK_NOGUI
#include "gui.hpp"
#endif /*RACK_NOGUI*/
#include "util/request.hpp"
#ifndef RACK_NOGUI
#include "../ext/osdialog/osdialog.h"
#endif /*RACK_NOGUI*/
#include <string.h>
#include <thread>


namespace rack {


static std::string newVersion = "";

static void checkVersion() {
	json_t *resJ = requestJson(METHOD_GET, gApiHost + "/version", NULL);

	if (resJ) {
		json_t *versionJ = json_object_get(resJ, "version");
		if (versionJ) {
			const char *version = json_string_value(versionJ);
			if (version && strlen(version) > 0 && version != gApplicationVersion) {
				newVersion = version;
			}
		}
		json_decref(resJ);
	}
}


RackScene::RackScene() {
#ifndef RACK_NOGUI
	scrollWidget = new RackScrollWidget();
	{
		zoomWidget = new ZoomWidget();
		{
			assert(!gRackWidget);
			gRackWidget = new RackWidget();
			zoomWidget->addChild(gRackWidget);
		}
		scrollWidget->container->addChild(zoomWidget);
	}
	addChild(scrollWidget);

	gToolbar = new Toolbar();
	addChild(gToolbar);
	scrollWidget->box.pos.y = gToolbar->box.size.y;
#endif /*RACK_NOGUI*/

	// Check for new version
	if (!gApplicationVersion.empty()) {
		std::thread versionThread(checkVersion);
		versionThread.detach();
	}
}

void RackScene::step() {
#ifndef RACK_NOGUI
	// Resize owned descendants
	gToolbar->box.size.x = box.size.x;
	scrollWidget->box.size = box.size.minus(scrollWidget->box.pos);

	// Resize to be a bit larger than the ScrollWidget viewport
	gRackWidget->box.size = scrollWidget->box.size
		.minus(scrollWidget->container->box.pos)
		.plus(Vec(500, 500))
		.div(zoomWidget->zoom);
#endif /*RACK_NOGUI*/

	Scene::step();

#ifndef RACK_NOGUI
	zoomWidget->box.size = gRackWidget->box.size.mult(zoomWidget->zoom);
#endif /*RACK_NOGUI*/

	// Version popup message
	if (!newVersion.empty()) {
		std::string versionMessage = stringf("Rack %s is available.\n\nYou have Rack %s.\n\nWould you like to download the new version on the website?", newVersion.c_str(), gApplicationVersion.c_str());
#ifndef RACK_NOGUI
		if (osdialog_message(OSDIALOG_INFO, OSDIALOG_YES_NO, versionMessage.c_str())) {
			std::thread t(openBrowser, "https://vcvrack.com/");
			t.detach();
			guiClose();
		}
#endif /*RACK_NOGUI*/
		newVersion = "";
	}
}

#ifndef RACK_NOGUI
void RackScene::draw(NVGcontext *vg) {
	Scene::draw(vg);
}

Widget *RackScene::onHoverKey(Vec pos, int key) {
	switch (key) {
		case GLFW_KEY_N:
			if (guiIsModPressed() && !guiIsShiftPressed()) {
				gRackWidget->reset();
				return this;
			}
			break;
		case GLFW_KEY_Q:
			if (guiIsModPressed() && !guiIsShiftPressed()) {
				guiClose();
				return this;
			}
			break;
		case GLFW_KEY_O:
			if (guiIsModPressed() && !guiIsShiftPressed()) {
				gRackWidget->openDialog();
				return this;
			}
			break;
		case GLFW_KEY_S:
			if (guiIsModPressed() && !guiIsShiftPressed()) {
				gRackWidget->saveDialog();
				return this;
			}
			if (guiIsModPressed() && guiIsShiftPressed()) {
				gRackWidget->saveAsDialog();
				return this;
			}
			break;
	}

	return Widget::onHoverKey(pos, key);
}
#endif /*RACK_NOGUI*/



} // namespace rack
