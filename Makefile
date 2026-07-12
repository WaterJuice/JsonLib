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

.PHONY: help build configure test ctest rewrite sample clean rebuild

help:
	@echo "JsonLib - available targets"
	@echo ""
	@echo "  make build       Configure (if needed) and build everything"
	@echo "  make test        Build and run the unit tests (detailed output)"
	@echo "  make ctest       Build and run the unit tests via ctest (portable)"
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

# Always invoke cmake configure. It is idempotent and cheap: it only regenerates when something
# actually changed. Running it every time (rather than gating on a cache file) means a changed
# BUILD_TYPE is picked up instead of being silently ignored because the cache already exists.
configure:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	cmake --build $(BUILD_DIR)

test: build
	$(TESTS)

ctest: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

sample: build
	$(SAMPLE) $(SAMPLE_FILE)

rewrite: build
	@echo "Built $(REWRITE)"
	@echo "Run it directly:  $(REWRITE) [options] <JsonFile>"
	@echo "Options:          -a ascii  -i indent  -x hex  -b bare  -s singlequote  -c comma"

clean:
	rm -rf $(BUILD_DIR)

# Uses sub-makes to force clean-then-build ordering. Listing "clean build" as prerequisites would
# race under "make -j" (build could start before clean finished deleting the directory).
rebuild:
	$(MAKE) clean
	$(MAKE) build
