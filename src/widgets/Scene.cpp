#include "widgets.hpp"
#ifndef RACK_NOGUI
#include "gui.hpp"
#endif /*RACK_NOGUI*/


namespace rack {

#ifndef RACK_NOGUI
void Scene::setOverlay(Widget *w) {
	if (overlay) {
		removeChild(overlay);
		delete overlay;
		overlay = NULL;
	}
	if (w) {
		addChild(w);
		overlay = w;
		overlay->box.pos = Vec();
	}
}

Menu *Scene::createMenu() {
	// Get relative position of the click
	MenuOverlay *overlay = new MenuOverlay();
	Menu *menu = new Menu();
	menu->box.pos = gMousePos;

	overlay->addChild(menu);
	gScene->setOverlay(overlay);

	return menu;
}
#endif /*RACK_NOGUI*/

void Scene::step() {
#ifndef RACK_NOGUI
	if (overlay) {
		overlay->box.size = box.size;
	}
#endif /*RACK_NOGUI*/

	Widget::step();
}


} // namespace rack
