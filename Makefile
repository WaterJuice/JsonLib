####################################################################################################
#  JsonLib
#
#  Convenience wrapper around CMake. Run "make help" to see the available targets.
####################################################################################################

BUILD_DIR  ?= Build
BUILD_TYPE ?= Debug

# JsonLibSample reads this file, increments its run count, and writes it back out. It creates the
# file if it does not exist, so it defaults into the build dir rather than the source tree.
SAMPLE_FILE ?= $(BUILD_DIR)/SampleConfig.json

TESTS   := $(BUILD_DIR)/projects/JsonLibTests/JsonLibTests
SAMPLE  := $(BUILD_DIR)/projects/JsonLibSample/JsonLibSample
REWRITE := $(BUILD_DIR)/projects/JsonRewrite/JsonRewrite

.DEFAULT_GOAL := help

.PHONY: help build configure test rewrite sample clean rebuild

help:
	@echo "JsonLib - available targets"
	@echo ""
	@echo "  make build       Configure (if needed) and build everything"
	@echo "  make test        Build and run the unit tests"
	@echo "  make sample      Build and run JsonLibSample against SAMPLE_FILE"
	@echo "  make rewrite     Build the JsonRewrite tool (takes args, so run it yourself)"
	@echo "  make clean       Delete the $(BUILD_DIR) directory"
	@echo "  make rebuild     Clean and build from scratch"
	@echo "  make help        Show this message (default)"
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_DIR=$(BUILD_DIR)                       Directory to build in"
	@echo "  BUILD_TYPE=$(BUILD_TYPE)                      CMake build type (Debug, Release, ...)"
	@echo "  SAMPLE_FILE=$(SAMPLE_FILE)   Json file the sample reads and rewrites"
	@echo ""
	@echo "Example: make build BUILD_TYPE=Release"

# Re-runs whenever the top level CMakeLists.txt is newer than the generated cache.
$(BUILD_DIR)/CMakeCache.txt: CMakeLists.txt
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

configure: $(BUILD_DIR)/CMakeCache.txt

build: configure
	cmake --build $(BUILD_DIR)

test: build
	$(TESTS)

sample: build
	$(SAMPLE) $(SAMPLE_FILE)

rewrite: build
	@echo "Built $(REWRITE)"
	@echo "Run it directly:  $(REWRITE) [options] <JsonFile>"
	@echo "Options:          -a ascii  -i indent  -x hex  -b bare  -s singlequote  -c comma"

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean build
