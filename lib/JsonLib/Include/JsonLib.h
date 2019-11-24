////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This is the main interface to JsonLib. This defines the main public functions for parsing and creating JSON from
//  and to C structures.
//  JSON can be parsed to a JlDataObject* tree and then unmarshalled into structures. This can be done in one step
//  using the function JlJsonToStruct. Similarly structs can be marshalled in one step to JSON using JlStructToJson.
//  Alternatively the data model can be examined and manipulated using the functions in JlDataModel.h and
//  JlDataModelHelpers.h. This is also required for JSON that can't be represented in the structs. For example the
//  Marshalling code requires that lists contain the same data type for each element. Also they require that the root
//  item is a dictionary.
//
//  By default this library will use calloc and free from standard lib for memory allocation functions. This can be
//  changed by defining the macros JlAlloc and JlFree in a header file and then setting a global project define of
//  JL_INCLUDE_H to point to that file. This will allow the use of a different memory allocator, as long as the
//  prototype matches calloc and free.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "JlStatus.h"
#include "JlMarshallTypes.h"
#include "JlDataModel.h"
#include "JlDataModelHelpers.h"

#ifdef JL_INCLUDE_H
   #include JL_INCLUDE_H
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MEMORY ALLOCATION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// By default JlAlloc will be set as calloc and JlFree as free from stdlib. If you wish to use a different allocation
// function then define JlAlloc and JlFree inside the build settings (eg cmake file).
// JlAlloc requires a function that has same prototype as C malloc function BUT zeros the memory (eg like calloc)
// JlFree requires a function that has same prototype as C free function.
#ifndef JlAlloc
    #define JlAlloc( AllocSize )    calloc( (AllocSize), 1 )
    #define JlFree( Allocation )    free( Allocation )
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Defines the maximum depth of a JSON tree.
#define MAX_JSON_DEPTH              64

// Library version number. JL_VERSION_STR is of form x.x.x. JL_VERSION is a number of form:
// (MajorVersion * 10000) + (MinorVersion * 100) + SubMinorVersion
#define JL_VERSION                  10000
#define JL_VERSION_STR              "1.0.0"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The flags control the output. All flags with J5 will cause the output to require Json5 parsing to read.
typedef uint64_t JL_OUTPUT_FLAGS;
#define JL_OUTPUT_FLAGS_NONE                    ((JL_OUTPUT_FLAGS) 0x0 )
#define JL_OUTPUT_FLAGS_ASCII                   ((JL_OUTPUT_FLAGS) 0x1 )
#define JL_OUTPUT_FLAGS_INDENT                  ((JL_OUTPUT_FLAGS) 0x2 )
#define JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS    ((JL_OUTPUT_FLAGS) 0x4 )
#define JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES        ((JL_OUTPUT_FLAGS) 0x8 )
#define JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS      ((JL_OUTPUT_FLAGS) 0x10 )
#define JL_OUTPUT_FLAGS_J5_ALLOW_HEX            ((JL_OUTPUT_FLAGS) 0x20 )

// Define a standard set of defaults for Json5 output
#define JL_OUTPUT_FLAGS_JSON5   ( \
      JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS \
    | JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES \
    | JL_OUTPUT_FLAGS_J5_ALLOW_HEX \
    )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlParseJson
//
//  Parses JSON 5 in a string and returns a JlDataObject representing it.
//  If an error occurs (other than JL_STATUS_INVALID_PARAMETER) then *pErrorAtPos will be set with the position
//  within JsonString where the error occurred. pErrorAtPos is an OPTIONAL parameter
//  If string JSON 1 parsing is required use JlParseJsonEx
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlParseJson
    (
        char const*     JsonString,
        JlDataObject**  pRootObject,
        size_t*         pErrorAtPos
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlParseJsonEx
//
//  Parses JSON in a string and returns a JlDataObject representing it.
//  If an error occurs (other than JL_STATUS_INVALID_PARAMETER) then *pErrorAtPos will be set with the position
//  within JsonString where the error occurred. pErrorAtPos is an OPTIONAL parameter.
//  If IsJson5 is true then JSON 5 parsing will be done, otherwise strict JSON 1 parsing.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlParseJsonEx
    (
        char const*     JsonString,
        bool            IsJson5,
        JlDataObject**  pRootObject,
        size_t*         pErrorAtPos
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlFreeObjectTree
//
//  Frees an object and all items below it (if it is a dictionary or list)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlFreeObjectTree
    (
        JlDataObject**              pRootObject
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlOutputJson
//
//  Outputs JSON representing the JlDataObject. If IndentedFormat is true then spaces and new lines will be inserted
//  to make the layout readable. Otherwise the JSON will be as compact as possible with no white spacing.
//  *pJsonStringBuffer will be a pointer to a newly allocated string. Use JlFree to deallocate the string when
//  finished.
//  This will output in JSON 1 compliant format.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlOutputJson
    (
        JlDataObject const*     DataObject,
        bool                    IndentedFormat,
        char**                  pJsonStringBuffer
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlOutputJsonEx
//
//  Outputs JSON representing the JlDataObject. OutputFlags controls the output. If no flags are set then Json1
//  without formatting will be output. Flags can be combined.
//  Possible flags:
//      JL_OUTPUT_FLAGS_INDENT - Spaces and new lines will be inserted to make the layout readable.
//      JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS - Bare keywords will be used in dictionaries when possible
//      JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES - Use single quotes instead of double quotes for strings.
//      JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS - Place a trailing comma in lists and dictionaries IF indenting is chosen.
//      JL_OUTPUT_FLAGS_J5_ALLOW_HEX - Allow hex numbers to be written if they are marked as hex in data object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlOutputJsonEx
    (
        JlDataObject const*     DataObject,
        JL_OUTPUT_FLAGS         OutputFlags,
        char**                  pJsonStringBuffer
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlFreeJsonStringBuffer
//
//  Free's the buffer as created by JlOutputJson
//  *pJsonStringBuffer is set to NULL on return
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlFreeJsonStringBuffer
    (
        char**                  pJsonStringBuffer
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlUnmarshallToStruct
//
//  Unmarshall a dictionary JlDataObject (from a Json string that had a dictionary as the root) into a C structure.
//  StructDescription is an array describing the mapping of Json elements to the C structure elements. StructDescription
//  must be created using the JlMarshall* macros from JlMarshallTypes.h
//  For any item in the structure being filled in with data from DictionaryObject, this function will deallocate any
//  existing item (This is to allow partial fills from subsets of the JSON). To avoid deallocating anything use
//  JlUnmarshallToStructEx function
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlUnmarshallToStruct
    (
        JlDataObject const*         DictionaryObject,
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        void*                       Structure,
        size_t*                     pErrorAtPos                         // OPTIONAL
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlUnmarshallToStructEx
//
//  Unmarshall a dictionary JlDataObject (from a Json string that had a dictionary as the root) into a C structure.
//  StructDescription is an array describing the mapping of Json elements to the C structure elements. StructDescription
//  must be created using the JlMarshall* macros from JlMarshallTypes.h
//  For any item in the structure being filled in with data from DictionaryObject, this function will by default
//  deallocate any existing item (This is to allow partial fills from subsets of the JSON).
//  If IgnoreExistingValuesInStruct is true then this will not deallocate existing items and will just overwrite
//  the value.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlUnmarshallToStructEx
    (
        JlDataObject const*         DictionaryObject,
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        bool                        IgnoreExistingValuesInStruct,
        void*                       Structure,
        size_t*                     pErrorAtPos                         // OPTIONAL
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlUnmarshallFreeStructAllocs
//
//  Frees all the allocations made by JlUnmarshallToStruct in a structure. This does not deallocate the structure
//  itself (as it may be a stack variable).
//  This will use JlFree to deallocate any items. All pointers must be allocated and not pointing to static or
//  constant data.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlUnmarshallFreeStructAllocs
    (
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        void*                       Structure
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallFromStructToDataObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlMarshallFromStructToDataObject
    (
        void const*                 Structure,
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        JlDataObject*               DictionaryObject
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlMarshallFromStructToNewDataObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlMarshallFromStructToNewDataObject
    (
        void const*                 Structure,
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        JlDataObject**              pDictionaryObject
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlJsonToStruct
//
//  Combines JlParseJson and JlUnmarshallToStruct into one function. This will create and then free a JlDataObject.
//  To free the items allocated in the structure use JlUnmarshallFreeStructAllocs.
//  This function will deallocate any existing values within the struct that are being replaced with the new Json
//  values (note it will not deallocate the entire struct, just the items being replaced). To avoid any deallocations
//  at all use JlJsonToStructEx with IgnoreExistingValuesInStruct true
//  This function will allow Json5 input. To restrict to Json1 use JlJsonStructToStruct
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlJsonToStruct
    (
        char const*                 JsonString,                     // [in]
        JlMarshallElement const*    StructDescription,              // [in]
        size_t                      StructDescriptionCount,         // [in]
        void*                       Structure,                      // [out]
        size_t*                     pErrorAtPos                     // [out] [OPTIONAL]
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlJsonToStructEx
//
//  Combines JlParseJson and JlUnmarshallToStructEx into one function. This will create and then free a JlDataObject.
//  To free the items allocated in the structure use JlUnmarshallFreeStructAllocs.
//  This extended version specifies whether the Structure contains existing data from a previous unmarshall that should
//  be deallocated, or whether to ignore any existing values in struct
//  AllowJson5 specifies whether Json5 will be accepted.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlStructToJson
//
//  Combines JlMarshallFromStructToNewDataObject and JlOutputJson into one function. This will create and then free
//  a JlDataObject. Use JlFreeJsonStringBuffer to free the output string buffer.
//  This outputs strict Json1. Use JlStructToJsonEx to have more output options.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlStructToJson
    (
        void const*                 Structure,                      // [in]
        JlMarshallElement const*    StructDescription,              // [in]
        size_t                      StructDescriptionCount,         // [in]
        bool                        IndentedFormat,                 // [in]
        char**                      pJsonStringBuffer               // [out]
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlStructToJsonEx
//
//  Combines JlMarshallFromStructToNewDataObject and JlOutputJson into one function. This will create and then free
//  a JlDataObject. Use JlFreeJsonStringBuffer to free the output string buffer.
//  OutputFlags controls the output style, including Json5 options.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlStructToJsonEx
    (
        void const*                 Structure,                      // [in]
        JlMarshallElement const*    StructDescription,              // [in]
        size_t                      StructDescriptionCount,         // [in]
        JL_OUTPUT_FLAGS             OutputFlags,                    // [in]
        char**                      pJsonStringBuffer               // [out]
    );
