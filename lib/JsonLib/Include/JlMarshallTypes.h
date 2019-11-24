////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This defines JlMarshallElement which is used to map C structures to JSON file elements. To Marshall and Unmarshall
//  a JlDataObject tree into C structures an array of JlMarshallElement is used to define the root dictionary, this
//  in turn may reference other JlMarshallElement arrays (for dictionaries within dictionaries).
//
//  There is no need to deal with the JlMarshallElement items other than creating them using the macros defined here.
//  Each macro provides the definition to map a particular data type to an element within a structure.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string.h>
#include "JlStatus.h"
#include "JlDataModel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallElement
//
//  A structure for marshalling or unmarshalling has its mappings to JSON types defined as an array of JlMarshallElement
//  Each item in the array should be defined using one of the following macros.
//
//  For example:
//
//  typedef struct
//  {
//      bool        BoolField;
//      char*       StringField;
//      uint64_t    U64Field;
//  } MyStruct;
//
//  JlMarshallElement myStructMappings[] =
//  {
//      JlMarshallBool( MyStruct, BoolField, "bool" ),
//      JlMarshallString( MyStruct, StringField, "str" ),
//      JlMarshallUnsigned( MyStruct, U64Field, "num" )
//  };
//
//  This creates a mapping from a JSON to the structure MyStruct. The JSON file is of format
//  {
//      "bool": true,
//      "str": "a string",
//      "num": 1234
//  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct JlMarshallElement;
typedef struct JlMarshallElement JlMarshallElement;

struct JlMarshallElement
{
    JL_DATA_TYPE    Type;
    char const*     Name;
    bool            IsArray;

    size_t          FieldOffset;
    size_t          FieldSize;
    size_t          CountFieldOffset;
    size_t          CountFieldSize;

    size_t          ArrayFieldSize;
    size_t          ArrayItemSize;

    JL_NUM_TYPE     NumberType;
    bool            IsHex;          // Only relevant if NumberType is JL_NUM_TYPE_UNSIGNED
    bool            IsBase64;       // Only relevant if Type is JL_DATA_TYPE_STRING

    JlMarshallElement*  ChildStructDescription;
    uint32_t            ChildStructDescriptionCount;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MACROS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallString
//
//  Maps a string in a JSON dictionary to an allocated char* field within STRUCT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallString( STRUCT, StructStrField, KeyName )                                                 \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_STRING,                                                                        \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStrField))                                         \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallStringFixed
//
//  Maps a string in a JSON dictionary to a fixed size char[n] field within STRUCT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallStringFixed( STRUCT, StructStrField, KeyName )                                            \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_STRING,                                                                        \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStrField)),                                        \
        .FieldSize = sizeof((((STRUCT*)NULL)->StructStrField))                                              \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallStringArray
//
//  Maps a list of strings in a JSON dictionary to an array of strings char** field within STRUCT and a count field.
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallStringArray( STRUCT, StructStrArrayField, StructStrArrayCountField, KeyName )             \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_STRING,                                                                        \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStrArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStrArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructStrArrayCountField)),                              \
        .ArrayItemSize = sizeof(char*)                                                                      \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallStringFixedArray
//
//  Maps a list of strings in a JSON dictionary to a fixed array of strings char* [n] field within STRUCT and a count
//  field.
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallStringFixedArray( STRUCT, StructStrArrayField, StructStrArrayCountField, KeyName )        \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_STRING,                                                                        \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStrArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStrArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructStrArrayCountField)),                              \
        .ArrayFieldSize = sizeof((((STRUCT*)NULL)->StructStrArrayField)),                                   \
        .ArrayItemSize = sizeof(char*)                                                                      \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallStruct
//
//  Maps a dictionary within a JSON dictionary to another struct along with its structure mapping.
//  StructDescription is another array of JlMarshallElement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallStruct( STRUCT, StructStructField, KeyName, StructDescription, StructDescriptionCount )   \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_DICTIONARY,                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStructField)),                                     \
        .FieldSize = sizeof((((STRUCT*)NULL)->StructStructField)),                                          \
        .ChildStructDescription = (StructDescription),                                                      \
        .ChildStructDescriptionCount = (StructDescriptionCount)                                             \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallStructArray
//
//  Maps a list of dictionaries within a JSON dictionary to an array of structs.
//  SUBSTRUCT is the struct C type for the structure.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallStructArray( STRUCT, StructStructArrayField, StructStructArrayCountField, KeyName, SUBSTRUCT, StructDescription, StructDescriptionCount )       \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_DICTIONARY,                                                                    \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStructArrayField)),                                \
        .FieldSize = sizeof((((STRUCT*)NULL)->StructStructArrayField)),                                     \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStructArrayCountField)),                      \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructStructArrayCountField)),                           \
        .ArrayItemSize = sizeof( SUBSTRUCT ),                                                               \
        .ChildStructDescription = (StructDescription),                                                      \
        .ChildStructDescriptionCount = (StructDescriptionCount)                                             \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallStructFixedArray
//
//  Maps a list of dictionaries within a JSON dictionary to a fixed size array of structs.
//  SUBSTRUCT is the struct C type for the structure.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallStructFixedArray( STRUCT, StructStructArrayField, StructStructArrayCountField, KeyName, SUBSTRUCT, StructDescription, StructDescriptionCount )       \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_DICTIONARY,                                                                    \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStructArrayField)),                                \
        .FieldSize = sizeof((((STRUCT*)NULL)->StructStructArrayField)),                                     \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructStructArrayCountField)),                      \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructStructArrayCountField)),                           \
        .ArrayItemSize = sizeof( SUBSTRUCT ),                                                               \
        .ArrayFieldSize = sizeof((((STRUCT*)NULL)->StructStructArrayField)),                                \
        .ChildStructDescription = (StructDescription),                                                      \
        .ChildStructDescriptionCount = (StructDescriptionCount)                                             \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallUnsigned
//
//  Maps a number field within a JSON dictionary to an unsigned number of size 8, 16, 32, or 64 bits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallUnsigned( STRUCT, StructUnsignedField, KeyName )                                          \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructUnsignedField)),                                   \
        .FieldSize = sizeof(((STRUCT*)NULL)->StructUnsignedField),                                          \
        .NumberType = JL_NUM_TYPE_UNSIGNED                                                                  \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallUnsignedArray
//
//  Maps a list of numbers within a JSON dictionary to an allocated array of unsigned ints size 8, 16, 32, or 64 bits
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallUnsignedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )           \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayCountField)),                              \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                 \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                     \
        .NumberType = JL_NUM_TYPE_UNSIGNED                                                                  \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallUnsignedFixedArray
//
//  Maps a list of numbers within a JSON dictionary to a fixed array of unsigned ints size 8, 16, 32, or 64 bits
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallUnsignedFixedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )      \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayCountField)),                              \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                 \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                     \
        .ArrayFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .NumberType = JL_NUM_TYPE_UNSIGNED                                                                  \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallUnsignedHex
//
//  Maps a number field within a JSON dictionary to an unsigned number of size 8, 16, 32, or 64 bits.
//  If written out as Json5 the number will be written as Hex. Note this does not force the input json to be hex.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallUnsignedHex( STRUCT, StructUnsignedField, KeyName )                                       \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .IsHex = true,                                                                                      \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructUnsignedField)),                                   \
        .FieldSize = sizeof(((STRUCT*)NULL)->StructUnsignedField),                                          \
        .NumberType = JL_NUM_TYPE_UNSIGNED                                                                  \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallUnsignedHexArray
//
//  Maps a list of numbers within a JSON dictionary to an allocated array of unsigned ints size 8, 16, 32, or 64 bits
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
//  If written out as Json5 the number will be written as Hex. Note this does not force the input json to be hex.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallUnsignedHexArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )        \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .IsHex = true,                                                                                      \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayCountField)),                              \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                 \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                     \
        .NumberType = JL_NUM_TYPE_UNSIGNED                                                                  \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallUnsignedHexFixedArray
//
//  Maps a list of numbers within a JSON dictionary to a fixed array of unsigned ints size 8, 16, 32, or 64 bits
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
//  If written out as Json5 the number will be written as Hex. Note this does not force the input json to be hex.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallUnsignedHexFixedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )   \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .IsHex = true,                                                                                      \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayCountField)),                              \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                 \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                     \
        .ArrayFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .NumberType = JL_NUM_TYPE_UNSIGNED                                                                  \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallSigned
//
//  Maps a number field within a JSON dictionary to a signed number of size 8, 16, 32, or 64 bits
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallSigned( STRUCT, StructSignedField, KeyName )                                              \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructSignedField)),                                     \
        .FieldSize = sizeof(((STRUCT*)NULL)->StructSignedField),                                            \
        .NumberType = JL_NUM_TYPE_SIGNED                                                                    \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallSignedArray
//
//  Maps a list of numbers within a JSON dictionary to an allocated array of signed ints size 8, 16, 32, or 64 bits
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallSignedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )             \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayCountField)),                              \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                 \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                     \
        .NumberType = JL_NUM_TYPE_SIGNED                                                                    \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallSignedFixedArray
//
//  Maps a list of numbers within a JSON dictionary to a fixed array of signed ints size 8, 16, 32, or 64 bits
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallSignedFixedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )             \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayCountField)),                              \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                 \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                     \
        .ArrayFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .NumberType = JL_NUM_TYPE_SIGNED                                                                    \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallFloat
//
//  Maps a number field within a JSON dictionary to a float or double type.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallFloat( STRUCT, StructSignedField, KeyName )                                               \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructSignedField)),                                     \
        .FieldSize = sizeof(((STRUCT*)NULL)->StructSignedField),                                            \
        .NumberType = JL_NUM_TYPE_FLOAT                                                                     \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallFloatArray
//
//  Maps an array of numbers within a JSON dictionary to an allocated array of float or double type.
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallFloatArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )              \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayCountField)),                              \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                 \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                     \
        .NumberType = JL_NUM_TYPE_FLOAT                                                                     \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallFloatFixedArray
//
//  Maps an array of numbers within a JSON dictionary to a fixed array of float or double type.
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallFloatFixedArray( STRUCT, StructNumArrayField, StructNumArrayCountField, KeyName )              \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_NUMBER,                                                                        \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructNumArrayCountField)),                         \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayCountField)),                              \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                 \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructNumArrayField[0] ),                                     \
        .ArrayFieldSize = sizeof((((STRUCT*)NULL)->StructNumArrayField)),                                   \
        .NumberType = JL_NUM_TYPE_FLOAT                                                                     \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallBool
//
//  Maps a bool field within a JSON dictionary to a bool type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallBool( STRUCT, StructBoolField, KeyName )                                                  \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_BOOL,                                                                          \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructBoolField)),                                       \
        .FieldSize = sizeof(((STRUCT*)NULL)->StructBoolField)                                               \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallBoolArray
//
//  Maps a list of bools field within a JSON dictionary to an allocated array of bool
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallBoolArray( STRUCT, StructBoolArrayField, StructBoolArrayCountField, KeyName )             \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_BOOL,                                                                          \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructBoolArrayField)),                                  \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructBoolArrayCountField)),                        \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructBoolArrayCountField)),                             \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructBoolArrayField[0] ),                                \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructBoolArrayField[0] )                                     \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallBoolFixedArray
//
//  Maps a list of bools field within a JSON dictionary to a fixed array of bool
//  The count field must be of type unsigned int 8, 16, 32, or 64 bit in size. Recommended to use size_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallBoolFixedArray( STRUCT, StructBoolArrayField, StructBoolArrayCountField, KeyName )        \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_BOOL,                                                                          \
        .IsArray = true,                                                                                    \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructBoolArrayField)),                                  \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructBoolArrayCountField)),                        \
        .CountFieldSize = sizeof((((STRUCT*)NULL)->StructBoolArrayCountField)),                             \
        .ArrayItemSize = sizeof( ((STRUCT*)NULL)->StructBoolArrayField[0] ),                                \
        .FieldSize = sizeof( ((STRUCT*)NULL)->StructBoolArrayField[0] ),                                    \
        .ArrayFieldSize = sizeof((((STRUCT*)NULL)->StructBoolArrayField))                                   \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallBinary
//
//  Maps a string containing Base64 in a JSON dictionary to an allocated arbitrary field within STRUCT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallBinary( STRUCT, StructDataField, StructDataSizeField, KeyName )                           \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_STRING,                                                                        \
        .IsBase64 = true,                                                                                   \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructDataField)),                                       \
        .CountFieldOffset = (size_t)(&(((STRUCT*)NULL)->StructDataSizeField)),                              \
        .CountFieldSize = sizeof(((STRUCT*)NULL)->StructDataSizeField)                                      \
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallBinaryFixed
//
//  Maps a string containing Base64 in a JSON dictionary to a fixed sized arbitrary binary blob
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define JlMarshallBinaryFixed( STRUCT, StructDataField, KeyName )                                           \
    {                                                                                                       \
        .Type = JL_DATA_TYPE_STRING,                                                                        \
        .IsBase64 = true,                                                                                   \
        .Name = (KeyName),                                                                                  \
        .FieldOffset = (size_t)(&(((STRUCT*)NULL)->StructDataField)),                                       \
        .FieldSize = sizeof((((STRUCT*)NULL)->StructDataField))                                             \
    }
