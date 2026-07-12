# JsonLib - 1.1.0 - 2026-07-12

## Added

- `Makefile` wrapping the CMake build. `make build` compiles everything, `make test` runs
  the unit tests, and `make help` (the default target) lists the rest.
- Regression tests for all of the fixes below (`ZeroAndNegativeFloats`, `SurrogatePairs`,
  and `LargeKeyAndDeepNesting`).
- `ctest` integration. The test executable is registered with `add_test`, and `make ctest`
  runs it portably.
- Documentation built with `wj-mkdocs`. `make docs` produces a self-contained HTML site in
  `html/`, and `make publish-docs` publishes it to the WaterJuice docs site via `wj-publish`.

## Fixed

- Heap buffer overflow when writing a dictionary key larger than the output buffer's grow
  block (~32 KB). The buffer now grows by whole blocks until the data fits.
- Test harness (WjTestLib) always exited with a success code even when tests failed, because
  the per-test result was discarded in the run loop. A failing assert now produces a non-zero
  exit code, so `make test`, `make ctest`, and CI can actually gate on it. A memory leak in a
  test now also fails the run, matching how the printed summary already reported it.
- Test helper `FloatsAreEqual` divided by the expected value, giving a divide-by-zero (NaN,
  wrongly reported as unequal) when comparing against zero. It now compares relative to the
  larger magnitude.
- Missing nesting-depth bound on the output side: serializing an object tree nested deeper
  than `MAX_JSON_DEPTH` overran the fixed process stack. It now returns
  `JL_STATUS_JSON_NESTING_TOO_DEEP`, matching the parser.
- Unmarshalling a number into a `float` field rejected zero, negatives, and subnormals
  because the lower bound was `FLT_MIN` (smallest positive normal) instead of `-FLT_MAX`.
- Zero-valued float literals (`0.0`, `-0.0`) were rejected as invalid data during parsing.
- Surrogate-pair decoding validated the wrong half, accepting an invalid low surrogate
  (>= 0xE000) instead of rejecting it.
- Unchecked size multiplication when allocating an unmarshalled array could overflow
  `size_t` on 32-bit platforms; oversized lists now return `JL_STATUS_TOO_MANY_ITEMS`.

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
