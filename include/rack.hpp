#pragma once

#include "util.hpp"
#include "math.hpp"
#include "asset.hpp"
#include "plugin.hpp"
#include "engine.hpp"
#ifndef RACK_NOGUI
#include "gui.hpp"
#endif /*RACK_NOGUI*/
#include "app.hpp"
#ifndef RACK_NOGUI
#include "components.hpp"
#endif /*RACK_NOGUI*/


namespace rack {


////////////////////
// helpers
////////////////////

template <class TModuleWidget>
Model *createModel(std::string manufacturerSlug, std::string manufacturerName, std::string slug, std::string name) {
	struct TModel : Model {
		ModuleWidget *createModuleWidget() override {
			ModuleWidget *moduleWidget = new TModuleWidget();
			moduleWidget->model = this;
			return moduleWidget;
		}
	};
	Model *model = new TModel();
	model->slug = slug;
	model->name = name;
	model->manufacturerSlug = manufacturerSlug;
	model->manufacturerName = manufacturerName;
	return model;
}

template <class TScrew>
Widget *createScrew(Vec pos) {
	Widget *screw = new TScrew();
	screw->box.pos = pos;
	return screw;
}

template <class TParamWidget>
ParamWidget *createParam(Vec pos, Module *module, int paramId, float minValue, float maxValue, float defaultValue) {
	ParamWidget *param = new TParamWidget();
	param->box.pos = pos;
	param->module = module;
	param->paramId = paramId;
	param->setLimits(minValue, maxValue);
	param->setDefaultValue(defaultValue);
	return param;
}

template <class TPort>
Port *createInput(Vec pos, Module *module, int inputId) {
	Port *port = new TPort();
	port->box.pos = pos;
	port->module = module;
	port->type = Port::INPUT;
	port->portId = inputId;
	return port;
}

template <class TPort>
Port *createOutput(Vec pos, Module *module, int outputId) {
	Port *port = new TPort();
	port->box.pos = pos;
	port->module = module;
	port->type = Port::OUTPUT;
	port->portId = outputId;
	return port;
}

template<class TModuleLightWidget>
ModuleLightWidget *createLight(Vec pos, Module *module, int lightId) {
	ModuleLightWidget *light = new TModuleLightWidget();
	light->box.pos = pos;
	light->module = module;
	light->lightId = lightId;
	return light;
}


} // namespace rack
