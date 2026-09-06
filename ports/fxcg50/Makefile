include ../../py/mkenv.mk

SH_CFLAGS := -DFXCG50
TARGETCASIO := "FXCG50"
SH_LDFLAGS := -T fxcg50.ld -ljustui-cg -lm -lgint-cg -lc -lgint-cg -lgcc

SH_ASSETS := img_modifier_states.png font_9.png font_13.png font_19.png PoliceNW
SH_METADATA := fxconv-metadata.txt
SH_CONVFLAGS := --cg

.PHONY: pythonultra-runtime-fixes
pythonultra-runtime-fixes:
	python3 ../sh/apply_runtime_fixes.py
	python3 ../sh/apply_pythonultra_features.py
	python3 ../sh/apply_pythonultra_ui2.py
	python3 ../sh/apply_pythonultra_permissions.py
	python3 -m py_compile \
		../sh/modules/pythonultra/__init__.py \
		../sh/modules/pyeditor/__init__.py \
		../sh/modules/pyfiles/__init__.py \
		../sh/modules/pyperm/__init__.py \
		../sh/modules/pyterm/__init__.py \
		../sh/modules/zipfile/__init__.py
	python3 ../../tests/ports/sh/pygame_compat.py
	python3 ../../tests/ports/sh/pygame_cg50_regression.py
	python3 ../../tests/ports/sh/numpy_compat.py
	python3 ../../tests/ports/sh/pythonultra_features.py

# The frozen Python modules must be patched and tested before firmware.bin is generated.
$(BUILD)/firmware.bin: pythonultra-runtime-fixes

all: PythonUltra.g3a

ICONS_RELEASE := --icon-uns=icon-uns.png --icon-sel=icon-sel.png
ICONS_DEV := --icon-uns=icon-uns-dev.png --icon-sel=icon-sel-dev.png

PythonUltra.g3a: $(BUILD)/firmware.bin
	python3 blacken_icon.py
	fxgxa --g3a -n PythonUltra $(ICONS_$(BUILDTYPE)) $< -o $@

send: all
	fxlink -sw PythonUltra.g3a

include $(TOP)/ports/sh/Makefile
