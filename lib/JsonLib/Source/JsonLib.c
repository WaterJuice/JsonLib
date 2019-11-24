////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This contains some common functions for JsonLib
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JsonLib.h"
#include "JlMemory.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    )
{
    JL_STATUS jlStatus;

    if(     NULL != pJsonStringBuffer
        &&  NULL != *pJsonStringBuffer )
    {
        JlFree( *pJsonStringBuffer );
        *pJsonStringBuffer = NULL;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

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
    )
{
    return JlJsonToStructEx( JsonString, StructDescription, StructDescriptionCount, false, true, Structure, pErrorAtPos );
}

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
    )
{
    JL_STATUS jlStatus;

    if(     NULL != JsonString
        &&  NULL != StructDescription
        &&  0 != StructDescriptionCount
        &&  NULL != Structure )
    {
        JlDataObject* objectTree = NULL;

        jlStatus = JlParseJsonEx( JsonString, AllowJson5, &objectTree, pErrorAtPos );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlUnmarshallToStructEx(
                objectTree,
                StructDescription, StructDescriptionCount,
                IgnoreExistingValuesInStruct,
                Structure,
                pErrorAtPos );

            // Now free the intermediate objectTree regardless of whether the unmarshall succeeded.
            (void) JlFreeObjectTree( &objectTree );
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

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
    )
{
    return JlStructToJsonEx(
        Structure,
        StructDescription,
        StructDescriptionCount,
        IndentedFormat ? JL_OUTPUT_FLAGS_INDENT : JL_OUTPUT_FLAGS_NONE,
        pJsonStringBuffer );
}

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
    )
{
    JL_STATUS jlStatus;

    if(     NULL != Structure
        &&  NULL != StructDescription
        &&  0 != StructDescriptionCount
        &&  NULL != pJsonStringBuffer )
    {
        JlDataObject* objectTree = NULL;

        *pJsonStringBuffer = NULL;

        jlStatus = JlMarshallFromStructToNewDataObject( Structure, StructDescription, StructDescriptionCount, &objectTree );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlOutputJsonEx( objectTree, OutputFlags, pJsonStringBuffer );

            // Now free the intermediate objectTree regardless of whether the unmarshall succeeded.
            (void) JlFreeObjectTree( &objectTree );
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}
