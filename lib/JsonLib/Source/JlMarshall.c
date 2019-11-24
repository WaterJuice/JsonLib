////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  The module contains functions for marshalling data from C structures into a JlDataObject tree. The C structures
//  are described by arrays of JlMarshallElement which map each element in a structure to a JSON data type.
//  The Marshaller code will take all the data from the structures it has definitions for and add them to a
//  JlDataObject tree. This tree can then be used to produce the JSON output.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JsonLib.h"
#include "JlBuffer.h"
#include "JlMemory.h"
#include "JlStatus.h"
#include "JlBase64.h"
#include <stdint.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  FORWARD DECLARTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static
JL_STATUS
    MarshallDictionary
    (
        void const*                 Structure,
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        JlDataObject*               DictionaryObject
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  VerifyFixedStringIsTerminated
//
//  Verifies that the string within a fixed length field has a null terminator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    VerifyFixedStringIsTerminated
    (
        char const*     String,
        size_t          StringSize
    )
{
    JL_STATUS jlStatus = JL_STATUS_STRING_NOT_TERMINATED;

    for( size_t i=0; i<StringSize; i++ )
    {
        if( 0 == String[i] )
        {
            jlStatus = JL_STATUS_SUCCESS;
            break;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MarshallNumber
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    MarshallNumber
    (
        void const*                 Input,
        JlMarshallElement const*    Description,
        JlDataObject**              pNewObject
    )
{
    JL_STATUS jlStatus;

    if( JL_NUM_TYPE_UNSIGNED == Description->NumberType )
    {
        uint64_t number = 0;

        // Get the unsigned number into "number", we allow for different number sizes
        jlStatus = JL_STATUS_SUCCESS;
        switch( Description->FieldSize )
        {
        case sizeof(uint64_t):
        {
            uint64_t const* InputU64 = Input;
            number = *InputU64;
            break;
        }
        case sizeof(uint32_t):
        {
            uint32_t const* InputU32 = Input;
            number = *InputU32;
            break;
        }
        case sizeof(uint16_t):
        {
            uint16_t const* InputU16 = Input;
            number = *InputU16;
            break;
        }
        case sizeof(uint8_t):
        {
            uint8_t const* InputU8 = Input;
            number = *InputU8;
            break;
        }
        default:
            jlStatus = JL_STATUS_INVALID_SPECIFICATION;
        }

        if( JL_STATUS_SUCCESS == jlStatus )
        {
            // Create the new number object
            jlStatus = JlCreateObject( JL_DATA_TYPE_NUMBER, pNewObject );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                if( Description->IsHex )
                {
                    jlStatus = JlSetObjectNumberHex( *pNewObject, number );
                }
                else
                {
                    jlStatus = JlSetObjectNumberU64( *pNewObject, number );
                }
            }
        }
    }
    else if( JL_NUM_TYPE_SIGNED == Description->NumberType )
    {
        int64_t number = 0;

        // Get the signed number into "number", we allow for different number sizes
        jlStatus = JL_STATUS_SUCCESS;
        switch( Description->FieldSize )
        {
        case sizeof(int64_t):
        {
            int64_t const* InputS64 = Input;
            number = *InputS64;
            break;
        }
        case sizeof(int32_t):
        {
            int32_t const* InputS32 = Input;
            number = *InputS32;
            break;
        }
        case sizeof(int16_t):
        {
            int16_t const* InputS16 = Input;
            number = *InputS16;
            break;
        }
        case sizeof(int8_t):
        {
            int8_t const* InputS8 = Input;
            number = *InputS8;
            break;
        }
        default:
            jlStatus = JL_STATUS_INVALID_SPECIFICATION;
        }

        if( JL_STATUS_SUCCESS == jlStatus )
        {
            // Create the new number object
            jlStatus = JlCreateObject( JL_DATA_TYPE_NUMBER, pNewObject );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                jlStatus = JlSetObjectNumberS64( *pNewObject, number );
            }
        }
    }
    else if( JL_NUM_TYPE_FLOAT == Description->NumberType )
    {
        double number = 0.0;

        // Get the float number into "number", we allow for float and double
        jlStatus = JL_STATUS_SUCCESS;
        switch( Description->FieldSize )
        {
        case sizeof(double):
        {
            double const* InputF64 = Input;
            number = *InputF64;
            break;
        }
        case sizeof(float):
        {
            float const* InputF32 = Input;
            number = *InputF32;
            break;
        }
        default:
            jlStatus = JL_STATUS_INVALID_SPECIFICATION;
        }

        if( JL_STATUS_SUCCESS == jlStatus )
        {
            // Create the new number object
            jlStatus = JlCreateObject( JL_DATA_TYPE_NUMBER, pNewObject );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                jlStatus = JlSetObjectNumberF64( *pNewObject, number );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_TYPE;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MarshallBool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    MarshallBool
    (
        void const*                 Input,
        JlMarshallElement const*    Description,
        JlDataObject**              pNewObject
    )
{
    JL_STATUS jlStatus;

    bool boolValue = false;

    // Get the bool value into "boolValue", we allow for different bool field sizes
    jlStatus = JL_STATUS_SUCCESS;
    switch( Description->FieldSize )
    {
    case sizeof(uint64_t):
    {
        uint64_t const* InputU64 = Input;
        boolValue = *InputU64 ? true : false;
        break;
    }
    case sizeof(uint32_t):
    {
        uint32_t const* InputU32 = Input;
        boolValue = *InputU32 ? true : false;
        break;
    }
    case sizeof(uint16_t):
    {
        uint16_t const* InputU16 = Input;
        boolValue = *InputU16 ? true : false;
        break;
    }
    case sizeof(uint8_t):
    {
        uint8_t const* InputU8 = Input;
        boolValue = *InputU8 ? true : false;
        break;
    }
    default:
        jlStatus = JL_STATUS_INVALID_SPECIFICATION;
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        // Create the new bool object
        jlStatus = JlCreateObject( JL_DATA_TYPE_BOOL, pNewObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlSetObjectBool( *pNewObject, boolValue );
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MarshallString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    MarshallString
    (
        void const*                 Input,
        JlMarshallElement const*    Description,
        JlDataObject**              pNewObject
    )
{
    JL_STATUS jlStatus;
    char const* string = NULL;

    // Get the string
    if( 0 == Description->FieldSize )
    {
        // Allocated string
        string = *(char const**)Input;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else
    {
        // Fixed size string in structure
        string = Input;

        // Verify string is null terminated
        jlStatus = VerifyFixedStringIsTerminated( string, Description->FieldSize );
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        // Create the new string object
        jlStatus = JlCreateObject( JL_DATA_TYPE_STRING, pNewObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            if( NULL != string )
            {
                jlStatus = JlSetObjectString( *pNewObject, string );
            }
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MarshalBinaryData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    MarshalBinaryData
    (
        void const*                 Input,
        size_t                      InputSize,      // Used if Description->FieldSize is 0 and Input is an allocated buffer
        JlMarshallElement const*    Description,
        JlDataObject**              pNewObject
    )
{
    JL_STATUS jlStatus;
    uint8_t const* data = NULL;
    size_t dataSize = 0;

    // Get the data
    if( 0 == Description->FieldSize )
    {
        // Allocated string. The string
        data = *(uint8_t const**)Input;
        dataSize = InputSize;
        jlStatus = JL_STATUS_SUCCESS;
    }
    else
    {
        // Fixed size string in structure
        data = Input;
        dataSize = Description->FieldSize;
        jlStatus = JL_STATUS_SUCCESS;
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        if( dataSize > 0 )
        {
            char* base64String = NULL;
            // Convert the data to a base64 string
            jlStatus = JlBase64Encode( data, dataSize, &base64String );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                // Create the new string object
                jlStatus = JlCreateObject( JL_DATA_TYPE_STRING, pNewObject );
                if( JL_STATUS_SUCCESS == jlStatus )
                {
                    jlStatus = JlSetObjectString( *pNewObject, base64String );
                }

                JlFree( base64String );
            }
        }
        else
        {
            // Zero data. So create it as a null object. (Make it as string and don't add any data)
            jlStatus = JlCreateObject( JL_DATA_TYPE_STRING, pNewObject );
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MarshallObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    MarshallObject
    (
        void const*                 Input,
        JlMarshallElement const*    Description,
        JlDataObject**              pNewObject
    )
{
    JL_STATUS jlStatus;

    *pNewObject = NULL;

    if( JL_DATA_TYPE_STRING == Description->Type )
    {
        // Regular allocated string.
        // Input is a pointer to a string.
        jlStatus = MarshallString( Input, Description, pNewObject );
    }
    else if( JL_DATA_TYPE_NUMBER == Description->Type )
    {
        jlStatus = MarshallNumber( Input, Description, pNewObject );
    }
    else if( JL_DATA_TYPE_BOOL == Description->Type )
    {
        jlStatus = MarshallBool( Input, Description, pNewObject );
    }
    else if (JL_DATA_TYPE_DICTIONARY == Description->Type )
    {
        // This is only called when we are processing dictionary in a list, otherwise dictionaries within
        // dictionaries are processed directly in MarshallDictionary.
        // Dictionaries within a list are always created new, they do not preserve an existing dictionary in
        // the list.
        jlStatus = JlCreateObject( JL_DATA_TYPE_DICTIONARY, pNewObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = MarshallDictionary( Input, Description->ChildStructDescription, Description->ChildStructDescriptionCount, *pNewObject );
        }
    }
    else
    {
        // Unknown data type
        jlStatus = JL_STATUS_INVALID_TYPE;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  RemoveExistingObjectFromDictionary
//
//  Removes the named object from the dictionary (and frees it) if it exists. If it does not exist then this does
//  nothing. No errors are reported.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
void
    RemoveExistingObjectFromDictionary
    (
        JlDataObject*               DictionaryObject,
        char const*                 KeyName
    )
{
    JL_STATUS jlStatus;
    JlDataObject* object = NULL;

    jlStatus = JlGetObjectFromDictionaryByKey( DictionaryObject, KeyName, &object );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        jlStatus = JlDetachObjectFromDictionaryObject( DictionaryObject, KeyName );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlFreeObjectTree( &object );
        }
        else
        {
            jlStatus = JL_STATUS_INTERNAL_ERROR;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MarshallList
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    MarshallList
    (
        void const*                 Input,
        size_t                      ArrayCount,
        JlMarshallElement const*    Description,
        JlDataObject**              pNewListObject
    )
{
    JL_STATUS jlStatus;
    void const* arrayStart = NULL;

    if( 0 == Description->ArrayFieldSize )
    {
        // Array is dynamically allocated, so Input is a pointer to the array
        arrayStart = *((void**)Input);
    }
    else
    {
        // Array is a fixed size in struct. Input points to the start of the array in struct
        arrayStart = Input;
    }

    jlStatus = JlCreateObject( JL_DATA_TYPE_LIST, pNewListObject );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        for( size_t i=0; i<ArrayCount && JL_STATUS_SUCCESS==jlStatus; i++ )
        {
            JlDataObject* newObject = NULL;
            void const* arrayItemPtr = ((uint8_t*)arrayStart) + (i * Description->ArrayItemSize);

            // Now marshall the data in the array item
            jlStatus = MarshallObject( arrayItemPtr, Description, &newObject );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                jlStatus = JlAttachObjectToListObject( *pNewListObject, newObject );
            }
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  MarshallDictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    MarshallDictionary
    (
        void const*                 Structure,
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        JlDataObject*               DictionaryObject
    )
{
    JL_STATUS jlStatus = JL_STATUS_SUCCESS;

    for( uint32_t i=0; i<StructDescriptionCount && JL_STATUS_SUCCESS==jlStatus; i++ )
    {
        JlMarshallElement const* currentItem = &StructDescription[i];
        void const* elementInStruct = (void*)(((uint8_t*)Structure)+currentItem->FieldOffset);

        if( currentItem->IsArray )
        {
            void const* countFieldPtr = ((uint8_t*)Structure) + currentItem->CountFieldOffset;
            size_t countValue = JlMemoryReadCountValue( countFieldPtr, currentItem->CountFieldSize );

            // Remove the list from the DictionaryObject if it already exists, as we will replace it with our
            // new object for our new list
            RemoveExistingObjectFromDictionary( DictionaryObject, currentItem->Name );

            // Marshall new list object
            JlDataObject* newListObject = NULL;
            jlStatus = MarshallList( elementInStruct, countValue, currentItem, &newListObject );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                // Attach the object to the dictionary
                jlStatus = JlAttachObjectToDictionaryObject( DictionaryObject, currentItem->Name, newListObject );
            }
        }
        else if( JL_DATA_TYPE_DICTIONARY == currentItem->Type )
        {
            // Dictionary within this dictionary. We treat this a bit differently from other objects as we
            // will allow existing items in the dictionary to remain if they are already there and not
            // referenced by the Description.

            // Get or create a new dictionary.
            JlDataObject* childDictionary = NULL;
            jlStatus = JlGetObjectFromDictionaryByKey( DictionaryObject, currentItem->Name, &childDictionary );
            if( JL_STATUS_NOT_FOUND == jlStatus )
            {
                // Create a new dictionary and attach it.
                jlStatus = JlCreateObject( JL_DATA_TYPE_DICTIONARY, &childDictionary );
                if( JL_STATUS_SUCCESS == jlStatus )
                {
                    jlStatus = JlAttachObjectToDictionaryObject( DictionaryObject, currentItem->Name, childDictionary );
                }
            }

            if( JL_STATUS_SUCCESS == jlStatus )
            {
                // childDictionary is the new dictionary we are going to marshall into.
                jlStatus = MarshallDictionary( elementInStruct, currentItem->ChildStructDescription, currentItem->ChildStructDescriptionCount, childDictionary );
            }
        }
        else
        {
            // Remove the item from the DictionaryObject if it already exists, as we will replace it with our
            // new object
            RemoveExistingObjectFromDictionary( DictionaryObject, currentItem->Name );

            // Marshall new object.
            JlDataObject* newObject = NULL;
            if( currentItem->IsBase64 )
            {
                size_t sizeValue = 0;
                if( currentItem->CountFieldOffset )
                {
                    void const* sizeFieldPtr = ((uint8_t*)Structure) + currentItem->CountFieldOffset;
                    sizeValue = JlMemoryReadCountValue( sizeFieldPtr, currentItem->CountFieldSize );
                }
                jlStatus = MarshalBinaryData( elementInStruct, sizeValue, currentItem, &newObject );
            }
            else
            {
                jlStatus = MarshallObject( elementInStruct, currentItem, &newObject );
            }
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                // Attach the object to the dictionary
                jlStatus = JlAttachObjectToDictionaryObject( DictionaryObject, currentItem->Name, newObject );
            }
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    )
{
    JL_STATUS jlStatus;

    if(     NULL != DictionaryObject
        &&  NULL != StructDescription
        &&  0 != StructDescriptionCount
        &&  NULL != Structure )
    {
        if( JL_DATA_TYPE_DICTIONARY == JlGetObjectType( DictionaryObject ) )
        {
            jlStatus = MarshallDictionary( Structure, StructDescription, StructDescriptionCount, DictionaryObject );
        }
        else
        {
            // Root object must be a dictionary
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
//  JlMarshallFromStructToNewDataObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlMarshallFromStructToNewDataObject
    (
        void const*                 Structure,
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        JlDataObject**              pDictionaryObject
    )
{
    JL_STATUS jlStatus;

    if(     NULL != Structure
        &&  NULL != StructDescription
        &&  0 != StructDescriptionCount
        &&  NULL != pDictionaryObject )
    {
        JlDataObject* newDictionaryObject = NULL;
        *pDictionaryObject = NULL;

        jlStatus = JlCreateObject( JL_DATA_TYPE_DICTIONARY, &newDictionaryObject );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            jlStatus = JlMarshallFromStructToDataObject( Structure, StructDescription, StructDescriptionCount, newDictionaryObject );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                *pDictionaryObject = newDictionaryObject;
            }
            else
            {
                JlFreeObjectTree( &newDictionaryObject );
            }
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}
