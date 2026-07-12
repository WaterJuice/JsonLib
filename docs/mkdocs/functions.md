# Functions

All functions return a status code of type `JL_STATUS`. `JL_STATUS_SUCCESS` indicates success; all other
values are errors. They are defined in `JlStatus.h`.

The struct-to-JSON functions require that the root JSON object is a dictionary (to match a struct). They
cannot be used to process a JSON document that has a list as its root object.

## JlJsonToStruct

```c
JL_STATUS
    JlJsonToStruct
    (
        char const*                 JsonString,                     // [in]
        JlMarshallElement const*    StructDescription,              // [in]
        size_t                      StructDescriptionCount,         // [in]
        void*                       Structure,                      // [out]
        size_t*                     pErrorAtPos                     // [out] [OPTIONAL]
    );
```

Parses a JSON5 document in `JsonString` and unmarshals the data into `Structure` using the mapping in
`StructDescription`. Items in the JSON that have no mapping are ignored.

The JSON must have a dictionary as its root object. All JSON5 syntax is accepted. If an error occurs in
parsing or unmarshalling, an error value is returned and `*pErrorAtPos` is set to the approximate
character position.

This function deallocates any existing values within the struct that are being replaced with new JSON
values (it does not deallocate the entire struct, only the items being replaced). To avoid any
deallocations, use `JlJsonToStructEx`.

Use `JlUnmarshallFreeStructAllocs` to free any allocations made in the structure by this function.

## JlJsonToStructEx

```c
JL_STATUS
    JlJsonToStructEx
    (
        char const*                 JsonString,                     // [in]
        JlMarshallElement const*    StructDescription,              // [in]
        size_t                      StructDescriptionCount,         // [in]
        bool                        IgnoreExistingValuesInStruct,   // [in]
        bool                        AllowJson5,                     // [in]
        void*                       Structure,                      // [in,out]
        size_t*                     pErrorAtPos                     // [out] [OPTIONAL]
    );
```

The extended form of `JlJsonToStruct` with two additional options.

`IgnoreExistingValuesInStruct` specifies whether the structure contains existing data from a previous
unmarshal that should be deallocated, or whether existing values should be ignored.

`AllowJson5` specifies whether JSON5 is accepted. If false, strict JSON rules are applied (including no
comments).

## JlStructToJson

```c
JL_STATUS
    JlStructToJson
    (
        void const*                 Structure,                      // [in]
        JlMarshallElement const*    StructDescription,              // [in]
        size_t                      StructDescriptionCount,         // [in]
        bool                        IndentedFormat,                 // [in]
        char**                      pJsonStringBuffer               // [out]
    );
```

Marshals the items in `Structure`, as defined by `StructDescription`, into a JSON string. Items in the
structure not defined in the mapping are ignored.

If `IndentedFormat` is true, indented formatting is applied: each item on its own line, four spaces of
indent per nesting level, and a space after the colon in dictionaries. If false, no extra whitespace is
emitted.

This outputs strict JSON. Use `JlStructToJsonEx` to allow JSON5 output.

Use `JlFreeJsonStringBuffer` to deallocate the JSON string when finished.

## JlStructToJsonEx

```c
JL_STATUS
    JlStructToJsonEx
    (
        void const*                 Structure,                      // [in]
        JlMarshallElement const*    StructDescription,              // [in]
        size_t                      StructDescriptionCount,         // [in]
        JL_OUTPUT_FLAGS             OutputFlags,                    // [in]
        char**                      pJsonStringBuffer               // [out]
    );
```

The extended form of `JlStructToJson`. `OutputFlags` is a set of bit flags controlling the output; with
no flags set, unformatted JSON is produced. Flags can be combined. Every flag with `J5` in its name
produces output that requires a JSON5 parser to read.

| Flag | Effect |
| --- | --- |
| `JL_OUTPUT_FLAGS_ASCII` | Escape all non-ASCII characters. |
| `JL_OUTPUT_FLAGS_INDENT` | Insert spaces and new lines to make the layout readable. |
| `JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS` | Use bare keywords in dictionaries when possible. |
| `JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES` | Use single quotes instead of double quotes for strings. |
| `JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS` | Place a trailing comma in lists and dictionaries (if indenting is chosen). |
| `JL_OUTPUT_FLAGS_J5_ALLOW_HEX` | Allow hex numbers to be written if they are marked as hex in the mapping. |

`JL_OUTPUT_FLAGS_JSON5` defines a standard set for JSON5 output. `JL_OUTPUT_FLAGS_NONE` defines no flags.

Use `JlFreeJsonStringBuffer` to deallocate the JSON string when finished.

## JlFreeJsonStringBuffer

```c
JL_STATUS
    JlFreeJsonStringBuffer
    (
        char**                  pJsonStringBuffer
    );
```

Frees the JSON string allocated by `JlStructToJson` or `JlStructToJsonEx`.

## JlUnmarshallFreeStructAllocs

```c
JL_STATUS
    JlUnmarshallFreeStructAllocs
    (
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        void*                       Structure
    );
```

Frees all the allocations made by `JlJsonToStruct` or `JlJsonToStructEx` in a structure. It does not
deallocate the structure itself (which may be a stack variable).

This uses `JlFree` to deallocate items, so all pointers must be allocated and not pointing to static or
constant data.
