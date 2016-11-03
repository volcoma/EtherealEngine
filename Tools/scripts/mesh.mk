#
# Copyright 2011-2016 Branimir Karadzic. All rights reserved.
# License: http:/www.opensource.org/licenses/BSD-2-Clause
#

THISDIR:=$(dir $(lastword $(MAKEFILE_LIST)))
include $(THISDIR)/tools.mk

# Define MESHES_DIR if your meshes files are in a different dir than the makefile including this.
# Notice: If defined, MESHES_DIR should end with dir slash '/'.
# Example:
#     MESHES_DIR=_files/

ifndef TARGET
.PHONY: all
all:
	@echo Usage: make TARGET=# [clean, all, rebuild]
	@echo "  TARGET=0 (.obj)"
	@echo "  VERBOSE=1 show build commands."
else

ifeq ($(TARGET), 0)
FORMAT=.obj
endif

MESH_PATH=""

THISDIR := $(dir $(lastword $(MAKEFILE_LIST)))

BUILD_OUTPUT_DIR=$(addprefix ./, $(RUNTIME_DIR)/$(MESH_PATH))
BUILD_INTERMEDIATE_DIR=$(addprefix $(BUILD_DIR)/, $(MESH_PATH))

MESH_SOURCES=$(notdir $(wildcard $(addprefix $(MESHES_DIR), *$(FORMAT))))
MESH_BIN = $(addprefix $(BUILD_INTERMEDIATE_DIR)/, $(addsuffix .asset, $(basename $(notdir $(MESH_SOURCES)))))

BIN = $(MESH_BIN)

$(BUILD_INTERMEDIATE_DIR)/%.asset : $(MESHES_DIR)%$(FORMAT)
	@echo [$(<)]
	$(SILENT) $(GEOMETRYC) -o $(@) -f $(<) --tangent --barycentric --packnormal 1 --packuv 1
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