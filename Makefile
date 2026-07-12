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

# Project name used for the documentation site path and the docs archive name.
PROJECT := JsonLib

# Version string for the documentation, derived from git (tag if present, else short commit). The
# leading "Version_" from this repo's tag is stripped so the version begins with a digit, which the
# docs publisher requires to parse the archive name.
VERSION_STR := $(shell git describe --tags --always 2>/dev/null | sed 's/^Version_//' | sed 's/-/.post.dev/' | sed 's/-g/-/')

.DEFAULT_GOAL := help

.PHONY: help build compile configure test ctest rewrite sample clean rebuild docs docs-deps publish-docs

help:
	@echo "JsonLib - available targets"
	@echo ""
	@echo "  make build       Build everything: the library, tools, tests, and documentation"
	@echo "  make compile     Compile just the C code (library, tools, tests) - no documentation"
	@echo "  make test        Build and run the unit tests (detailed output)"
	@echo "  make ctest       Build and run the unit tests via ctest (portable)"
	@echo "  make sample      Build and run JsonLibSample against SAMPLE_FILE"
	@echo "  make rewrite     Build the JsonRewrite tool (takes args, so run it yourself)"
	@echo "  make docs        Build the HTML documentation into html/ (uses wj-mkdocs)"
	@echo "  make publish-docs  Build and publish the docs to the WaterJuice docs site"
	@echo "  make clean       Delete the $(BUILD_DIR), html/ and output/ directories"
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

# Build everything - the C code and the documentation. "make compile" alone builds just the C code
# (and needs no docs toolchain); the docs step additionally needs uv (installed automatically).
build: compile docs

compile: configure
	cmake --build $(BUILD_DIR)

test: compile
	$(TESTS)

ctest: compile
	ctest --test-dir $(BUILD_DIR) --output-on-failure

sample: compile
	$(SAMPLE) $(SAMPLE_FILE)

rewrite: compile
	@echo "Built $(REWRITE)"
	@echo "Run it directly:  $(REWRITE) [options] <JsonFile>"
	@echo "Options:          -a ascii  -i indent  -x hex  -b bare  -s singlequote  -c comma"

# Build the HTML documentation with wj-mkdocs (the WaterJuice standard MkDocs wrapper). Output goes to
# html/, a self-contained site that can be opened directly or hosted anywhere.
docs: docs-deps
	rm -rf html/
	mkdir -p docs/mkdocs/_include
	cp CHANGELOG.md docs/mkdocs/_include/
	VERSION=$(VERSION_STR) uv run wj-mkdocs -f docs/mkdocs.yml -d docs/mkdocs -o html/
	cp docs/docinfo.* html/
	rm -rf docs/mkdocs/_include html/_include

# Ensure uv (and thus the docs toolchain) is available. Installs uv via pip if it is missing.
docs-deps:
	uv --version >/dev/null 2>&1 || pip3 install uv
	uv sync

# Package the built docs and publish them to the WaterJuice docs site with wj-publish. JsonLib is a
# C library with nothing to publish to PyPI, so only the docs are published (--docs-only). Publishing
# requires a wj-publish config with the "waterjuice" profile (see pyproject.toml [tool.wj-publish]).
publish-docs: docs
	rm -rf output/
	mkdir -p output
	cd html && uv run python -m zipfile -c ../output/$(PROJECT)-$(VERSION_STR)-docs.zip .
	uv run wj-publish --docs-only output/

clean:
	rm -rf $(BUILD_DIR) html/ output/

# Uses sub-makes to force clean-then-build ordering. Listing "clean build" as prerequisites would
# race under "make -j" (build could start before clean finished deleting the directory).
rebuild:
	$(MAKE) clean
	$(MAKE) build
