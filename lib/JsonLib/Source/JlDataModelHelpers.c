////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This module contains helper "shortcut" functions for combining multiple JlDataModel functions in one.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JlDataModelHelpers.h"
#include "JlDataModel.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlCreateStringObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlCreateStringObject
    (
        char const*         String,             // [in]
        JlDataObject**      pStringObject       // [out]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* stringObject = NULL;

    if( NULL != pStringObject )
    {
        *pStringObject = NULL;

        jlStatus = JlCreateObject( JL_DATA_TYPE_STRING, &stringObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlSetObjectString( stringObject, String );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                *pStringObject = stringObject;
            }
            else
            {
                JlFreeObjectTree( &stringObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddStringToListObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddStringToListObject
    (
        JlDataObject*       ListObject,         // [in,out]
        char const*         String              // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* stringObject = NULL;

    if( NULL != ListObject )
    {
        jlStatus = JlCreateStringObject( String, &stringObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToListObject( ListObject, stringObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &stringObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddStringToDictionaryObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddStringToDictionaryObject
    (
        JlDataObject*       DictionaryObject,   // [in,out]
        char const*         KeyName,            // [in]
        char const*         String              // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* stringObject = NULL;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName )
    {
        jlStatus = JlCreateStringObject( String, &stringObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToDictionaryObject( DictionaryObject, KeyName, stringObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &stringObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlCreateNumberU64Object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlCreateNumberU64Object
    (
        uint64_t            NumberU64,          // [in]
        JlDataObject**      pNumberObject       // [out]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* numberObject = NULL;

    if( NULL != pNumberObject )
    {
        *pNumberObject = NULL;

        jlStatus = JlCreateObject( JL_DATA_TYPE_NUMBER, &numberObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlSetObjectNumberU64( numberObject, NumberU64 );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                *pNumberObject = numberObject;
            }
            else
            {
                JlFreeObjectTree( &numberObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddNumberU64ToListObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddNumberU64ToListObject
    (
        JlDataObject*       ListObject,         // [in,out]
        uint64_t            NumberU64           // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* numberObject = NULL;

    if( NULL != ListObject )
    {
        jlStatus = JlCreateNumberU64Object( NumberU64, &numberObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToListObject( ListObject, numberObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &numberObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddNumberU64ToDictionaryObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddNumberU64ToDictionaryObject
    (
        JlDataObject*       DictionaryObject,   // [in,out]
        char const*         KeyName,            // [in]
        uint64_t            NumberU64           // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* numberObject = NULL;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName )
    {
        jlStatus = JlCreateNumberU64Object( NumberU64, &numberObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToDictionaryObject( DictionaryObject, KeyName, numberObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &numberObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlCreateNumberS64Object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlCreateNumberS64Object
    (
        int64_t             NumberS64,          // [in]
        JlDataObject**      pNumberObject       // [out]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* numberObject = NULL;

    if( NULL != pNumberObject )
    {
        *pNumberObject = NULL;

        jlStatus = JlCreateObject( JL_DATA_TYPE_NUMBER, &numberObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlSetObjectNumberS64( numberObject, NumberS64 );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                *pNumberObject = numberObject;
            }
            else
            {
                JlFreeObjectTree( &numberObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddNumberS64ToListObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddNumberS64ToListObject
    (
        JlDataObject*       ListObject,         // [in,out]
        int64_t             NumberS64           // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* numberObject = NULL;

    if( NULL != ListObject )
    {
        jlStatus = JlCreateNumberS64Object( NumberS64, &numberObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToListObject( ListObject, numberObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &numberObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddNumberS64ToDictionaryObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddNumberS64ToDictionaryObject
    (
        JlDataObject*       DictionaryObject,   // [in,out]
        char const*         KeyName,            // [in]
        int64_t             NumberS64           // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* numberObject = NULL;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName )
    {
        jlStatus = JlCreateNumberS64Object( NumberS64, &numberObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToDictionaryObject( DictionaryObject, KeyName, numberObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &numberObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlCreateNumberF64Object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlCreateNumberF64Object
    (
        double              NumberF64,          // [in]
        JlDataObject**      pNumberObject       // [out]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* numberObject = NULL;

    if( NULL != pNumberObject )
    {
        *pNumberObject = NULL;

        jlStatus = JlCreateObject( JL_DATA_TYPE_NUMBER, &numberObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlSetObjectNumberF64( numberObject, NumberF64 );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                *pNumberObject = numberObject;
            }
            else
            {
                JlFreeObjectTree( &numberObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddNumberF64ToListObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddNumberF64ToListObject
    (
        JlDataObject*       ListObject,         // [in,out]
        double              NumberF64           // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* numberObject = NULL;

    if( NULL != ListObject )
    {
        jlStatus = JlCreateNumberF64Object( NumberF64, &numberObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToListObject( ListObject, numberObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &numberObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddNumberF64ToDictionaryObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddNumberF64ToDictionaryObject
    (
        JlDataObject*       DictionaryObject,   // [in,out]
        char const*         KeyName,            // [in]
        double              NumberF64           // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* numberObject = NULL;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName )
    {
        jlStatus = JlCreateNumberF64Object( NumberF64, &numberObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToDictionaryObject( DictionaryObject, KeyName, numberObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &numberObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlCreateBoolObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlCreateBoolObject
    (
        bool                BoolValue,          // [in]
        JlDataObject**      pBoolObject         // [out]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* boolObject = NULL;

    if( NULL != pBoolObject )
    {
        *pBoolObject = NULL;

        jlStatus = JlCreateObject( JL_DATA_TYPE_BOOL, &boolObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlSetObjectBool( boolObject, BoolValue );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                *pBoolObject = boolObject;
            }
            else
            {
                JlFreeObjectTree( &boolObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddBoolToListObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddBoolToListObject
    (
        JlDataObject*       ListObject,         // [in,out]
        bool                BoolValue           // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* boolObject = NULL;

    if( NULL != ListObject )
    {
        jlStatus = JlCreateBoolObject( BoolValue, &boolObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToListObject( ListObject, boolObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &boolObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddBoolToDictionaryObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddBoolToDictionaryObject
    (
        JlDataObject*       DictionaryObject,   // [in,out]
        char const*         KeyName,            // [in]
        bool                BoolValue           // [in]
    )
{
    JL_STATUS jlStatus;
    JlDataObject* boolObject = NULL;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName )
    {
        jlStatus = JlCreateBoolObject( BoolValue, &boolObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlAttachObjectToDictionaryObject( DictionaryObject, KeyName, boolObject );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                (void) JlFreeObjectTree( &boolObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}
