# JsonLib

A public-domain C library for marshalling C structures to and from JSON and
JSON5. Describe a struct once with a declarative mapping table, and the library
converts it in both directions.

It parses JSON5 or strict JSON, and outputs JSON or JSON5. By default it parses
JSON5 and outputs JSON, the most compatible combination. There are no external
dependencies.

*Created November 2019. Version 1.1.0, July 2026.*

## Documentation

Full documentation is at
[docs.waterjuice.org/JsonLib](https://docs.waterjuice.org/JsonLib/latest/): the
function reference, the complete mapping reference, and worked examples. The
source is under `docs/` and can be built locally with `make docs`.

Only `JsonLib.h` needs to be included to use the library.

## Example

Given a struct and a mapping table:

```c
typedef struct
{
    bool        BoolField;
    char*       StringField;
    uint64_t    U64Field;
} MyStruct;

JlMarshallElement myStructMap[] =
{
    JlMarshallBool(     MyStruct, BoolField,   "bool" ),
    JlMarshallString(   MyStruct, StringField, "str" ),
    JlMarshallUnsigned( MyStruct, U64Field,    "num" ),
};
```

parse this JSON into the struct:

```json
{ "bool": true, "str": "a string", "num": 1234 }
```

```c
MyStruct myStruct = {0};
size_t errorPos = 0;
JL_STATUS status = JlJsonToStruct(
    jsonString, myStructMap, sizeof(myStructMap) / sizeof(myStructMap[0]),
    &myStruct, &errorPos );
```

See the [documentation](https://docs.waterjuice.org/JsonLib/latest/) for more
examples, including nested structs, arrays, and Base64 binary fields.

## Building

JsonLib builds with CMake. A Makefile wraps the common tasks:

```bash
make build      # build the library, tools, tests, and documentation
make compile    # compile just the C code (no docs toolchain needed)
make test       # build and run the unit tests
make help       # list all targets
```

Or drive CMake directly:

```bash
cmake -B Build
cmake --build Build
```

The library builds to a static library, `libJsonLib.a`.

## Licence

Unlicense — public domain; see [UNLICENSE](UNLICENSE).
