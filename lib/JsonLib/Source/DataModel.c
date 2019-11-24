////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//
//  This is free and unencumbered software released into the public domain - August 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataModel.h"
#include "JsonLib.h"
#include "LinkedLists.h"
#include "JlMemory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct JlNumber
{
    JL_NUM_TYPE     Type;
    union
    {
        // JL_NUM_TYPE_UNSIGNED
        uint64_t    u64;
        // JL_NUM_TYPE_SIGNED
        int64_t     s64;
        // JL_NUM_TYPE_FLOAT
        double      f64;
    };
};

struct JlListItem
{
    JlListItem*     Next;
    JlList*         ParentList;
    JlDataObject*   Object;
};

struct JlList
{
    uint32_t        Count;

    JlListItem*     ListHead;
    JlListItem*     ListTail;
};

struct JlDictionaryItem
{
    JlDictionaryItem*   Next;
    JlDictionary*       ParentDictionary;
    char*               KeyName;
    JlDataObject*       Object;
};

struct JlDictionary
{
    JlDictionaryItem*   DictionaryHead;
    JlDictionaryItem*   DictionaryTail;
};

struct JlDataObject
{
    JL_DATA_TYPE    Type;
    size_t          Tag;        // Used for tracking original Json character position

    union
    {
        // JL_DATA_TYPE_STRING
        char*           String;

        // JL_DATA_TYPE_NUMBER
        JlNumber        Number;

        // JL_DATA_TYPE_BOOL
        bool            Boolean;

        // JL_DATA_TYPE_LIST
        JlList          List;

        // JL_DATA_TYPE_DICTIONARY
        JlDictionary    Dictionary;
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IsDataTypeValid
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
bool
    IsDataTypeValid
    (
        JL_DATA_TYPE    Type
    )
{
    if(     Type > JL_DATA_TYPE_NONE
        &&  Type <= JL_DATA_TYPE_MAX_VALUE )
    {
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  FindDictionaryItem
//
//  Returns the object in the dictionary specified by key. Returns NULL if key does not exist in the dictionary.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JlDataObject*
    FindDictionaryItem
    (
        JlDictionary*   Dictionary,
        char const*     KeyName
    )
{
    JlDataObject* retObject = NULL;

    for( JlDictionaryItem* item=Dictionary->DictionaryHead; item!=NULL; item=item->Next )
    {
        if( 0 == strcmp( KeyName, item->KeyName ) )
        {
            retObject = item->Object;
            break;
        }
    }

    return retObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  FindAndDetachDictionaryItem
//
//  Returns the object in the dictionary specified by key. Returns NULL if key does not exist in the dictionary.
//  If it is found the object is detached from the dictionary.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JlDataObject*
    FindAndDetachDictionaryItem
    (
        JlDictionary*   Dictionary,
        char const*     KeyName
    )
{
    JlDataObject* retObject = NULL;
    JlDictionaryItem* prevItem = NULL;

    for( JlDictionaryItem* item=Dictionary->DictionaryHead; item!=NULL; item=item->Next )
    {
        if( 0 == strcmp( KeyName, item->KeyName ) )
        {
            // Grab the object which we will return
            retObject = item->Object;

            // Now remove from linked list
            if( NULL != prevItem )
            {
                prevItem->Next = item->Next;
            }
            else
            {
                // First item in list
                Dictionary->DictionaryHead = item->Next;
            }

            if( item == Dictionary->DictionaryTail )
            {
                // Was last item in the list
                Dictionary->DictionaryTail = prevItem;
            }

            // Now free the DictionaryItem Key string
            JlFree( item->KeyName );

            // Now free the DictionaryItem (not the object)
            JlFree( item );
            item = NULL;
            break;
        }

        prevItem = item;
    }

    return retObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlCreateObject
//
//  Creates a new JlDataObject of the specified type.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlCreateObject
    (
        JL_DATA_TYPE    Type,
        JlDataObject**  pNewObject
    )
{
    return JlCreateObjectWithTag( Type, 0, pNewObject );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlCreateObjectWithTag
//
//  This creates a new JlDataObject of the specified type and also adds an optional tag value. This is intended
//  to be used to indicate the original character position within the json in order to be able to return a helpful
//  error hint when unmarshalling the data. If no tag is required then use JlCreateObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlCreateObjectWithTag
    (
        JL_DATA_TYPE    Type,
        size_t          Tag,
        JlDataObject**  pNewObject
    )
{
    JL_STATUS jlStatus;
    JlDataObject* object = NULL;

    if( NULL != pNewObject )
    {
        *pNewObject = NULL;

        if( IsDataTypeValid( Type ) )
        {
            object = JlNew( JlDataObject );
            if( NULL != object )
            {
                object->Type = Type;
                object->Tag = Tag;
                *pNewObject = object;
                jlStatus = JL_STATUS_SUCCESS;
            }
            else
            {
                jlStatus = JL_STATUS_OUT_OF_MEMORY;
            }
        }
        else
        {
            jlStatus = JL_STATUS_INVALID_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlSetObjectString
//
//  Sets the value of a string object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlSetObjectString
    (
        JlDataObject*   StringObject,
        char const*     String
    )
{
    JL_STATUS jlStatus;

    if( NULL != StringObject )
    {
        if( JL_DATA_TYPE_STRING == StringObject->Type )
        {
            if( NULL != StringObject->String )
            {
                // Remove existing string
                JlFree( StringObject->String );
                StringObject->String = NULL;
            }

            size_t length = 0;
            if( NULL != String )
            {
                length = strlen( String );
            }

            if( length > 0 )
            {
                // Only create a buffer if there is data. No need to just store a Zero terminator.
                StringObject->String = JlStrDup( String );
                if( NULL != StringObject->String )
                {
                    jlStatus = JL_STATUS_SUCCESS;
                }
                else
                {
                    jlStatus = JL_STATUS_OUT_OF_MEMORY;
                }
            }
            else
            {
                // Nothing more to do.
                jlStatus = JL_STATUS_SUCCESS;
            }
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlSetObjectNumberU64
//
//  Sets the value of a number object as a uint64_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlSetObjectNumberU64
    (
        JlDataObject*   NumberObject,
        uint64_t        NumberU64
    )
{
    JL_STATUS jlStatus;

    if( NULL != NumberObject )
    {
        if( JL_DATA_TYPE_NUMBER == NumberObject->Type )
        {
            NumberObject->Number.Type = JL_NUM_TYPE_UNSIGNED;
            NumberObject->Number.u64 = NumberU64;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlSetObjectNumberS64
//
//  Sets the value of a number object as an int64_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlSetObjectNumberS64
    (
        JlDataObject*   NumberObject,
        int64_t         NumberS64
    )
{
    JL_STATUS jlStatus;

    if( NULL != NumberObject )
    {
        if( JL_DATA_TYPE_NUMBER == NumberObject->Type )
        {
            NumberObject->Number.Type = JL_NUM_TYPE_SIGNED;
            NumberObject->Number.s64 = NumberS64;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlSetObjectNumberF64
//
//  Sets the value of a number object as a double
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlSetObjectNumberF64
    (
        JlDataObject*   NumberObject,
        double          NumberF64
    )
{
    JL_STATUS jlStatus;

    if( NULL != NumberObject )
    {
        if( JL_DATA_TYPE_NUMBER == NumberObject->Type )
        {
            NumberObject->Number.Type = JL_NUM_TYPE_FLOAT;
            NumberObject->Number.f64 = NumberF64;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlSetObjectNumberString
//
//  Sets the value of a number object from a string
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlSetObjectNumberString
    (
        JlDataObject*   NumberObject,
        char const*     NumberString
    )
{
    JL_STATUS jlStatus;
    int converted;
    char newString [32] = "";

    if(     NULL != NumberObject
        &&  NULL != NumberString )
    {
        jlStatus = JL_STATUS_INVALID_DATA;

        if( JL_STATUS_SUCCESS != jlStatus )
        {
            // Try to convert the number as u64 first
            uint64_t u64 = 0;
            converted = sscanf( NumberString, "%llu", &u64 );
            if( 1 == converted )
            {
                sprintf( newString, "%llu", u64 );
                if( strcmp( NumberString, newString ) == 0 )
                {
                    jlStatus = JlSetObjectNumberU64( NumberObject, u64 );
                }
            }
        }

        if( JL_STATUS_SUCCESS != jlStatus )
        {
            // Try to convert the number as a signed 64 bit number
            int64_t s64 = 0;
            converted = sscanf( NumberString, "%lld", &s64 );
            if( 1 == converted )
            {
                sprintf( newString, "%lld", s64 );
                if( strcmp( NumberString, newString ) == 0 )
                {
                    jlStatus = JlSetObjectNumberS64( NumberObject, s64 );
                }
            }

        }

        if( JL_STATUS_SUCCESS != jlStatus )
        {
            // Try to convert the number as a float
            double f64 = 0.0;
            converted = sscanf( NumberString, "%lg", &f64 );
            if( 1 == converted )
            {
                double a_inf = DBL_MAX * 2.0;
                double a_ninf = DBL_MAX * -2.0;

                if(     0.0 != f64
                    &&  !_isnan( f64 )
                    &&  a_inf != f64
                    &&  a_ninf != f64 )
                {
                    jlStatus = JlSetObjectNumberF64( NumberObject, f64 );
                }
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
//  JlSetObjectBool
//
//  Sets the value of a boolean object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlSetObjectBool
    (
        JlDataObject*   BoolObject,
        bool            BoolValue
    )
{
    JL_STATUS jlStatus;

    if( NULL != BoolObject )
    {
        if( JL_DATA_TYPE_BOOL == BoolObject->Type )
        {
            BoolObject->Boolean = BoolValue ? true : false;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAttachObjectToListObject
//
//  Adds an object to the end of an existing list object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAttachObjectToListObject
    (
        JlDataObject*   ListObject,
        JlDataObject*   NewObject
    )
{
    JL_STATUS jlStatus;

    if(     NULL != ListObject
        &&  NULL != NewObject )
    {
        if( JL_DATA_TYPE_LIST == ListObject->Type )
        {
            // Create list item container for object
            JlListItem* listItem = JlNew( JlListItem );
            if( NULL != listItem )
            {
                listItem->Next = NULL;
                listItem->Object = NewObject;
                listItem->ParentList = &ListObject->List;

                LinkedListAddToEnd( ListObject->List.ListHead, ListObject->List.ListTail, listItem );
                ListObject->List.Count += 1;

                jlStatus = JL_STATUS_SUCCESS;
            }
            else
            {
                jlStatus = JL_STATUS_OUT_OF_MEMORY;
            }
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAttachObjectToDictionaryObject
//
//  Adds an object to an existing dictionary object with specified key. The key must not already exist in the
//  dictionary.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAttachObjectToDictionaryObject
    (
        JlDataObject*   DictionaryObject,
        char const*     KeyName,
        JlDataObject*   NewObject
    )
{
    JL_STATUS jlStatus;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName
        &&  0 != KeyName[0]
        &&  NULL != NewObject )
    {
        if( JL_DATA_TYPE_DICTIONARY == DictionaryObject->Type )
        {
            // Check key does not already exist
            if( NULL == FindDictionaryItem( &DictionaryObject->Dictionary, KeyName ) )
            {
                // Create dictionary item container for object
                JlDictionaryItem* dictionaryItem = JlNew( JlDictionaryItem );
                if( NULL != dictionaryItem )
                {
                    dictionaryItem->Next = NULL;
                    dictionaryItem->Object = NewObject;
                    dictionaryItem->ParentDictionary = &DictionaryObject->Dictionary;
                    dictionaryItem->KeyName = JlStrDup( KeyName );
                    if( NULL != dictionaryItem->KeyName )
                    {
                        LinkedListAddToEnd( DictionaryObject->Dictionary.DictionaryHead, DictionaryObject->Dictionary.DictionaryTail, dictionaryItem );
                        jlStatus = JL_STATUS_SUCCESS;
                    }
                    else
                    {
                        jlStatus = JL_STATUS_OUT_OF_MEMORY;
                        JlFree( dictionaryItem );
                        dictionaryItem = NULL;
                    }
                }
                else
                {
                    jlStatus = JL_STATUS_OUT_OF_MEMORY;
                }
            }
            else
            {
                jlStatus = JL_STATUS_DICTIONARY_ITEM_REPEATED;
            }
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlDetachObjectFromDictionaryObject
//
//  Removes an object from a dictionary object. This does NOT deallocate the object, it is left as a free standing
//  object tree of its own. Use JlFreeObjectTree on the detached object to free it.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlDetachObjectFromDictionaryObject
    (
        JlDataObject*   DictionaryObject,
        char const*     KeyName
    )
{
    JL_STATUS jlStatus;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName
        &&  0 != KeyName[0] )
    {
        if( JL_DATA_TYPE_DICTIONARY == DictionaryObject->Type )
        {
            // Find and detach object
            JlDataObject* object = FindAndDetachDictionaryItem( &DictionaryObject->Dictionary, KeyName );
            if( NULL != object )
            {
                // object is now detached from the dictionary.
                jlStatus = JL_STATUS_SUCCESS;
            }
            else
            {
                jlStatus = JL_STATUS_NOT_FOUND;
            }
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlAddStringToList
//
//  Adds a new string to a list object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlAddStringToList
    (
        JlDataObject const*     ListObject,
        char const*             String
    )
{
    JL_STATUS jlStatus;
    JlDataObject* object = NULL;

    if(     NULL != ListObject
        &&  NULL != String
    JL_ASSERT_SUCCESS( JlCreateObject( JL_DATA_TYPE_STRING, &object ) );
    JL_ASSERT_SUCCESS( JlSetObjectString( object, String ) );
    JL_ASSERT_SUCCESS( JlAttachObjectToListObject( ListObject, object ) );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectType
//
//  Returns the object type of an object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_DATA_TYPE
    JlGetObjectType
    (
        JlDataObject const*   Object
    )
{
    JL_DATA_TYPE retType = JL_DATA_TYPE_NONE;

    if( NULL != Object )
    {
        retType = Object->Type;
    }

    return retType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectTag
//
//  Returns the object tag if set with JlCreateObjectWithTag
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
size_t
    JlGetObjectTag
    (
        JlDataObject const*   Object
    )
{
    size_t tag = 0;

    if( NULL != Object )
    {
        tag = Object->Tag;
    }

    return tag;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectString
//
//  Gets the string from a String object. This returns the pointer of the internal string and must not be modified.
//  Returns JL_STATUS_WRONG_TYPE if the object is not a string object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetObjectString
    (
        JlDataObject const* StringObject,
        char const**        pString
    )
{
    JL_STATUS jlStatus;

    if(     NULL != StringObject
        &&  NULL != pString )
    {
        if( JL_DATA_TYPE_STRING == StringObject->Type )
        {
            *pString = StringObject->String;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            *pString = NULL;
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectNumberU64
//
//  Gets the number object as uint64_t. Returns JL_STATUS_WRONG_TYPE if the object is not a number or can not be
//  represented as a uint64_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetObjectNumberU64
    (
        JlDataObject const* NumberObject,
        uint64_t*           pNumber64
    )
{
    JL_STATUS jlStatus;

    if(     NULL != NumberObject
        &&  NULL != pNumber64 )
    {
        if(     JL_DATA_TYPE_NUMBER == NumberObject->Type
            &&  JL_NUM_TYPE_UNSIGNED == NumberObject->Number.Type )
        {
            // Positive integers will always be stored as unsigned, even if they were added as signed number
            *pNumber64 = NumberObject->Number.u64;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            *pNumber64 = 0;
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectNumberS64
//
//  Gets the number object as int64_t. Returns JL_STATUS_WRONG_TYPE if the object is not a number or can not be
//  represented as a int64_t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetObjectNumberS64
    (
        JlDataObject const* NumberObject,
        int64_t*            pNumberS64
    )
{
    JL_STATUS jlStatus;

    if(     NULL != NumberObject
        &&  NULL != pNumberS64 )
    {
        if(     JL_DATA_TYPE_NUMBER == NumberObject->Type
            &&  JL_NUM_TYPE_SIGNED == NumberObject->Number.Type )
        {
            // Number stored as a signed integer. Easy.
            *pNumberS64 = NumberObject->Number.s64;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else if(     JL_DATA_TYPE_NUMBER == NumberObject->Type
            &&  JL_NUM_TYPE_UNSIGNED == NumberObject->Number.Type
            &&  NumberObject->Number.u64 <= INT64_MAX )
        {
            // Number is stored as unsigned number but it is smaller than INT64_MAX so it can be represented
            // in a signed 64 bit number
            *pNumberS64 = NumberObject->Number.u64;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            *pNumberS64 = 0;
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectNumberF64
//
//  Gets the number object as double. Returns JL_STATUS_WRONG_TYPE if the object is not a number or can not be
//  represented as a double
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetObjectNumberF64
    (
        JlDataObject const* NumberObject,
        double*             pNumberF64
    )
{
    JL_STATUS jlStatus;

    if(     NULL != NumberObject
        &&  NULL != pNumberF64 )
    {
        if(     JL_DATA_TYPE_NUMBER == NumberObject->Type
            &&  JL_NUM_TYPE_FLOAT == NumberObject->Number.Type )
        {
            // Number stored as a double. Easy.
            *pNumberF64 = NumberObject->Number.f64;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else if(     JL_DATA_TYPE_NUMBER == NumberObject->Type
            &&  JL_NUM_TYPE_UNSIGNED == NumberObject->Number.Type )
        {
            // Convert from uint64_t to double. Precision may be lost.
            *pNumberF64 = (double)(NumberObject->Number.u64);
            jlStatus = JL_STATUS_SUCCESS;
        }
        else if(     JL_DATA_TYPE_NUMBER == NumberObject->Type
            &&  JL_NUM_TYPE_SIGNED == NumberObject->Number.Type )
        {
            // Convert from int64_t to double. Precision may be lost.
            *pNumberF64 = (double)(NumberObject->Number.s64);
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            *pNumberF64 = 0.0;
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectBool
//
//  Gets the boolean object. Returns JL_STATUS_WRONG_TYPE if object is not a bool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetObjectBool
    (
        JlDataObject const* BoolObject,
        bool*               pBoolValue
    )
{
    JL_STATUS jlStatus;

    if(     NULL != BoolObject
        &&  NULL != pBoolValue )
    {
        if( JL_DATA_TYPE_BOOL == BoolObject->Type )
        {
            *pBoolValue = BoolObject->Boolean;
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            *pBoolValue = false;
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectNumberType
//
//  Returns the JL_NUM_TYPE of the object. If the object is not a number or not valid then this will
//  return JL_NUM_TYPE_NONE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_NUM_TYPE
    JlGetObjectNumberType
    (
        JlDataObject const*     NumberObject
    )
{
    JL_NUM_TYPE returnType = JL_NUM_TYPE_NONE;

    if(     NULL != NumberObject
        &&  JL_DATA_TYPE_NUMBER == NumberObject->Type )
    {
        returnType = NumberObject->Number.Type;
    }

    return returnType;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectListNextItem
//
//  Gets the next object in the list object.
//  To get the first item in the list *pEnumerator must be NULL. To get the next item in the list pass the value
//  of *pEnumerator that was returned from previous call. The value of *pEnumerator is opaque and must not be used
//  for any other purpose.
//  The function returns the next data object in the list until there are no more, then it will return
//  JL_STATUS_END_OF_DATA.
//  If ListObject is not a list object or pEnumerator is not valid then this function will return JL_STATUS_WRONG_TYPE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetObjectListNextItem
    (
        JlDataObject const*     ListObject,
        JlDataObject const**    pNextObject,
        JlListItem const**      pEnumerator
    )
{
    JL_STATUS jlStatus;

    if(     NULL != ListObject
        &&  NULL != pNextObject
        &&  NULL != pEnumerator )
    {
        if(     JL_DATA_TYPE_LIST == ListObject->Type
            &&  ( NULL == *pEnumerator  || (*pEnumerator)->ParentList == &ListObject->List ) )
        {
            if( NULL == *pEnumerator )
            {
                *pEnumerator = ListObject->List.ListHead;
                if( NULL != ListObject->List.ListHead )
                {
                    *pNextObject = ListObject->List.ListHead->Object;
                    jlStatus = JL_STATUS_SUCCESS;
                }
                else
                {
                    *pNextObject = NULL;
                    jlStatus = JL_STATUS_END_OF_DATA;
                }
            }
            else
            {
                *pEnumerator = (*pEnumerator)->Next;
                if( NULL != *pEnumerator )
                {
                    *pNextObject = (*pEnumerator)->Object;
                    jlStatus = JL_STATUS_SUCCESS;
                }
                else
                {
                    *pNextObject = NULL;
                    jlStatus = JL_STATUS_END_OF_DATA;
                }
            }
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectDictionaryNextItem
//
//  Gets the next object in the dictionary object.
//  To get the first item in the dictionary *pEnumerator must be NULL. To get the next item in the list pass the value
//  of *pEnumerator that was returned from previous call. The value of *pEnumerator is opaque and must not be used
//  for any other purpose.
//  *pKeyName will be set to the Key of the dictionary associated with the object. This is a pointer to a string
//  within the DictionaryObject and must not be modified.
//  The function returns the next data object in the list until there are no more, then it will return
//  JL_STATUS_END_OF_DATA.
//  If DictionaryObject is not a dictionary object or pEnumerator is not valid then this function will return
//  JL_STATUS_WRONG_TYPE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetObjectDictionaryNextItem
    (
        JlDataObject const*       DictionaryObject,
        JlDataObject const**      pNextObject,
        char const**              pKeyName,
        JlDictionaryItem const**  pEnumerator
    )
{
    JL_STATUS jlStatus;

    if(     NULL != DictionaryObject
        &&  NULL != pNextObject
        &&  NULL != pKeyName
        &&  NULL != pEnumerator )
    {
        if(     JL_DATA_TYPE_DICTIONARY == DictionaryObject->Type
            &&  ( NULL == *pEnumerator  || (*pEnumerator)->ParentDictionary == &DictionaryObject->Dictionary ) )
        {
            if( NULL == *pEnumerator )
            {
                // Get first item
                *pEnumerator = DictionaryObject->Dictionary.DictionaryHead;
                if( NULL != DictionaryObject->Dictionary.DictionaryHead )
                {
                    *pNextObject = DictionaryObject->Dictionary.DictionaryHead->Object;
                    *pKeyName    = DictionaryObject->Dictionary.DictionaryHead->KeyName;
                    jlStatus = JL_STATUS_SUCCESS;
                }
                else
                {
                    // Empty dictionary
                    *pNextObject = NULL;
                    *pKeyName = NULL;
                    jlStatus = JL_STATUS_END_OF_DATA;
                }
            }
            else
            {
                // Move enumerator to next item.
                *pEnumerator = (*pEnumerator)->Next;
                if( NULL != *pEnumerator )
                {
                    *pNextObject = (*pEnumerator)->Object;
                    *pKeyName    = (*pEnumerator)->KeyName;
                    jlStatus = JL_STATUS_SUCCESS;
                }
                else
                {
                    *pNextObject = NULL;
                    *pKeyName    = NULL;
                    jlStatus = JL_STATUS_END_OF_DATA;
                }
            }
        }
        else
        {
            jlStatus = JL_STATUS_WRONG_TYPE;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetObjectDictionaryByKey
//
//  Gets an object from a dictionary based on key name.
//  Returns JL_STATUS_NOT_FOUND if the key does not exist in the dictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetObjectFromDictionaryByKey
    (
        JlDataObject const*         DictionaryObject,
        char const*                 KeyName,
        JlDataObject**              pObject
    )
{
    JL_STATUS jlStatus = JL_STATUS_SUCCESS;
    JlDictionaryItem* enumerator = NULL;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName
        &&  NULL != pObject )
    {
        *pObject = NULL;

        while( JL_STATUS_SUCCESS == jlStatus )
        {
            JlDataObject* object = NULL;
            char const* key = NULL;

            jlStatus = JlGetObjectDictionaryNextItem( DictionaryObject, &object, &key, &enumerator );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                if( strcmp( key, KeyName ) == 0 )
                {
                    // Found
                    *pObject = object;
                    break;
                }
            }
        }

        if( JL_STATUS_END_OF_DATA == jlStatus )
        {
            // This means we got to end of dictionary and did not find the key
            jlStatus = JL_STATUS_NOT_FOUND;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetStringFromDictionaryByKey
//
//  Gets an string from a dictionary based on key name.
//  Returns JL_STATUS_NOT_FOUND if the key does not exist in the dictionary.
//  Returns JL_STATUS_WRONG_TYPE if the key exists but is not a string type.
//  This returns a pointer into DictionaryObject and must not be modified
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetStringFromDictionaryByKey
    (
        JlDataObject const*         DictionaryObject,
        char const*                 KeyName,
        char const**                pString
    )
{
    JL_STATUS jlStatus;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName
        &&  NULL != pString )
    {
        *pString = NULL;

        JlDataObject* object = NULL;
        jlStatus = JlGetObjectFromDictionaryByKey( DictionaryObject, KeyName, &object );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            char const* stringPtr = NULL;
            jlStatus = JlGetObjectString( object, &stringPtr );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                *pString = stringPtr;
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
//  JlGetListCount
//
//  Gets the count of elements in a list object. Returns 0 if the object is not a list type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t
    JlGetListCount
    (
        JlDataObject const*         ListObject
    )
{
    uint32_t count = 0;

    if( NULL != ListObject )
    {
        if( JL_DATA_TYPE_LIST == ListObject->Type )
        {
            count = ListObject->List.Count;
        }
        // else: Not a list
    } // else: Invalid parameter

    return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlGetListFromDictionaryByKey
//
//  Gets a list object from a dictionary based on key name.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlGetListFromDictionaryByKey
    (
        JlDataObject const*         DictionaryObject,
        char const*                 KeyName,
        JlDataObject**              pListObject
    )
{
    JL_STATUS jlStatus;

    if(     NULL != DictionaryObject
        &&  NULL != KeyName
        &&  NULL != pListObject )
    {
        *pListObject = NULL;

        JlDataObject* object = NULL;
        jlStatus = JlGetObjectFromDictionaryByKey( DictionaryObject, KeyName, &object );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            if( JL_DATA_TYPE_LIST == object->Type )
            {
                *pListObject = object;
            }
            else
            {
                jlStatus = JL_STATUS_WRONG_TYPE;
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
//  JlFreeObjectTree
//
//  Frees an object and all items below it (if it is a dictionary or list)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlFreeObjectTree
    (
        JlDataObject**              pRootObject
    )
{
    JL_STATUS jlStatus;
    JL_DATA_TYPE objectType = JL_DATA_TYPE_NONE;

    if(     NULL != pRootObject
        &&  NULL != *pRootObject )
    {
        JlDataObject* object = *pRootObject;

        objectType = JlGetObjectType( object );
        switch( objectType )
        {
        case JL_DATA_TYPE_STRING:
            if( NULL != object->String )
            {
                JlFree( object->String );
                object->String = NULL;
            }
            jlStatus = JL_STATUS_SUCCESS;
            break;
        case JL_DATA_TYPE_NUMBER:
        case JL_DATA_TYPE_BOOL:
            jlStatus = JL_STATUS_SUCCESS;
            break;
        case JL_DATA_TYPE_DICTIONARY:
            JlDictionaryItem* prevDictionaryItem = NULL;
            jlStatus = JL_STATUS_SUCCESS;
            for( JlDictionaryItem* dictionaryItem=object->Dictionary.DictionaryHead; dictionaryItem!=NULL; dictionaryItem=dictionaryItem->Next )
            {
                if( NULL != prevDictionaryItem )
                {
                    JlFree( prevDictionaryItem );
                    prevDictionaryItem = NULL;
                }
                JlFree( dictionaryItem->KeyName );
                dictionaryItem->KeyName = NULL;
                jlStatus = JlFreeObjectTree( &dictionaryItem->Object );
                prevDictionaryItem = dictionaryItem;
                if( JL_STATUS_SUCCESS != jlStatus )
                {
                    break;
                }
            }
            if( NULL != prevDictionaryItem )
            {
                JlFree( prevDictionaryItem );
                prevDictionaryItem = NULL;
            }
            break;
        case JL_DATA_TYPE_LIST:
            JlListItem* prevListItem = NULL;
            jlStatus = JL_STATUS_SUCCESS;
            for( JlListItem* listItem=object->List.ListHead; listItem!=NULL; listItem=listItem->Next )
            {
                if( NULL != prevListItem )
                {
                    JlFree( prevListItem );
                    prevListItem = NULL;
                }
                jlStatus = JlFreeObjectTree( &listItem->Object );
                listItem->Object = NULL;
                prevListItem = listItem;
                if( JL_STATUS_SUCCESS != jlStatus )
                {
                    break;
                }
            }
            if( NULL != prevListItem )
            {
                JlFree( prevListItem );
                prevListItem = NULL;
            }
            break;
        default:
            jlStatus = JL_STATUS_CORRUPT_MEMORY;
            break;
        }

        // Now free the object container
        JlFree( object );
        object = NULL;
        *pRootObject = NULL;
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}
