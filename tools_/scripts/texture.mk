#
# Copyright 2011-2016 Branimir Karadzic. All rights reserved.
# License: http:/www.opensource.org/licenses/BSD-2-Clause
#

THISDIR:=$(dir $(lastword $(MAKEFILE_LIST)))
include $(THISDIR)/tools.mk

# Define TEXTURES_DIR if your meshes files are in a different dir than the makefile including this.
# Notice: If defined, TEXTURES_DIR should end with dir slash '/'.
# Example:
#     TEXTURES_DIR=_files/

ifndef TARGET
.PHONY: all
all:
	@echo Usage: make TARGET=# [clean, all, rebuild]
	@echo "  TARGET=0 (.png)"
	@echo "  TARGET=1 (.tga)"
	@echo "  TARGET=2 (.dds)"
	@echo "  TARGET=3 (.ktx)"
	@echo "  TARGET=4 (.pvr)"
	@echo "  VERBOSE=1 show build commands."
else


ifeq ($(TARGET), 0)
FORMAT=.png
else
ifeq ($(TARGET), 1)
FORMAT=.tga
else
ifeq ($(TARGET), 2)
FORMAT=.dds
else
ifeq ($(TARGET), 3)
FORMAT=.ktx
else
ifeq ($(TARGET), 4)
FORMAT=.pvr
else
endif
endif
endif
endif
endif

TEXTURE_PATH=""

THISDIR := $(dir $(lastword $(MAKEFILE_LIST)))

BUILD_OUTPUT_DIR=$(addprefix ./, $(RUNTIME_DIR)/$(TEXTURE_PATH))
BUILD_INTERMEDIATE_DIR=$(addprefix $(BUILD_DIR)/, $(TEXTURE_PATH))

TEXTURE_SOURCES=$(notdir $(wildcard $(addprefix $(TEXTURES_DIR), *$(FORMAT))))
TEXTURE_BIN = $(addprefix $(BUILD_INTERMEDIATE_DIR)/, $(addsuffix .asset, $(basename $(notdir $(TEXTURE_SOURCES)))))

BIN = $(TEXTURE_BIN)

$(BUILD_INTERMEDIATE_DIR)/%.asset : $(TEXTURES_DIR)%$(FORMAT)
	@echo [$(<)]
	$(SILENT) $(TEXTUREC) -o $(@) -f $(<) -m -t RGBA8
	$(SILENT) cp $(@) $(BUILD_OUTPUT_DIR)/$(@F)


.PHONY: all
all: dirs $(BIN)
	@echo Target $(FORMAT)

.PHONY: clean
clean:
	@echo Cleaning...
	@-rm -vf $(BIN)
	@-$(call CMD_RMDIR,$(BUILD_INTERMEDIATE_DIR))

.PHONY: dirs
dirs:
	@-$(call CMD_MKDIR,$(BUILD_INTERMEDIATE_DIR))
	@-$(call CMD_MKDIR,$(BUILD_OUTPUT_DIR))

.PHONY: rebuild
rebuild: clean all
endif # TARGET
