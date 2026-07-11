# Unreleased

## Added

- `Makefile` wrapping the CMake build. `make build` compiles everything, `make test` runs
  the unit tests, and `make help` (the default target) lists the rest.

## Removed

- `Source/DataModel.c`, a stale pre-rename copy of `JlDataModel.c`. It was not part of the
  build and could not compile, as the headers it included no longer exist.

# JsonLib - 1.0.0 - 2019-11-24

Initial release.

## Added

- Marshalling of C structures to JSON, and unmarshalling of JSON back into C structures,
  driven by a declarative `JlMarshallElement` mapping table built with the `JlMarshall*`
  macros.
- Parsing of both JSON5 and strict JSON. JSON5 is accepted by default; strict mode is
  available via the `Ex` functions.
- Output of both JSON and JSON5, with flags for indenting, ASCII escaping, bare keywords,
  single quotes, trailing commas, and hex numbers.
- Supported field mappings: strings, structs, unsigned and signed integers (8/16/32/64
  bit), hex formatted unsigned integers, floats and doubles, bools, and Base64 encoded
  binary blobs. Each is available as a single value, an allocated array, and a fixed size
  array.
- `JsonLibTests` unit test suite, built on the bundled `WjTestLib` test framework with
  allocation tracking for leak detection.
- `JsonRewrite` command line tool for reading a JSON file and writing it back out in a
  chosen format.
- `JsonLibSample` minimal example program.
