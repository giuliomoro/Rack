#pragma once
#include <vector>
#include <jansson.h>
#include "widgets.hpp"


namespace rack {


struct Model;
struct Module;
struct Wire;

struct RackWidget;
struct ParamWidget;
struct Port;

////////////////////
// module
////////////////////

// A 1U module should be 15x380. Thus the width of a module should be a factor of 15.
#define RACK_GRID_WIDTH 15
#define RACK_GRID_HEIGHT 380

struct ModuleWidget : OpaqueWidget {
	Model *model = NULL;
	/** Owns the module pointer */
	Module *module = NULL;

	std::vector<Port*> inputs;
	std::vector<Port*> outputs;
	std::vector<ParamWidget*> params;

	~ModuleWidget();
	void setModule(Module *module);
	/** Convenience functions for adding special widgets (calls addChild()) */
	void addInput(Port *input);
	void addOutput(Port *output);
	void addParam(ParamWidget *param);

	virtual json_t *toJson();
	virtual void fromJson(json_t *rootJ);

	/** Disconnects cables from all ports
	Called when the user clicks Disconnect Cables in the context menu.
	*/
	virtual void disconnect();
	/** Resets the parameters of the module and calls the Module's randomize().
	Called when the user clicks Initialize in the context menu.
	*/
	virtual void reset();
	/** Deprecated */
	virtual void initialize() final {}
	/** Randomizes the parameters of the module and calls the Module's randomize().
	Called when the user clicks Randomize in the context menu.
	*/
	virtual void randomize();
#ifndef RACK_NOGUI
	virtual Menu *createContextMenu();

	void draw(NVGcontext *vg) override;

	Vec dragPos;
	Widget *onMouseMove(Vec pos, Vec mouseRel) override;
	Widget *onHoverKey(Vec pos, int key) override;
	void onDragStart() override;
	void onDragMove(Vec mouseRel) override;
	void onDragEnd() override;
	void onMouseDownOpaque(int button) override;
#endif /*RACK_NOGUI*/
};

struct WireWidget : OpaqueWidget {
	Port *outputPort = NULL;
	Port *inputPort = NULL;
	Port *hoveredOutputPort = NULL;
	Port *hoveredInputPort = NULL;
	Wire *wire = NULL;
#ifndef RACK_NOGUI
	NVGcolor color;
#endif /*RACK_NOGUI*/

	WireWidget();
	~WireWidget();
	/** Synchronizes the plugged state of the widget to the owned wire */
	void updateWire();
#ifndef RACK_NOGUI
	Vec getOutputPos();
	Vec getInputPos();
	void draw(NVGcontext *vg) override;
	void drawPlugs(NVGcontext *vg);
#endif /*RACK_NOGUI*/
};

struct WireContainer : TransparentWidget {
	WireWidget *activeWire = NULL;
	/** Takes ownership of `w` and adds it as a child if it isn't already */
	void setActiveWire(WireWidget *w);
	/** "Drops" the wire onto the port, making an engine connection if successful */
	void commitActiveWire();
	void removeTopWire(Port *port);
	void removeAllWires(Port *port);
	/** Returns the most recently added wire connected to the given Port, i.e. the top of the stack */
	WireWidget *getTopWire(Port *port);
#ifndef RACK_NOGUI
	void draw(NVGcontext *vg) override;
#endif /*RACK_NOGUI*/
};

struct RackWidget : OpaqueWidget {
#ifndef RACK_NOGUI
	FramebufferWidget *rails;
#endif /*RACK_NOGUI*/
	// Only put ModuleWidgets in here
	Widget *moduleContainer;
	// Only put WireWidgets in here
	WireContainer *wireContainer;
	std::string lastPath;
	Vec lastMousePos;

	RackWidget();
	~RackWidget();

	/** Completely clear the rack's modules and wires */
	void clear();
	/** Clears the rack and loads the template patch */
	void reset();
#ifndef RACK_NOGUI
	void openDialog();
	void saveDialog();
	void saveAsDialog();
#endif /*RACK_NOGUI*/
	void savePatch(std::string filename);
	void loadPatch(std::string filename);
	json_t *toJson();
	void fromJson(json_t *rootJ);

	void addModule(ModuleWidget *m);
	/** Transfers ownership to the caller so they must `delete` it if that is the intension */
	void deleteModule(ModuleWidget *m);
	void cloneModule(ModuleWidget *m);
#ifndef RACK_NOGUI
	/** Sets a module's box if non-colliding. Returns true if set */
	bool requestModuleBox(ModuleWidget *m, Rect box);
	/** Moves a module to the closest non-colliding position */
	bool requestModuleBoxNearest(ModuleWidget *m, Rect box);
#endif /*RACK_NOGUI*/
	void step() override;
#ifndef RACK_NOGUI
	void draw(NVGcontext *vg) override;

	Widget *onMouseMove(Vec pos, Vec mouseRel) override;
	void onMouseDownOpaque(int button) override;
	void onZoom() override;
#endif /*RACK_NOGUI*/
};

#ifndef RACK_NOGUI
struct RackRail : TransparentWidget {
	void draw(NVGcontext *vg) override;
};

struct Panel : TransparentWidget {
	NVGcolor backgroundColor;
	std::shared_ptr<Image> backgroundImage;
	void draw(NVGcontext *vg) override;
};

struct SVGPanel : FramebufferWidget {
	void step() override;
	void setBackground(std::shared_ptr<SVG> svg);
};
#endif /*RACK_NOGUI*/

////////////////////
// params
////////////////////

#ifndef RACK_NOGUI
struct CircularShadow : TransparentWidget {
	float blur = 0.0;
	void draw(NVGcontext *vg) override;
};
#endif /*RACK_NOGUI*/

struct ParamWidget : OpaqueWidget, QuantityWidget {
	Module *module = NULL;
	int paramId;
	/** Used to momentarily disable value randomization
	To permanently disable or change randomization behavior, override the randomize() method instead of changing this.
	*/
	bool randomizable = true;

	json_t *toJson();
	void fromJson(json_t *rootJ);
	virtual void randomize();
#ifndef RACK_NOGUI
	void onMouseDownOpaque(int button) override;
#endif /*RACK_NOGUI*/
	void onChange() override;
};

#ifndef RACK_NOGUI
/** Implements vertical dragging behavior for ParamWidgets */
struct Knob : ParamWidget {
	/** Snap to nearest integer while dragging */
	bool snap = false;
	float dragValue;
	void onDragStart() override;
	void onDragMove(Vec mouseRel) override;
	void onDragEnd() override;
	/** Tell engine to smoothly vary this parameter */
	void onChange() override;
};

struct SpriteKnob : virtual Knob, SpriteWidget {
	int minIndex, maxIndex, spriteCount;
	void step() override;
};

/** A knob which rotates an SVG and caches it in a framebuffer */
struct SVGKnob : virtual Knob, FramebufferWidget {
	/** Angles in radians */
	float minAngle, maxAngle;
	/** Not owned */
	TransformWidget *tw;
	SVGWidget *sw;

	SVGKnob();
	void setSVG(std::shared_ptr<SVG> svg);
	void step() override;
	void onChange() override;
};

struct SVGSlider : Knob, FramebufferWidget {
	/** Intermediate positions will be interpolated between these positions */
	Vec minHandlePos, maxHandlePos;
	/** Not owned */
	SVGWidget *background;
	SVGWidget *handle;

	SVGSlider();
	void step() override;
	void onChange() override;
};

struct Switch : ParamWidget {
};

struct SVGSwitch : virtual Switch, FramebufferWidget {
	std::vector<std::shared_ptr<SVG>> frames;
	/** Not owned */
	SVGWidget *sw;

	SVGSwitch();
	/** Adds an SVG file to represent the next switch position */
	void addFrame(std::shared_ptr<SVG> svg);
	void step() override;
	void onChange() override;
};

/** A switch that cycles through each mechanical position */
struct ToggleSwitch : virtual Switch {
	void onDragStart() override {
		// Cycle through values
		// e.g. a range of [0.0, 3.0] would have modes 0, 1, 2, and 3.
		if (value >= maxValue)
			setValue(minValue);
		else
			setValue(value + 1.0);
	}
};

/** A switch that is turned on when held */
struct MomentarySwitch : virtual Switch {
	/** Don't randomize state */
	void randomize() override {}
	void onDragStart() override {
		setValue(maxValue);
	}
	void onDragEnd() override {
		setValue(minValue);
	}
};
#endif /*RACK_NOGUI*/

////////////////////
// ports
////////////////////

struct Port : OpaqueWidget {
	enum PortType {
		INPUT,
		OUTPUT
	};

	Module *module = NULL;
	PortType type = INPUT;
	int portId;

	~Port();
#ifndef RACK_NOGUI
	void draw(NVGcontext *vg) override;
	void onMouseDownOpaque(int button) override;
	void onDragEnd() override;
	void onDragStart() override;
	void onDragDrop(Widget *origin) override;
	void onDragEnter(Widget *origin) override;
	void onDragLeave(Widget *origin) override;
#endif /*RACK_NOGUI*/
};

#ifndef RACK_NOGUI
struct SVGPort : Port, FramebufferWidget {
	SVGWidget *background;

	SVGPort();
	void draw(NVGcontext *vg) override;
};

/** If you don't add these to your ModuleWidget, they will fall out of the rack... */
struct SVGScrew : FramebufferWidget {
	SVGWidget *sw;

	SVGScrew();
};
#endif /*RACK_NOGUI*/

#ifndef RACK_NOGUI
////////////////////
// lights
////////////////////

struct LightWidget : TransparentWidget {
	NVGcolor bgColor = nvgRGBf(0, 0, 0);
	NVGcolor color = nvgRGBf(1, 1, 1);
	void draw(NVGcontext *vg) override;
};

/** A LightWidget that points to a module's Light or a range of lights */
struct ModuleLightWidget : LightWidget {
	Module *module = NULL;
	int lightId;
};

/** Mixes colors based on the brightness of the module light at lightId, lightId + 1, etc */
struct ColorLightWidget : ModuleLightWidget {
	std::vector<NVGcolor> colors;
	void addColor(NVGcolor c);
	void step() override;
};
#endif /*RACK_NOGUI*/

#ifndef RACK_NOGUI
////////////////////
// scene
////////////////////

struct Toolbar : OpaqueWidget {
	Slider *wireOpacitySlider;
	Slider *wireTensionSlider;
	Slider *zoomSlider;
	RadioButton *cpuUsageButton;
	RadioButton *plugLightButton;

	Toolbar();
	void draw(NVGcontext *vg) override;
};
#endif /*RACK_NOGUI*/

struct PluginManagerWidget : Widget {
	Widget *loginWidget;
	Widget *manageWidget;
	Widget *downloadWidget;
	PluginManagerWidget();
	void step() override;
};

#ifndef RACK_NOGUI
struct RackScrollWidget : ScrollWidget {
	void step() override;
};
#endif /*RACK_NOGUI*/

struct RackScene : Scene {
#ifndef RACK_NOGUI
	ScrollWidget *scrollWidget;
	ZoomWidget *zoomWidget;
#endif /*RACK_NOGUI*/

	RackScene();
	void step() override;
#ifndef RACK_NOGUI
	void draw(NVGcontext *vg) override;
	Widget *onHoverKey(Vec pos, int key) override;
#endif /*RACK_NOGUI*/
};

////////////////////
// globals
////////////////////

extern std::string gApplicationName;
extern std::string gApplicationVersion;
extern std::string gApiHost;

// Easy access to "singleton" widgets
extern RackScene *gRackScene;
extern RackWidget *gRackWidget;
#ifndef RACK_NOGUI
extern Toolbar *gToolbar;
#endif

void sceneInit();
void sceneDestroy();

} // namespace rack
