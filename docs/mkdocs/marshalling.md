# Marshalling Reference

The mapping between C structure fields and JSON is provided by an array of `JlMarshallElement`. This
array should only be created using the `JlMarshall*` macros defined in `JlMarshallTypes.h`.

A simple example that maps a JSON dictionary to the structure `MyStruct`:

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

Which maps this JSON:

```json
{
    "bool": true,
    "str": "a string",
    "num": 1234
}
```

Throughout the reference below, count fields must be an unsigned integer of 8, 16, 32, or 64 bits;
`size_t` is recommended.

## Strings

| Macro | Maps to |
| --- | --- |
| `JlMarshallString( STRUCT, Field, Key )` | A string, to an allocated `char*` field. |
| `JlMarshallStringFixed( STRUCT, Field, Key )` | A string, to a fixed-size `char[n]` field. |
| `JlMarshallStringArray( STRUCT, Field, CountField, Key )` | A list of strings, to an allocated `char**` array plus count. |
| `JlMarshallStringFixedArray( STRUCT, Field, CountField, Key )` | A list of strings, to a fixed `char*[n]` array plus count. |

## Structs

| Macro | Maps to |
| --- | --- |
| `JlMarshallStruct( STRUCT, Field, Key, Description, DescriptionCount )` | A dictionary, to another struct with its own mapping. |
| `JlMarshallStructArray( STRUCT, Field, CountField, Key, SUBSTRUCT, Description, DescriptionCount )` | A list of dictionaries, to an allocated array of structs. |
| `JlMarshallStructFixedArray( STRUCT, Field, CountField, Key, SUBSTRUCT, Description, DescriptionCount )` | A list of dictionaries, to a fixed-size array of structs. |

`SUBSTRUCT` is the C type of the sub-structure.

## Unsigned integers

Unsigned fields may be 8, 16, 32, or 64 bits.

| Macro | Maps to |
| --- | --- |
| `JlMarshallUnsigned( STRUCT, Field, Key )` | A number, to an unsigned integer. |
| `JlMarshallUnsignedArray( STRUCT, Field, CountField, Key )` | A list of numbers, to an allocated array of unsigned integers. |
| `JlMarshallUnsignedFixedArray( STRUCT, Field, CountField, Key )` | A list of numbers, to a fixed array of unsigned integers. |

The `Hex` variants behave identically but write the number as hexadecimal when output as JSON5. This does
not force the input JSON to be hex.

| Macro | Maps to |
| --- | --- |
| `JlMarshallUnsignedHex( STRUCT, Field, Key )` | A number, to an unsigned integer (hex on output). |
| `JlMarshallUnsignedHexArray( STRUCT, Field, CountField, Key )` | A list of numbers, to an allocated array (hex on output). |
| `JlMarshallUnsignedHexFixedArray( STRUCT, Field, CountField, Key )` | A list of numbers, to a fixed array (hex on output). |

## Signed integers

Signed fields may be 8, 16, 32, or 64 bits.

| Macro | Maps to |
| --- | --- |
| `JlMarshallSigned( STRUCT, Field, Key )` | A number, to a signed integer. |
| `JlMarshallSignedArray( STRUCT, Field, CountField, Key )` | A list of numbers, to an allocated array of signed integers. |
| `JlMarshallSignedFixedArray( STRUCT, Field, CountField, Key )` | A list of numbers, to a fixed array of signed integers. |

## Floating point

Float fields may be `float` or `double`.

| Macro | Maps to |
| --- | --- |
| `JlMarshallFloat( STRUCT, Field, Key )` | A number, to a float or double. |
| `JlMarshallFloatArray( STRUCT, Field, CountField, Key )` | A list of numbers, to an allocated array of floats or doubles. |
| `JlMarshallFloatFixedArray( STRUCT, Field, CountField, Key )` | A list of numbers, to a fixed array of floats or doubles. |

## Booleans

| Macro | Maps to |
| --- | --- |
| `JlMarshallBool( STRUCT, Field, Key )` | A bool, to a `bool` field. |
| `JlMarshallBoolArray( STRUCT, Field, CountField, Key )` | A list of bools, to an allocated array of `bool`. |
| `JlMarshallBoolFixedArray( STRUCT, Field, CountField, Key )` | A list of bools, to a fixed array of `bool`. |

## Binary data

Binary fields map a JSON string containing Base64 to a raw byte field.

| Macro | Maps to |
| --- | --- |
| `JlMarshallBinary( STRUCT, DataField, SizeField, Key )` | A Base64 string, to an allocated byte buffer plus size. |
| `JlMarshallBinaryFixed( STRUCT, DataField, Key )` | A Base64 string, to a fixed-size byte blob. |
