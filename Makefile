ARM=yes
EMBEDDED=yes

FLAGS += \
	-Iinclude \
	-Idep/include -Idep/lib/libzip/include

SOURCES = $(wildcard src/*.cpp src/*/*.cpp) \
	ext/nanovg/src/nanovg.c
ifeq ($(EMBEDDED),yes)
SOURCES := $(filter-out src/gui.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/FramebufferWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/ZoomWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/MenuLabel.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/RadioButton.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/QuantityWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/MenuEntry.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/TextField.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/ProgressBar.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/Tooltip.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/Slider.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/ScrollBar.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/ChoiceButton.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/Label.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/SpriteWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/SVGWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/ZoomWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/TransformWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/PasswordField.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/Menu.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/MenuItem.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/ScrollWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/Button.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/LightWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/SVGScrew.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/SVGSlider.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/SVGKnob.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/SVGPanel.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/SVGSwitch.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/SVGPort.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/RackRail.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/Knob.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/Toolbar.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/RackScrollWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/ColorLightWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/ColorLightWidget.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/SpriteKnob.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/Panel.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/CircularShadow.cpp, $(SOURCES))
SOURCES := $(filter-out src/app/Port.cpp, $(SOURCES))
SOURCES := $(filter-out src/widgets/MenuOverlay.cpp, $(SOURCES))
endif

include arch.mk

ifeq ($(ARCH), lin)
	LDFLAGS += -rdynamic \
		-lpthread -ldl \
		-Ldep/lib -ljansson -lsamplerate -lcurl -lzip -lportaudio -lrtmidi
	TARGET = Rack
endif

ifeq ($(ARCH), mac)
	SOURCES += ext/osdialog/osdialog_mac.m
	CXXFLAGS += -DAPPLE -stdlib=libc++
	LDFLAGS += -stdlib=libc++ -lpthread -ldl \
		-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo \
		-Ldep/lib -lGLEW -lglfw -ljansson -lsamplerate -lcurl -lzip -lportaudio -lrtmidi
	TARGET = Rack
endif

ifeq ($(ARCH), win)
	SOURCES += ext/osdialog/osdialog_win.c
	LDFLAGS += -static-libgcc -static-libstdc++ -lpthread \
		-Wl,--export-all-symbols,--out-implib,libRack.a -mwindows \
		-lgdi32 -lopengl32 -lcomdlg32 -lole32 \
		-Ldep/lib -lglew32 -lglfw3dll -lcurl -lzip -lportaudio_x64 -lrtmidi \
		-Wl,-Bstatic -ljansson -lsamplerate
	TARGET = Rack.exe
	OBJECTS = Rack.res
endif


all: $(TARGET)

dep:
	$(MAKE) -C dep

run: $(TARGET)
ifeq ($(ARCH), lin)
	LD_LIBRARY_PATH=dep/lib ./$<
endif
ifeq ($(ARCH), mac)
	DYLD_FALLBACK_LIBRARY_PATH=dep/lib ./$<
endif
ifeq ($(ARCH), win)
	# TODO get rid of the mingw64 path
	env PATH=dep/bin:/mingw64/bin ./$<
endif

debug: $(TARGET)
ifeq ($(ARCH), mac)
	lldb ./Rack
else
	LD_LIBRARY_PATH=dep/lib gdb -ex run ./Rack
endif

clean:
	rm -rfv $(TARGET) build dist

# For Windows resources
%.res: %.rc
	windres $^ -O coff -o $@

include compile.mk


ifeq ($(ARM),yes)
	CXXFLAGS+=-O3 -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ftree-vectorize -ffast-math -DNDEBUG -DRACK_ARM
	CFLAGS+=-O3 -march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -ftree-vectorize -ffast-math -DNDEBUG -DRACK_ARM
endif
ifeq ($(EMBEDDED),yes)
	CXXFLAGS+=-DRACK_NOGUI
	CFLAGS+=-DRACK_NOGUI
endif

dist: all
ifndef VERSION
	$(error VERSION must be defined when making distributables)
endif
	rm -rf dist
	$(MAKE) -C plugins/Fundamental dist

ifeq ($(ARCH), mac)
	$(eval BUNDLE := dist/$(TARGET).app)
	mkdir -p $(BUNDLE)
	mkdir -p $(BUNDLE)/Contents
	mkdir -p $(BUNDLE)/Contents/Resources
	cp Info.plist $(BUNDLE)/Contents/
	cp -R LICENSE* res $(BUNDLE)/Contents/Resources

	mkdir -p $(BUNDLE)/Contents/MacOS
	cp Rack $(BUNDLE)/Contents/MacOS/
	cp icon.icns $(BUNDLE)/Contents/Resources/

	otool -L $(BUNDLE)/Contents/MacOS/Rack

	cp dep/lib/libGLEW.2.1.0.dylib $(BUNDLE)/Contents/MacOS/
	cp dep/lib/libglfw.3.dylib $(BUNDLE)/Contents/MacOS/
	cp dep/lib/libjansson.4.dylib $(BUNDLE)/Contents/MacOS/
	cp dep/lib/libsamplerate.0.dylib $(BUNDLE)/Contents/MacOS/
	cp dep/lib/libcurl.4.dylib $(BUNDLE)/Contents/MacOS/
	cp dep/lib/libzip.5.dylib $(BUNDLE)/Contents/MacOS/
	cp dep/lib/libportaudio.2.dylib $(BUNDLE)/Contents/MacOS/
	cp dep/lib/librtmidi.4.dylib $(BUNDLE)/Contents/MacOS/

	install_name_tool -change /usr/local/lib/libGLEW.2.1.0.dylib @executable_path/libGLEW.2.1.0.dylib $(BUNDLE)/Contents/MacOS/Rack
	install_name_tool -change lib/libglfw.3.dylib @executable_path/libglfw.3.dylib $(BUNDLE)/Contents/MacOS/Rack
	install_name_tool -change $(PWD)/dep/lib/libjansson.4.dylib @executable_path/libjansson.4.dylib $(BUNDLE)/Contents/MacOS/Rack
	install_name_tool -change $(PWD)/dep/lib/libsamplerate.0.dylib @executable_path/libsamplerate.0.dylib $(BUNDLE)/Contents/MacOS/Rack
	install_name_tool -change $(PWD)/dep/lib/libcurl.4.dylib @executable_path/libcurl.4.dylib $(BUNDLE)/Contents/MacOS/Rack
	install_name_tool -change $(PWD)/dep/lib/libzip.5.dylib @executable_path/libzip.5.dylib $(BUNDLE)/Contents/MacOS/Rack
	install_name_tool -change $(PWD)/dep/lib/libportaudio.2.dylib @executable_path/libportaudio.2.dylib $(BUNDLE)/Contents/MacOS/Rack
	install_name_tool -change $(PWD)/dep/lib/librtmidi.4.dylib @executable_path/librtmidi.4.dylib $(BUNDLE)/Contents/MacOS/Rack

	otool -L $(BUNDLE)/Contents/MacOS/Rack

	mkdir -p $(BUNDLE)/Contents/Resources/plugins
	cp -R plugins/Fundamental/dist/Fundamental $(BUNDLE)/Contents/Resources/plugins
endif
ifeq ($(ARCH), win)
	mkdir -p dist/Rack
	cp -R LICENSE* res dist/Rack/
	cp Rack.exe dist/Rack/
	cp /mingw64/bin/libwinpthread-1.dll dist/Rack/
	cp /mingw64/bin/zlib1.dll dist/Rack/
	cp /mingw64/bin/libstdc++-6.dll dist/Rack/
	cp /mingw64/bin/libgcc_s_seh-1.dll dist/Rack/
	cp dep/bin/glew32.dll dist/Rack/
	cp dep/bin/glfw3.dll dist/Rack/
	cp dep/bin/libcurl-4.dll dist/Rack/
	cp dep/bin/libjansson-4.dll dist/Rack/
	cp dep/bin/librtmidi-4.dll dist/Rack/
	cp dep/bin/libsamplerate-0.dll dist/Rack/
	cp dep/bin/libzip-5.dll dist/Rack/
	cp dep/bin/portaudio_x64.dll dist/Rack/
	mkdir -p dist/Rack/plugins
	cp -R plugins/Fundamental/dist/Fundamental dist/Rack/plugins/
endif
ifeq ($(ARCH), lin)
	mkdir -p dist/Rack
	cp -R LICENSE* res dist/Rack/
	cp Rack Rack.sh dist/Rack/
	cp dep/lib/libsamplerate.so.0 dist/Rack/
	cp dep/lib/libjansson.so.4 dist/Rack/
	cp dep/lib/libcurl.so.4 dist/Rack/
	cp dep/lib/libzip.so.5 dist/Rack/
ifneq ($(EMBEDDED),)
	cp dep/lib/libglfw.so.3 dist/Rack/
	cp dep/lib/libGLEW.so.2.1 dist/Rack/
	cp dep/lib/libportaudio.so.2 dist/Rack/
endif
	cp dep/lib/librtmidi.so.4 dist/Rack/
	mkdir -p dist/Rack/plugins
	cp -R plugins/Fundamental/dist/Fundamental dist/Rack/plugins/
endif

ifeq ($(ARCH), mac)
	cd dist && ln -s /Applications Applications
	cd dist && hdiutil create -srcfolder . -volname Rack -ov -format UDZO Rack-$(VERSION)-$(ARCH).dmg
else
	cd dist && zip -5 -r Rack-$(VERSION)-$(ARCH).zip Rack
endif


# Plugin helpers

allplugins:
	for f in plugins/*; do $(MAKE) -C "$$f"; done

cleanplugins:
	for f in plugins/*; do $(MAKE) -C "$$f" clean; done

distplugins:
	for f in plugins/*; do $(MAKE) -C "$$f" dist; done

plugins:
	for f in plugins/*; do (cd "$$f" && ${CMD}); done

.PHONY: all dep run debug clean dist allplugins cleanplugins distplugins plugins
