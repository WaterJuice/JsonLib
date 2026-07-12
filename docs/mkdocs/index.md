# JsonLib

JsonLib is a public-domain C library that makes converting C structures to and from JSON easy. You
describe your struct once with a declarative mapping table, and the library handles marshalling (struct
to JSON) and unmarshalling (JSON to struct) in both directions.

The library can parse JSON5, or run in strict JSON mode, and can output either JSON or JSON5. By default
it parses JSON5 and outputs JSON, which is the most compatible combination.

## Features

- Marshal C structures to JSON, and unmarshal JSON back into C structures.
- Parse JSON5 or strict JSON; output JSON or JSON5 with control over indenting, ASCII escaping, bare
  keywords, single quotes, trailing commas, and hex numbers.
- Field types: strings, sub-structs, signed and unsigned integers (8/16/32/64-bit), hex integers,
  floats and doubles, bools, and Base64-encoded binary blobs — each available as a single value, an
  allocated array, or a fixed-size array.
- No external dependencies. Builds with CMake.
- Released into the public domain under the [Unlicense](licence.md).

## Quick example

Given a struct and a mapping table:

```c
typedef struct
{
    bool        BoolField;
    char*       StringField;
    uint64_t    U64Field;
} MyStruct;

JlMarshallElement myStructMappings[] =
{
    JlMarshallBool( MyStruct, BoolField, "bool" ),
    JlMarshallString( MyStruct, StringField, "str" ),
    JlMarshallUnsigned( MyStruct, U64Field, "num" )
};
```

parse this JSON into the struct:

```json
{
    "bool": true,
    "str": "a string",
    "num": 1234
}
```

```c
MyStruct myStruct = {0};
size_t errorAtPos = 0;

JL_STATUS status = JlJsonToStruct(
    jsonString,
    myStructMappings, sizeof(myStructMappings) / sizeof(myStructMappings[0]),
    &myStruct, &errorAtPos );
```

Only `JsonLib.h` needs to be included to use the library.

See the [Functions](functions.md) page for the full API, and the
[Marshalling Reference](marshalling.md) for every mapping macro.

## Building

JsonLib builds with CMake. From the repository root:

```bash
make build      # configure and compile everything
make test       # build and run the unit tests
make help       # list all targets
```

Or drive CMake directly:

```bash
cmake -B Build
cmake --build Build
```

The library builds to a static library (`libJsonLib.a`); the repository also builds a unit-test suite,
a `JsonRewrite` command-line tool, and a minimal sample program.
