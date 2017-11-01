#include "settings.hpp"
#include "app.hpp"
#ifndef RACK_NOGUI
#include "gui.hpp"
#endif
#include "engine.hpp"
#include "plugin.hpp"
#include <jansson.h>


namespace rack {


static json_t *settingsToJson() {
	// root
	json_t *rootJ = json_object();

#ifndef RACK_NOGUI
	// token
	json_t *tokenJ = json_string(gToken.c_str());
	json_object_set_new(rootJ, "token", tokenJ);

	// opacity
	float opacity = gToolbar->wireOpacitySlider->value;
	json_t *opacityJ = json_real(opacity);
	json_object_set_new(rootJ, "wireOpacity", opacityJ);

	// tension
	float tension = gToolbar->wireTensionSlider->value;
	json_t *tensionJ = json_real(tension);
	json_object_set_new(rootJ, "wireTension", tensionJ);

	// zoom
	float zoom = gRackScene->zoomWidget->zoom;
	json_t *zoomJ = json_real(zoom);
	json_object_set_new(rootJ, "zoom", zoomJ);

	// allowCursorLock
	json_t *allowCursorLockJ = json_boolean(gAllowCursorLock);
	json_object_set_new(rootJ, "allowCursorLock", allowCursorLockJ);
#endif /*RACK_NOGUI*/

	// sampleRate
	json_t *sampleRateJ = json_real(engineGetSampleRate());
	json_object_set_new(rootJ, "sampleRate", sampleRateJ);

#ifndef RACK_NOGUI
	// plugLight
	json_t *plugLightJ = json_boolean(gToolbar->plugLightButton->value > 0.0);
	json_object_set_new(rootJ, "plugLight", plugLightJ);
#endif /*RACK_NOGUI*/

	// lastPath
	json_t *lastPathJ = json_string(gRackWidget->lastPath.c_str());
	json_object_set_new(rootJ, "lastPath", lastPathJ);

	return rootJ;
}

static void settingsFromJson(json_t *rootJ) {
#ifndef RACK_NOGUI
	// token
	json_t *tokenJ = json_object_get(rootJ, "token");
	if (tokenJ)
		gToken = json_string_value(tokenJ);

	// opacity
	json_t *opacityJ = json_object_get(rootJ, "wireOpacity");
	if (opacityJ)
		gToolbar->wireOpacitySlider->value = json_number_value(opacityJ);

	// tension
	json_t *tensionJ = json_object_get(rootJ, "wireTension");
	if (tensionJ)
		gToolbar->wireTensionSlider->value = json_number_value(tensionJ);

	// zoom
	json_t *zoomJ = json_object_get(rootJ, "zoom");
	if (zoomJ) {
		gRackScene->zoomWidget->setZoom(clampf(json_number_value(zoomJ), 0.25, 4.0));
		gToolbar->zoomSlider->setValue(json_number_value(zoomJ) * 100.0);
	}

	// allowCursorLock
	json_t *allowCursorLockJ = json_object_get(rootJ, "allowCursorLock");
	if (allowCursorLockJ)
		gAllowCursorLock = json_is_true(allowCursorLockJ);
#endif /*RACK_NOGUI*/

	// sampleRate
	json_t *sampleRateJ = json_object_get(rootJ, "sampleRate");
	if (sampleRateJ) {
		float sampleRate = json_number_value(sampleRateJ);
		engineSetSampleRate(sampleRate);
	}

#ifndef RACK_NOGUI
	// plugLight
	json_t *plugLightJ = json_object_get(rootJ, "plugLight");
	if (plugLightJ)
		gToolbar->plugLightButton->setValue(json_is_true(plugLightJ) ? 1.0 : 0.0);
#endif /*RACK_NOGUI*/

	// lastPath
	json_t *lastPathJ = json_object_get(rootJ, "lastPath");
	if (lastPathJ)
		gRackWidget->lastPath = json_string_value(lastPathJ);
}


void settingsSave(std::string filename) {
	printf("Saving settings %s\n", filename.c_str());
	FILE *file = fopen(filename.c_str(), "w");
	if (!file)
		return;

	json_t *rootJ = settingsToJson();
	if (rootJ) {
		json_dumpf(rootJ, file, JSON_INDENT(2));
		json_decref(rootJ);
	}

	fclose(file);
}

void settingsLoad(std::string filename) {
	printf("Loading settings %s\n", filename.c_str());
	FILE *file = fopen(filename.c_str(), "r");
	if (!file)
		return;

	json_error_t error;
	json_t *rootJ = json_loadf(file, 0, &error);
	if (rootJ) {
		settingsFromJson(rootJ);
		json_decref(rootJ);
	}
	else {
		printf("JSON parsing error at %s %d:%d %s\n", error.source, error.line, error.column, error.text);
	}

	fclose(file);
}


} // namespace rack
