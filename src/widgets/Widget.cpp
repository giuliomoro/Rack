#include "widgets.hpp"
#include "app.hpp"
#include <algorithm>


namespace rack {

Widget::~Widget() {
	// You should only delete orphaned widgets
	assert(!parent);
	// Stop dragging and hovering this widget
#ifndef RACK_NOGUI
	if (gHoveredWidget == this) gHoveredWidget = NULL;
	if (gDraggedWidget == this) gDraggedWidget = NULL;
	if (gDragHoveredWidget == this) gDragHoveredWidget = NULL;
	if (gFocusedWidget == this) gFocusedWidget = NULL;
#endif /*RACK_NOGUI*/
	clearChildren();
}

#ifndef RACK_NOGUI
Rect Widget::getChildrenBoundingBox() {
	Rect bound;
	for (Widget *child : children) {
		if (child == children.front()) {
			bound = child->box;
		}
		else {
			bound = bound.expand(child->box);
		}
	}
	return bound;
}

Vec Widget::getRelativeOffset(Vec v, Widget *relative) {
	if (this == relative) {
		return v;
	}
	v = v.plus(box.pos);
	if (parent) {
		v = parent->getRelativeOffset(v, relative);
	}
	return v;
}

Rect Widget::getViewport(Rect r) {
	Rect bound;
	if (parent) {
		bound = parent->getViewport(box);
	}
	else {
		bound = box;
	}
	bound.pos = bound.pos.minus(box.pos);
	return r.clamp(bound);
}
#endif /*RACK_NOGUI*/

void Widget::addChild(Widget *widget) {
	assert(!widget->parent);
	widget->parent = this;
	children.push_back(widget);
}

void Widget::removeChild(Widget *widget) {
	assert(widget->parent == this);
	auto it = std::find(children.begin(), children.end(), widget);
	if (it != children.end()) {
		children.erase(it);
		widget->parent = NULL;
	}
}

void Widget::clearChildren() {
	for (Widget *child : children) {
		child->parent = NULL;
		delete child;
	}
	children.clear();
}

#ifndef RACK_NOGUI
void Widget::finalizeEvents() {
	// Stop dragging and hovering this widget
	if (gHoveredWidget == this) {
		gHoveredWidget->onMouseLeave();
		gHoveredWidget = NULL;
	}
	if (gDraggedWidget == this) {
		gDraggedWidget->onDragEnd();
		gDraggedWidget = NULL;
	}
	if (gDragHoveredWidget == this) {
		gDragHoveredWidget = NULL;
	}
	if (gFocusedWidget == this) {
		gFocusedWidget->onDefocus();
		gFocusedWidget = NULL;
	}
	for (Widget *child : children) {
		child->finalizeEvents();
	}
}
#endif /*RACK_NOGUI*/

void Widget::step() {
	for (Widget *child : children) {
		child->step();
	}
}

#ifndef RACK_NOGUI
void Widget::draw(NVGcontext *vg) {
	for (Widget *child : children) {
		if (!child->visible)
			continue;
		nvgSave(vg);
		nvgTranslate(vg, child->box.pos.x, child->box.pos.y);
		child->draw(vg);
		nvgRestore(vg);
	}
}

Widget *Widget::onMouseDown(Vec pos, int button) {
	for (auto it = children.rbegin(); it != children.rend(); it++) {
		Widget *child = *it;
		if (!child->visible)
			continue;
		if (child->box.contains(pos)) {
			Widget *w = child->onMouseDown(pos.minus(child->box.pos), button);
			if (w)
				return w;
		}
	}
	return NULL;
}

Widget *Widget::onMouseUp(Vec pos, int button) {
	for (auto it = children.rbegin(); it != children.rend(); it++) {
		Widget *child = *it;
		if (!child->visible)
			continue;
		if (child->box.contains(pos)) {
			Widget *w = child->onMouseUp(pos.minus(child->box.pos), button);
			if (w)
				return w;
		}
	}
	return NULL;
}

Widget *Widget::onMouseMove(Vec pos, Vec mouseRel) {
	for (auto it = children.rbegin(); it != children.rend(); it++) {
		Widget *child = *it;
		if (!child->visible)
			continue;
		if (child->box.contains(pos)) {
			Widget *w = child->onMouseMove(pos.minus(child->box.pos), mouseRel);
			if (w)
				return w;
		}
	}
	return NULL;
}

Widget *Widget::onHoverKey(Vec pos, int key) {
	for (auto it = children.rbegin(); it != children.rend(); it++) {
		Widget *child = *it;
		if (!child->visible)
			continue;
		if (child->box.contains(pos)) {
			Widget *w = child->onHoverKey(pos.minus(child->box.pos), key);
			if (w)
				return w;
		}
	}
	return NULL;
}

Widget *Widget::onScroll(Vec pos, Vec scrollRel) {
	for (auto it = children.rbegin(); it != children.rend(); it++) {
		Widget *child = *it;
		if (!child->visible)
			continue;
		if (child->box.contains(pos)) {
			Widget *w = child->onScroll(pos.minus(child->box.pos), scrollRel);
			if (w)
				return w;
		}
	}
	return NULL;
}

void Widget::onZoom() {
	for (auto it = children.rbegin(); it != children.rend(); it++) {
		Widget *child = *it;
		child->onZoom();
	}
}
#endif /*RACK_NOGUI*/

} // namespace rack
