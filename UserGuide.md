JsonLib - User Guide
====================

Version 1.0.0

Released: 24-Nov-2019

This is free and unencumbered software released into the public domain - November 2019 waterjuice.org

Introduction
============

This library is designed to make converting C structures to and from JSON easy. The library has
many support functions that will not be documented here. This guide only documents the main
struct-to-Json interfaces.

The library can parse JSON5, or be used in strict JSON mode. It can also output JSON or JSON5. By
default (and recommended for maximum compatibility) it will parse JSON5 and output in JSON.

The struct-to-Json functions require that the root JSON object will be a dictionary (to match up
to a struct). These functions can not be used to process a JSON file which has a list as its
root object.

The key to the library is struct->json maps defined as an array of `JlMarshallElement`. This structure
should only be created using the macros defined in `JlMarshallTypes.h`

Only `JsonLib.h` needs to be included in order to use the library.

Samples
=======

This includes a sample projcet called `JsonLibSample` which contains a very basic program that will
read and write a small JSON file. More examples of the possible `JlMarshallElement` mappings are
inside the `JsonLibTests` project.

Functions
=========

All functions return a status code of type `JL_STATUS`. `JL_STATUS_SUCCESS` indicates successs, all
other values are errors. They are defined in `JlStatus.h`

JlJsonToStruct
--------------

    JL_STATUS
        JlJsonToStruct
        (
            char const*                 JsonString,                     // [in]
            JlMarshallElement const*    StructDescription,              // [in]
            size_t                      StructDescriptionCount,         // [in]
            void*                       Structure,                      // [out]
            size_t*                     pErrorAtPos                     // [out] [OPTIONAL]
        );

This will parse a JSON5 document in `JsonString` and unmarshall the data into the struct `Structure` using the
mapping defined in `StructDescription`. Items in the JSON that do not have a mapping are simply ignored.

The JSON must have a dictionary as its root object. All JSON5 allowed syntax is accepted. If an error occurs
in the parsing or the unmarshalling an error value is returned and `*pErrorAtPos` will be set to the approximate
character position.

This function will deallocate any existing values within the struct that are being replaced with the new Json
values (note it will not deallocate the entire struct, just the items being replaced). To avoid any deallocations
at all use `JlJsonToStructEx`.

This function will allow JSON5 input. To restrict to strict JSON use `JlJsonStructToStruct`

The mapping type `JlMarshallElement` is described later in this guide.

Use function `JlUnmarshallFreeStructAllocs` to free any allocations that were made in the structure by this function.

JlJsonToStructEx
----------------

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

This extended function provides same ability as `JlJsonToStruct` with two additional options.

 `IgnoreExistingValuesInStruct` specifies whether the Structure contains existing data from a previous unmarshall that
 should be deallocated, or whether to ignore any existing values in the structure.

`AllowJson5` specifies whether JSON5 will be accepted, if false then strict JSON rules are applied (including no
comments allowed)

Use `JlUnmarshallFreeStructAllocs` to deallocate any allocations made in the structure by this function.

JlStructToJson
--------------

    JL_STATUS
        JlStructToJson
        (
            void const*                 Structure,                      // [in]
            JlMarshallElement const*    StructDescription,              // [in]
            size_t                      StructDescriptionCount,         // [in]
            bool                        IndentedFormat,                 // [in]
            char**                      pJsonStringBuffer               // [out]
        );

Marshalls the items in `Structure` as defined by `StructDescription` into a JSON string. Items in the structure
that are not defined in the mapping will be ignored.

If `IndentedFormat` is true then indented formatting will be applied to the string. This will put all items on
separate lines and indent 4 spaces for each level of dictionary and list. Additionally a space will be placed
after the colon in dictionaries. If `IndentedFormat` is false then there are no extra white space characters.

This outputs strict JSON. Use `JlStructToJsonEx` to allow JSON5 output.

Use function `JlFreeJsonStringBuffer` to deallocate the JSON string when finished.

JlStructToJsonEx
----------------

    JL_STATUS
        JlStructToJsonEx
        (
            void const*                 Structure,                      // [in]
            JlMarshallElement const*    StructDescription,              // [in]
            size_t                      StructDescriptionCount,         // [in]
            JL_OUTPUT_FLAGS             OutputFlags,                    // [in]
            char**                      pJsonStringBuffer               // [out]
        );

This extended function provides same capability as `JlStructToJson` with additional options.

`OutputFlags` is a set of bit flags that control the output. If no flags are set then JSON without formatting will be
output. Flags can be combined. All flags with J5 in the name will cause the output to require JSON5 parsing to read.

    * `JL_OUTPUT_FLAGS_ASCII` - All non-ascii characters will be escaped.
    * `JL_OUTPUT_FLAGS_INDENT` - Spaces and new lines will be inserted to make the layout readable.
    * `JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS` - Bare keywords will be used in dictionaries when possible.
    * `JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES` - Use single quotes instead of double quotes for strings.
    * `JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS` - Place a trailing comma in lists and dictionaries IF indenting is chosen.
    * `JL_OUTPUT_FLAGS_J5_ALLOW_HEX` - Allow hex numbers to be written if they are marked as hex in mapping.

`JL_OUTPUT_FLAGS_JSON5` - Defines a standard set for JSON5 output.

`JL_OUTPUT_FLAGS_NONE` - Defines no flags.

Use function `JlFreeJsonStringBuffer` to deallocate the JSON string when finished.

JlFreeJsonStringBuffer
----------------------

    JL_STATUS
        JlFreeJsonStringBuffer
        (
            char**                  pJsonStringBuffer
        );

Frees the JSON string allocated by `JlStructToJson` or 'JlStructToJsonEx`

JlUnmarshallFreeStructAllocs
----------------------------

    JL_STATUS
        JlUnmarshallFreeStructAllocs
        (
            JlMarshallElement const*    StructDescription,
            size_t                      StructDescriptionCount,
            void*                       Structure
        );

Frees all the allocations made by `JlJsonToStruct` or `JlJsonToStruct` in a structure. This does not deallocate the
structure itself (as it may be a stack variable).

This will use JlFree to deallocate any items. All pointers must be allocated and not pointing to static or constant
data.

JlMarshallElement Mapping
=========================

The mappings from C structure elements to JSON are provided by an array of `JlMarshallElement`. The array should only
be created using the `JlMarshall*` macros defined in `JlMarshallTypes.h`.

The following is a simple example. Following the example each element type for a mapping will be listed.

This creates a mapping from a JSON to the structure `MyStruct`.

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

 The JSON file is of format

    {
        "bool": true,
        "str": "a string",
        "num": 1234
    }

JlMarshallString
----------------

    JlMarshallString( STRUCT, StructStrField, KeyName )

Maps a string in a JSON dictionary to an allocated char* field within `STRUCT`

JlMarshallStringFixed
---------------------

    JlMarshallStringFixed( STRUCT, StructStrField, KeyName )

Maps a string in a JSON dictionary to a fixed size char[n] field within `STRUCT`

JlMarshallStringArray
---------------------

    JlMarshallStringArray( STRUCT, StructStrArrayField, StructStrArrayCountField, KeyName )

Maps a list of strings in a JSON dictionary to an array of strings char** field within `STRUCT` and a count field.
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t

JlMarshallStringFixedArray
--------------------------

    JlMarshallStringFixedArray( STRUCT, StructStrArrayField, StructStrArrayCountField, KeyName )

Maps a list of strings in a JSON dictionary to a fixed array of strings char* [n] field within `STRUCT` and a count
field. The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`

JlMarshallStruct
----------------

    JlMarshallStruct( STRUCT, StructStructField, KeyName, StructDescription, StructDescriptionCount )

Maps a dictionary within a JSON dictionary to another struct along with its structure mapping.
`StructDescription` is another array of JlMarshallElement

JlMarshallStructArray
---------------------

    JlMarshallStructArray( STRUCT, StructStructArrayField, StructStructArrayCountField, KeyName,
        SUBSTRUCT, StructDescription, StructDescriptionCount )

Maps a list of dictionaries within a JSON dictionary to an array of structs.
`SUBSTRUCT` is the struct C type for the structure.

JlMarshallStructFixedArray
--------------------------

    JlMarshallStructFixedArray( STRUCT, StructStructArrayField, StructStructArrayCountField, KeyName,
        SUBSTRUCT, StructDescription, StructDescriptionCount )

Maps a list of dictionaries within a JSON dictionary to a fixed size array of structs.
`SUBSTRUCT` is the struct C type for the structure.

JlMarshallUnsigned
------------------

    JlMarshallUnsigned( STRUCT, StructUnsignedField, KeyName )

Maps a number field within a JSON dictionary to an unsigned number of size 8, 16, 32, or 64 bits

JlMarshallUnsignedArray
-----------------------

    JlMarshallUnsignedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )

Maps a list of numbers within a JSON dictionary to an allocated array of unsigned ints size 8, 16, 32, or 64 bits
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`

JlMarshallUnsignedFixedArray
----------------------------

    JlMarshallUnsignedFixedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )

Maps a list of numbers within a JSON dictionary to a fixed array of unsigned ints size 8, 16, 32, or 64 bits
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`

JlMarshallUnsignedHex
---------------------

    JlMarshallUnsignedHex( STRUCT, StructUnsignedField, KeyName )

Maps a number field within a JSON dictionary to an unsigned number of size 8, 16, 32, or 64 bits.
If written out as Json5 the number will be written as Hex. Note this does not force the input json to be hex.

JlMarshallUnsignedHexArray
--------------------------

    JlMarshallUnsignedHexArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )

Maps a list of numbers within a JSON dictionary to an allocated array of unsigned ints size 8, 16, 32, or 64 bits
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`
If written out as Json5 the number will be written as Hex. Note this does not force the input json to be hex.

JlMarshallUnsignedHexFixedArray
-------------------------------

    JlMarshallUnsignedHexFixedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )

Maps a list of numbers within a JSON dictionary to a fixed array of unsigned ints size 8, 16, 32, or 64 bits
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`
If written out as Json5 the number will be written as Hex. Note this does not force the input json to be hex.

JlMarshallSigned
----------------

    JlMarshallSigned( STRUCT, StructSignedField, KeyName )

Maps a number field within a JSON dictionary to a signed number of size 8, 16, 32, or 64 bits

JlMarshallSignedArray
---------------------

    JlMarshallSignedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )

Maps a list of numbers within a JSON dictionary to an allocated array of signed ints size 8, 16, 32, or 64 bits
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`

JlMarshallSignedFixedArray
--------------------------

    JlMarshallSignedFixedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )

Maps a list of numbers within a JSON dictionary to a fixed array of signed ints size 8, 16, 32, or 64 bits
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`

JlMarshallFloat
---------------

    JlMarshallFloat( STRUCT, StructSignedField, KeyName )

Maps a number field within a JSON dictionary to a float or double type.

JlMarshallFloatArray
--------------------

    JlMarshallFloatArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )

Maps an array of numbers within a JSON dictionary to an allocated array of float or double type.
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`

JlMarshallFloatFixedArray
-------------------------

    JlMarshallFloatFixedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )

Maps an array of numbers within a JSON dictionary to a fixed array of float or double type.
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`

JlMarshallBool
--------------

    JlMarshallBool( STRUCT, StructBoolField, KeyName )

Maps a bool field within a JSON dictionary to a bool type

JlMarshallBoolArray
-------------------

    JlMarshallBoolArray( STRUCT, StructBoolArrayField, StructBoolArrayCountField, KeyName )

Maps a list of bools field within a JSON dictionary to an allocated array of `bool`
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`

JlMarshallBoolFixedArray
------------------------

    JlMarshallBoolFixedArray( STRUCT, StructBoolArrayField, StructBoolArrayCountField, KeyName )

Maps a list of bools field within a JSON dictionary to a fixed array of `bool`
The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use `size_t`

JlMarshallBinary
----------------

    JlMarshallBinary( STRUCT, StructDataField, StructDataSizeField, KeyName )

Maps a string containing Base64 in a JSON dictionary to an allocated arbitrary field within STRUCT

JlMarshallBinaryFixed
---------------------

    JlMarshallBinaryFixed( STRUCT, StructDataField, KeyName )

Maps a string containing Base64 in a JSON dictionary to a fixed sized arbitrary binary blob
