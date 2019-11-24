////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This module will unmarshall data inside a JlDataObject tree and place it into C structures. The mapping is defined
//  by arrays of JlMarshallElement which map the JSON elements to C structure types. Note, not every JSON file can
//  be unmarshalled. For example only lists that contain the same data type can be unmarshalled. Also only JlDataObject
//  trees that have a dictionary as a root (JSON starts with a dictionary) can be unmarshalled.
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JsonLib.h"
#include "JlMemory.h"
#include "JlBase64.h"
#include <float.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  FORWARD DECLARTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static
JL_STATUS
    UnmarshallDictionary
    (
        JlDataObject const*         DictionaryObject,
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        bool                        IgnoreExistingValuesInStruct,
        void*                       Structure,
        size_t*                     pErrorAtPos
    );

static
JL_STATUS
    FreeUnmarshalledDictionary
    (
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        void*                       Structure
    );

static
JL_STATUS
    FreeUnmarshalledList
    (
        JlMarshallElement const*    Description,
        void*                       ListPtr,
        size_t                      ListCount
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CopyString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    CopyString
    (
        char*       DestinationStr,
        size_t      DestinationStrSize,
        char const* SourceStr
    )
{
    JL_STATUS jlStatus;

    if(     NULL != DestinationStr
        &&  0 != DestinationStrSize
        &&  NULL != SourceStr )
    {
        size_t sourceLength = strlen( SourceStr );
        if( sourceLength < DestinationStrSize )
        {
            // Have room to copy it
            strcpy( DestinationStr, SourceStr );
            jlStatus = JL_STATUS_SUCCESS;
        }
        else
        {
            jlStatus = JL_STATUS_BUFFER_TOO_SMALL;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshalString
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshalString
    (
        JlDataObject const*         StringObject,
        bool                        IgnoreExistingValuesInStruct,
        void*                       Output
    )
{
    JL_STATUS jlStatus;
    char const* stringPtr = NULL;
    char** OutputStrPtr = Output;

    if( !IgnoreExistingValuesInStruct && NULL != *OutputStrPtr )
    {
        // Free the existing string here. Note this requires that the string had been allocated by this library
        // on a previous unmarshall, and was not pointing to a static string or junk memory.
        JlFree( *OutputStrPtr );
        *OutputStrPtr = NULL;
    }

    jlStatus = JlGetObjectString( StringObject, &stringPtr );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        if( NULL != stringPtr )
        {
            char* newString = JlStrDup( stringPtr );
            if( NULL != newString )
            {
                *OutputStrPtr = newString;
            }
            else
            {
                jlStatus = JL_STATUS_OUT_OF_MEMORY;
            }
        }
        else
        {
            // String was a NULL string, so leave it NULL
            jlStatus = JL_STATUS_SUCCESS;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshalStringFixed
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshalStringFixed
    (
        JlDataObject const*         StringObject,
        void*                       Output,
        size_t                      FixedSize
    )
{
    JL_STATUS jlStatus;
    char const* stringPtr = NULL;
    char* OutputStr = Output;

    memset( OutputStr, 0, FixedSize );

    jlStatus = JlGetObjectString( StringObject, &stringPtr );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        if( NULL != stringPtr )
        {
            jlStatus = CopyString( OutputStr, FixedSize, stringPtr );
        }
        else
        {
            // Null string type. We have no pointer to set to NULL, so we
            // will just have to make the string blank.
            memset( Output, 0, FixedSize );
            jlStatus = JL_STATUS_SUCCESS;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshalUnsigned
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshalUnsigned
    (
        JlDataObject const*         NumberObject,
        void*                       Output,
        size_t                      FixedSize
    )
{
    JL_STATUS jlStatus;
    uint64_t number = 0;

    jlStatus = JlGetObjectNumberU64( NumberObject, &number );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        switch( FixedSize )
        {
        case sizeof(uint64_t):
        {
            uint64_t* OutputU64 = Output;
            *OutputU64 = number;
            break;
        }
        case sizeof(uint32_t):
        {
            uint32_t* OutputU32 = Output;
            if( number <= UINT32_MAX )  { *OutputU32 = (uint32_t) number;  }
            else                        { jlStatus = JL_STATUS_VALUE_OUT_OF_RANGE;  }
            break;
        }
        case sizeof(uint16_t):
        {
            uint16_t* OutputU16 = Output;
            if( number <= UINT16_MAX )  { *OutputU16 = (uint16_t) number;  }
            else                        { jlStatus = JL_STATUS_VALUE_OUT_OF_RANGE;  }
            break;
        }
        case sizeof(uint8_t):
        {
            uint8_t* OutputU8 = Output;
            if( number <= UINT8_MAX )   { *OutputU8 = (uint8_t) number;  }
            else                        { jlStatus = JL_STATUS_VALUE_OUT_OF_RANGE; }
            break;
        }
        default:
            jlStatus = JL_STATUS_INVALID_SPECIFICATION;
            break;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshalSigned
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshalSigned
    (
        JlDataObject const*         NumberObject,
        void*                       Output,
        size_t                      FixedSize
    )
{
    JL_STATUS jlStatus;
    int64_t number = 0;

    jlStatus = JlGetObjectNumberS64( NumberObject, &number );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        switch( FixedSize )
        {
        case sizeof(int64_t):
        {
            int64_t* OutputS64 = Output;
            *OutputS64 = number;
            break;
        }
        case sizeof(int32_t):
        {
            int32_t* OutputS32 = Output;
            if( number <= INT32_MAX && number >= INT32_MIN ) { *OutputS32 = (int32_t) number;  }
            else                                             { jlStatus = JL_STATUS_VALUE_OUT_OF_RANGE;  }
            break;
        }
        case sizeof(int16_t):
        {
            int16_t* OutputS16 = Output;
            if( number <= INT16_MAX && number >= INT16_MIN ) { *OutputS16 = (int16_t) number;  }
            else                                             { jlStatus = JL_STATUS_VALUE_OUT_OF_RANGE;  }
            break;
        }
        case sizeof(int8_t):
        {
            int8_t* OutputS8 = Output;
            if( number <= INT8_MAX && number >= INT8_MIN )   { *OutputS8 = (int8_t) number;  }
            else                                             { jlStatus = JL_STATUS_VALUE_OUT_OF_RANGE; }
            break;
        }
        default:
            jlStatus = JL_STATUS_INVALID_SPECIFICATION;
            break;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshalFloat
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshalFloat
    (
        JlDataObject const*         NumberObject,
        void*                       Output,
        size_t                      FixedSize
    )
{
    JL_STATUS jlStatus;
    double number = 0.0;

    jlStatus = JlGetObjectNumberF64( NumberObject, &number );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        switch( FixedSize )
        {
        case sizeof(double):
        {
            double* OutputF64 = Output;
            *OutputF64 = number;
            break;
        }
        case sizeof(float):
        {
            float* OutputF32 = Output;
            if( number <= FLT_MAX && number >= FLT_MIN ) { *OutputF32 = (float) number;  }
            else                                         { jlStatus = JL_STATUS_VALUE_OUT_OF_RANGE;  }
            break;
        }
        default:
            jlStatus = JL_STATUS_INVALID_SPECIFICATION;
            break;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshalBool
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshalBool
    (
        JlDataObject const*         NumberObject,
        void*                       Output,
        size_t                      FixedSize
    )
{
    JL_STATUS jlStatus;
    bool boolValue = false;

    jlStatus = JlGetObjectBool( NumberObject, &boolValue );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        // Allow for different size bools: 1 byte, 2 bytes, 4 bytes, or 8 bytes
        switch( FixedSize )
        {
        case sizeof(uint64_t):
        {
            uint64_t* OutputU64 = Output;
            *OutputU64 = (uint64_t) boolValue;
            break;
        }
        case sizeof(uint32_t):
        {
            uint32_t* OutputU32 = Output;
            *OutputU32 = (uint32_t) boolValue;
            break;
        }
        case sizeof(uint16_t):
        {
            uint16_t* OutputU16 = Output;
            *OutputU16 = (uint16_t) boolValue;
            break;
        }
        case sizeof(uint8_t):
        {
            uint8_t* OutputU8 = Output;
            *OutputU8 = (uint8_t) boolValue;
            break;
        }
        default:
            jlStatus = JL_STATUS_INVALID_SPECIFICATION;
            break;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshalBinary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshalBinary
    (
        JlDataObject const*         Object,
        void*                       DataSizeField,
        size_t                      DataSizeFieldSize,
        JlMarshallElement const*    Description,
        bool                        IgnoreExistingValuesInStruct,
        void*                       Output,
        size_t*                     pErrorAtPos
    )
{
    JL_STATUS jlStatus;
    char const* stringPtr = NULL;
    void** BufferPtr = 0==Description->FieldSize ? (void**)Output : NULL;    // Only when its an allocated buffer

    if( !IgnoreExistingValuesInStruct && 0==Description->FieldSize && NULL != *BufferPtr )
    {
        // Free the existing buffer here. Note this requires that the buffer had been allocated by this library
        // on a previous unmarshall, and was not pointing to a static string or junk memory.
        JlFree( *BufferPtr );
        *BufferPtr = NULL;
    }

    jlStatus = JlGetObjectString( Object, &stringPtr );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        // Convert base64 to binary
        uint8_t* data = NULL;
        size_t dataSize = 0;

        if( NULL != stringPtr )
        {
            jlStatus = JlBase64Decode( stringPtr, (void**)&data, &dataSize );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                if( 0 == Description->FieldSize )
                {
                    // Set the pointer in struct to the allocated buffer
                    *BufferPtr = data;

                    // Set the size
                    jlStatus = JlMemoryWriteCountValue( DataSizeField, DataSizeFieldSize, dataSize );
                    if( JL_STATUS_SUCCESS == jlStatus )
                    {
                        // Success
                    }
                    else
                    {
                        // deallocate
                        *BufferPtr = NULL;
                        JlFree( data );
                        data = NULL;
                    }
                }
                else
                {
                    // Fixed sized buffer in struct.
                    if( dataSize == Description->FieldSize )
                    {
                        // Copy in data
                        memcpy( Output, data, dataSize );
                    }
                    else
                    {
                        // Base64 decoded to a different size from the fixed size buffer. This is invalid as
                        // there is no size field used.
                        jlStatus = JL_STATUS_INVALID_DATA;
                    }

                    JlFree( data );
                    data = NULL;
                }
            }
        }
        else
        {
            // Null string.
            if( 0 == Description->FieldSize )
            {
                *BufferPtr = NULL;
                jlStatus = JlMemoryWriteCountValue( DataSizeField, DataSizeFieldSize, 0 );
            }
            else
            {
                // Invalid for a fixed size field
                jlStatus = JL_STATUS_INVALID_DATA;
            }
        }
    }

    if( JL_STATUS_SUCCESS != jlStatus )
    {
        if( NULL != pErrorAtPos )
        {
            *pErrorAtPos = JlGetObjectTag( Object );
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshalObject
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshalObject
    (
        JlDataObject const*         Object,
        JlMarshallElement const*    Description,
        bool                        IgnoreExistingValuesInStruct,
        void*                       Output,
        size_t*                     pErrorAtPos
    )
{
    JL_STATUS jlStatus;

    if( JL_DATA_TYPE_STRING == Description->Type  &&  0 == Description->FieldSize )
    {
        // Regular allocated string.
        // Output is a pointer to a string.
        jlStatus = UnmarshalString( Object, IgnoreExistingValuesInStruct, Output );
    }
    else if( JL_DATA_TYPE_STRING == Description->Type  &&  Description->FieldSize > 0 )
    {
        // Special case: This is a fixed size string in a structure
        jlStatus = UnmarshalStringFixed( Object, Output, Description->FieldSize );
    }
    else if( JL_DATA_TYPE_NUMBER == Description->Type  &&  JL_NUM_TYPE_UNSIGNED == Description->NumberType )
    {
        jlStatus = UnmarshalUnsigned( Object, Output, Description->FieldSize );
    }
    else if( JL_DATA_TYPE_NUMBER == Description->Type  &&  JL_NUM_TYPE_SIGNED == Description->NumberType )
    {
        jlStatus = UnmarshalSigned( Object, Output, Description->FieldSize );
    }
    else if( JL_DATA_TYPE_NUMBER == Description->Type  &&  JL_NUM_TYPE_FLOAT == Description->NumberType )
    {
        jlStatus = UnmarshalFloat( Object, Output, Description->FieldSize );
    }
    else if( JL_DATA_TYPE_BOOL == Description->Type )
    {
        jlStatus = UnmarshalBool( Object, Output, Description->FieldSize );
    }
    else if( JL_DATA_TYPE_DICTIONARY == Description->Type )
    {
        jlStatus = UnmarshallDictionary(
            Object,
            Description->ChildStructDescription, Description->ChildStructDescriptionCount,
            IgnoreExistingValuesInStruct,
            Output,
            pErrorAtPos );
    }
    else
    {
        // Unknown data type
        jlStatus = JL_STATUS_INVALID_TYPE;
    }

    if(     JL_STATUS_SUCCESS != jlStatus
        &&  JL_DATA_TYPE_DICTIONARY != Description->Type
        &&  NULL != pErrorAtPos )
    {
        *pErrorAtPos = JlGetObjectTag( Object );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshalList
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshalList
    (
        JlDataObject const*         ListObject,
        JlMarshallElement const*    Description,
        bool                        IgnoreExistingValuesInStruct,
        void*                       Output,
        void*                       ArrayCountPtr,
        size_t                      ArrayCountSize,
        size_t*                     pErrorAtPos
    )
{
    JL_STATUS jlStatus;
    uint32_t listCount = JlGetListCount( ListObject );
    void* array = NULL;

    // Get existing count value and then set to zero.
    size_t originalCountValue = JlMemoryReadCountValue( ArrayCountPtr, ArrayCountSize );
    size_t newCountValue = 0;
    (void) JlMemoryWriteCountValue( ArrayCountPtr, ArrayCountSize, newCountValue );

    // Check if there already is a list allocated and free it first.
    if( !IgnoreExistingValuesInStruct && originalCountValue > 0 )
    {
        // There is already a list allocated from a previous unmarshall, so we need to free that first.
        (void) FreeUnmarshalledList( Description, Output, originalCountValue );
    }

    if( listCount > 0 )
    {
        if( 0 == Description->ArrayFieldSize )
        {
            // We need to allocate the array of objects
            void** listPtr = Output;

            size_t allocSize = Description->ArrayItemSize * listCount;
            void* listAllocation = JlAlloc( allocSize );
            if( NULL != listAllocation )
            {
                *listPtr = listAllocation;
                array = listAllocation;
                jlStatus = JL_STATUS_SUCCESS;
            }
            else
            {
                jlStatus = JL_STATUS_OUT_OF_MEMORY;
            }
        }
        else
        {
            // Fixed array size in struct
            // Zero it first. ArrayFieldSize is the sizeof the entire fixed array
            memset( Output, 0, Description->ArrayFieldSize );

            // Determine if there is enough room.
            size_t maxItemsInFixedArray = Description->ArrayFieldSize / Description->ArrayItemSize;
            if( listCount <= maxItemsInFixedArray )
            {
                array = Output;
                jlStatus = JL_STATUS_SUCCESS;
            }
            else
            {
                // More elements that we have room for.
                jlStatus = JL_STATUS_TOO_MANY_ITEMS;
            }
        }

        if( JL_STATUS_SUCCESS == jlStatus )
        {
            JlListItem* enumerator = NULL;
            for( uint32_t i=0; i<listCount  &&  JL_STATUS_SUCCESS==jlStatus; i++ )
            {
                void* elementInArrayPtr = ((uint8_t*)array) + (Description->ArrayItemSize * i);
                JlDataObject* object = NULL;
                jlStatus = JlGetObjectListNextItem( ListObject, &object, &enumerator );
                if( JL_STATUS_SUCCESS == jlStatus )
                {
                    JL_DATA_TYPE objectType = JlGetObjectType( object );
                    if( Description->Type == objectType )
                    {
                        jlStatus = UnmarshalObject( object, Description, IgnoreExistingValuesInStruct, elementInArrayPtr, pErrorAtPos );
                        if( JL_STATUS_SUCCESS == jlStatus )
                        {
                            newCountValue += 1;
                            (void) JlMemoryWriteCountValue( ArrayCountPtr, ArrayCountSize, newCountValue );
                        }
                    }
                    else
                    {
                        // Wrong type in list. All elements must be of the same type to go into a C array.
                        jlStatus = JL_STATUS_WRONG_TYPE;
                        if( NULL != pErrorAtPos )
                        {
                            *pErrorAtPos = JlGetObjectTag( object );
                        }
                    }
                }
            }
        }
    }
    else
    {
        // List count is 0
        if( 0 == Description->ArrayFieldSize )
        {
            void** listPtr = Output;
            *listPtr = NULL;
        }
        else
        {
            // Fixed array size in struct
            memset( Output, 0, Description->ArrayFieldSize );
        }
        jlStatus = JL_STATUS_SUCCESS;
    }

    if(     JL_STATUS_SUCCESS != jlStatus
        &&  NULL != pErrorAtPos
        &&  0 == *pErrorAtPos )
    {
        // If we haven't already set an error position then use the position of this object (if it exists)
        *pErrorAtPos = JlGetObjectTag( ListObject );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  UnmarshallDictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    UnmarshallDictionary
    (
        JlDataObject const*         DictionaryObject,
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        bool                        IgnoreExistingValuesInStruct,
        void*                       Structure,
        size_t*                     pErrorAtPos
    )
{
    JL_STATUS jlStatus = JL_STATUS_SUCCESS;

    for( uint32_t i=0; i<StructDescriptionCount && JL_STATUS_SUCCESS==jlStatus; i++ )
    {
        JlMarshallElement const* currentItem = &StructDescription[i];

        // Get object
        JlDataObject* object = NULL;
        JL_DATA_TYPE objectType = JL_DATA_TYPE_NONE;

        jlStatus = JlGetObjectFromDictionaryByKey( DictionaryObject, currentItem->Name, &object );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            void* elementInStruct = (void*)(((uint8_t*)Structure)+currentItem->FieldOffset);
            objectType = JlGetObjectType( object );

            if( currentItem->IsArray && JL_DATA_TYPE_LIST == objectType )
            {
                // First verify that the count field is large enough
                size_t arrayCount = JlGetListCount( object );
                jlStatus = JlMemoryVerifyCountFieldLargeEnoughForValue( currentItem->CountFieldSize, arrayCount );
                if( JL_STATUS_SUCCESS == jlStatus )
                {
                    // Unmarshall the list
                    void* arrayCountPtr = ((uint8_t*)Structure) + currentItem->CountFieldOffset;
                    jlStatus = UnmarshalList(
                        object,
                        currentItem,
                        IgnoreExistingValuesInStruct,
                        elementInStruct,
                        arrayCountPtr,
                        currentItem->CountFieldSize,
                        pErrorAtPos );
                }

                if( JL_STATUS_SUCCESS != jlStatus )
                {
                    if( NULL != pErrorAtPos && 0 == *pErrorAtPos )
                    {
                        *pErrorAtPos = JlGetObjectTag( object );
                    }
                }
            }
            else if( currentItem->Type == objectType )
            {
                if( currentItem->IsBase64 )
                {
                    void* dataSizeElementInStruct = (void*)(((uint8_t*)Structure)+currentItem->CountFieldOffset);
                    jlStatus = UnmarshalBinary( object, dataSizeElementInStruct, currentItem->CountFieldSize, currentItem, IgnoreExistingValuesInStruct, elementInStruct, pErrorAtPos );
                }
                else
                {
                    jlStatus = UnmarshalObject( object, currentItem, IgnoreExistingValuesInStruct, elementInStruct, pErrorAtPos );
                }
            }
            else
            {
                // Wrong type
                jlStatus = JL_STATUS_WRONG_TYPE;
                if( NULL != pErrorAtPos )
                {
                    *pErrorAtPos = JlGetObjectTag( object );
                }
            }

        }
        else if( JL_STATUS_NOT_FOUND == jlStatus )
        {
            // Keyname not found in dictionary. Do not consider this an error, we just don't have any value for this item, so we leave
            // it as default
            jlStatus = JL_STATUS_SUCCESS;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Freeing functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  FreeUnmarshalledList
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    FreeUnmarshalledList
    (
        JlMarshallElement const*    Description,
        void*                       ListPtr,
        size_t                      ListCount
    )
{
    JL_STATUS jlStatus = JL_STATUS_SUCCESS;
    JL_DATA_TYPE type = Description->Type;
    void* array = NULL;

    if( 0 == Description->ArrayFieldSize )
    {
        // Then the array container was allocated, and ListPtr points to the array container.
        void** allocatedArrayPtr = ListPtr;
        array = *allocatedArrayPtr;
    }
    else
    {
        // Fixed array, so ListPtr is pointing to the start of the array in the struct
        array = ListPtr;
    }

    for( size_t i=0; i<ListCount; i++ )
    {
        void* itemPtr = ((uint8_t*)array) + ( Description->ArrayItemSize * i );
        if( JL_DATA_TYPE_STRING == type )
        {
            char** stringPtr = itemPtr;

            if( NULL != *stringPtr )
            {
                JlFree( *stringPtr );
                *stringPtr = NULL;
            }
        }
        if( JL_DATA_TYPE_DICTIONARY == type )
        {
            jlStatus = FreeUnmarshalledDictionary( Description->ChildStructDescription, Description->ChildStructDescriptionCount, itemPtr );
        }
    }

    if( 0 == Description->ArrayFieldSize )
    {
        // Deallocate the container
        void** allocatedArrayPtr = ListPtr;

        if( NULL != *allocatedArrayPtr )
        {
            JlFree( *allocatedArrayPtr );
            *allocatedArrayPtr = NULL;
        }
    }

    jlStatus = JL_STATUS_SUCCESS;

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  FreeUnmarshalledDictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    FreeUnmarshalledDictionary
    (
        JlMarshallElement const*    StructDescription,
        size_t                      StructDescriptionCount,
        void*                       Structure
    )
{
    JL_STATUS jlStatus = JL_STATUS_SUCCESS;

    for( uint32_t i=0; i<StructDescriptionCount && JL_STATUS_SUCCESS==jlStatus; i++ )
    {
        JlMarshallElement const* itemDescription = &StructDescription[i];
        JL_DATA_TYPE type = itemDescription->Type;
        void* itemPtr = ((uint8_t*)Structure)+itemDescription->FieldOffset;

        if( itemDescription->IsArray )
        {
            void* countFieldPtr = ((uint8_t*)Structure) + itemDescription->CountFieldOffset;
            size_t arrayCount = JlMemoryReadCountValue( countFieldPtr, itemDescription->CountFieldSize );
            jlStatus = FreeUnmarshalledList( itemDescription, itemPtr, arrayCount );
            (void) JlMemoryWriteCountValue( countFieldPtr, itemDescription->CountFieldSize, 0 );
        }
        else if( JL_DATA_TYPE_DICTIONARY == type )
        {
            jlStatus = FreeUnmarshalledDictionary( itemDescription->ChildStructDescription, itemDescription->ChildStructDescriptionCount, itemPtr );
        }
        else if( JL_DATA_TYPE_STRING == type )
        {
            if( 0 == itemDescription->FieldSize )
            {
                // 0 means that itemPtr is pointing to a char* that was allocated
                char** stringPtr = itemPtr;
                if( NULL != *stringPtr )
                {
                    JlFree( *stringPtr );
                    *stringPtr = NULL;
                }
                jlStatus = JL_STATUS_SUCCESS;
            }
            else
            {
                // Fixed sized string in struct, so we don't deallocate it
                jlStatus = JL_STATUS_SUCCESS;
            }
        }
        else
        {
            // Is a Number, Bool. These are not allocated
            jlStatus = JL_STATUS_SUCCESS;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    )
{
    return JlUnmarshallToStructEx( DictionaryObject, StructDescription, StructDescriptionCount, false, Structure, pErrorAtPos );
}

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
    )
{
    JL_STATUS jlStatus;

    if(     NULL != DictionaryObject
        &&  NULL != StructDescription
        &&  0 != StructDescriptionCount
        &&  NULL != Structure )
    {
        if( NULL != pErrorAtPos )
        {
            *pErrorAtPos = 0;
        }

        if( JL_DATA_TYPE_DICTIONARY == JlGetObjectType( DictionaryObject ) )
        {
            jlStatus = UnmarshallDictionary(
                DictionaryObject,
                StructDescription, StructDescriptionCount,
                IgnoreExistingValuesInStruct,
                Structure,
                pErrorAtPos );
            if( JL_STATUS_SUCCESS != jlStatus )
            {
                // Deallocate items we may have allocated before the error occurred. Note this will also deallocate any
                // items that were in the structure before hand, however the structure is not guaranteed to be preserved from
                // original at this point anyway because other items may have been overwritten already.
                (void) JlUnmarshallFreeStructAllocs( StructDescription, StructDescriptionCount, Structure );
            }
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
    )
{
    JL_STATUS jlStatus;

    if(     NULL != StructDescription
        &&  0 != StructDescriptionCount
        &&  NULL != Structure )
    {
        jlStatus = FreeUnmarshalledDictionary( StructDescription, StructDescriptionCount, Structure );
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}
