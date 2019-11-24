////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JsonLib
//
//  This module produces JSON output from a JlDataObject tree
//
//  This is free and unencumbered software released into the public domain - November 2019 waterjuice.org
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IMPORTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JsonLib.h"
#include "JlBuffer.h"
#include "JlMemory.h"
#include "JlUnicode.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef _MSC_VER
    #include <inttypes.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  DEFINES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
    #define PRIu64 "llu"
    #define PRId64 "lld"
    #define PRIx64 "llx"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TYPES
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    JlDataObject const* Object;
    JL_DATA_TYPE        ObjectType;
    bool                StartedEnumerating;
    bool                ProcessedFirstItem;
    union
    {
        JlListItem*         ListEnumerator;
        JlDictionaryItem*   DictionaryEnumerator;
    };

} ProcessStack;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  AddNewLineAndIndentation
//
//  Adds a new line character and indentation to the JlBuffer if IndentedFormat is true, otherwise it adds nothing.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    AddNewLineAndIndentation
    (
        JlBuffer*       JsonBuffer,
        bool            IndentedFormat,
        uint32_t        Depth
    )
{
    JL_STATUS jlStatus;

    if( IndentedFormat )
    {
        if( JlBufferGetDataSize( JsonBuffer ) > 0 )
        {
            jlStatus = JlBufferAdd( JsonBuffer, "\n", 1 );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                char indentString [MAX_JSON_DEPTH*4];
                memset( indentString, ' ', sizeof(indentString) );

                if( Depth > 1 )
                {
                    jlStatus = JlBufferAdd( JsonBuffer, indentString, (Depth-1)*4 );
                }
            }
        }
        else
        {
            // Suppress first newline and indent
            jlStatus = JL_STATUS_SUCCESS;
        }
    }
    else
    {
        jlStatus = JL_STATUS_SUCCESS;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputUtf8String
//
//  Outputs the Utf8String to the JsonBuffer. This will escape necessary characters (control chars, and quote char).
//  Additionally if EscapeAllNonAscii is true then this will escape all characters over 127. Escaped characters will
//  use UTF16 escaped notation.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    OutputUtf8String
    (
        char const*         Utf8String,
        bool                EscapeAllNonAscii,
        bool                EscapeSingleQuote,
        JlBuffer*           JsonBuffer
    )
{
    JL_STATUS jlStatus = JL_STATUS_SUCCESS;
    size_t Utf8StringLength = strlen( Utf8String );
    size_t index = 0;

    while( index < Utf8StringLength  &&  JL_STATUS_SUCCESS == jlStatus )
    {
        // Collect next UTF8 character
        uint32_t unicodeValue = 0;
        size_t numBytes;

        jlStatus = JlUnicodeCharFromUtf8( Utf8String+index, Utf8StringLength-index, &unicodeValue, &numBytes );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            if(    (unicodeValue >= 32 && unicodeValue <= 127 && unicodeValue != '\"' && unicodeValue != '\\' && unicodeValue != '\'')
                || (unicodeValue > 127 && !EscapeAllNonAscii) )
            {
                // Single ASCII character, or multibyte utf8 and we are not escaping all non ascii. No need
                // to use the conversion to unicode, just copy in the original bytes
                jlStatus = JlBufferAdd( JsonBuffer, Utf8String+index, numBytes );
            }
            else if( '\"' == unicodeValue )
            {
                jlStatus = JlBufferAdd( JsonBuffer, "\\\"", 2 );
            }
            else if( '\\' == unicodeValue )
            {
                jlStatus = JlBufferAdd( JsonBuffer, "\\\\", 2 );
            }
            else if( '\b' == unicodeValue )
            {
                jlStatus = JlBufferAdd( JsonBuffer, "\\b", 2 );
            }
            else if( '\f' == unicodeValue )
            {
                jlStatus = JlBufferAdd( JsonBuffer, "\\f", 2 );
            }
            else if( '\n' == unicodeValue )
            {
                jlStatus = JlBufferAdd( JsonBuffer, "\\n", 2 );
            }
            else if( '\r' == unicodeValue )
            {
                jlStatus = JlBufferAdd( JsonBuffer, "\\r", 2 );
            }
            else if( '\t' == unicodeValue )
            {
                jlStatus = JlBufferAdd( JsonBuffer, "\\t", 2 );
            }
            else if( '\'' == unicodeValue )
            {
                if( EscapeSingleQuote )
                {
                    jlStatus = JlBufferAdd( JsonBuffer, "\\'", 2 );
                }
                else
                {
                    jlStatus = JlBufferAdd( JsonBuffer, "'", 1 );
                }
            }
            else
            {
                // Requires escaping using UTF16 escaped notation \uxxxx[\uxxxx]
                if( unicodeValue < 0xffff )
                {
                    // Requires a single UTF16 word
                    char string[7] = {0};
                    sprintf( string, "\\u%4.4x", unicodeValue );
                    jlStatus = JlBufferAdd( JsonBuffer, string, 6 );
                }
                else
                {
                    // Requires two UTF16 words (high and low surrogate pairs)
                    char string[13] = {0};
                    uint32_t highSurrogate = ( unicodeValue - 0x10000 ) >> 10;
                    uint32_t lowSurrogate  = ( unicodeValue - 0x10000 ) & 0x03ff;
                    sprintf( string, "\\u%4.4x\\u%4.4x", highSurrogate, lowSurrogate );
                    jlStatus = JlBufferAdd( JsonBuffer, string, 12 );
                }
            }
        }

        index += numBytes;
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputStringObject
//
//  Outputs a string object to the json buffer.
//  Warning: This does not check that the object is a string object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputStringObject
    (
        JlDataObject const*     StringObject,
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags,
        uint32_t                StackDepth
    )
{
    JL_STATUS jlStatus;
    char const* string = NULL;
    char const* const quoteCharStr = OutputFlags & JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES ? "\'" : "\"";
    bool escapeSingleQuote = (OutputFlags & JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES) ? true : false;
    bool escapeNonAscii = (OutputFlags & JL_OUTPUT_FLAGS_ASCII) ? true : false;

    jlStatus = JlGetObjectString( StringObject, &string );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        if( NULL != string )
        {
            jlStatus = JlBufferAdd( JsonBuffer, quoteCharStr, 1 );
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                jlStatus = OutputUtf8String( string, escapeNonAscii, escapeSingleQuote, JsonBuffer );
            }
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                jlStatus = JlBufferAdd( JsonBuffer, quoteCharStr, 1 );
            }
        }
        else
        {
            jlStatus = JlBufferAdd( JsonBuffer, "null", 4 );
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputNumberObject
//
//  Outputs a number object to the json buffer.
//  Warning: This does not check that the object is a number object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputNumberObject
    (
        JlDataObject const*     NumberObject,
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags,
        uint32_t                StackDepth
    )
{
    JL_STATUS jlStatus;
    char numString [64] = "";

    JL_NUM_TYPE numType = JlGetObjectNumberType( NumberObject );
    if( JL_NUM_TYPE_UNSIGNED == numType )
    {
        uint64_t numU64 = 0;
        jlStatus = JlGetObjectNumberU64( NumberObject, &numU64 );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            if(     OutputFlags & JL_OUTPUT_FLAGS_J5_ALLOW_HEX
                &&  JlIsObjectNumberHex( NumberObject ) )
            {
                // Format hex as 2 digits, 4 digits, 8 digits, or 16 digits depending on value
                if     ( numU64 <= 0xff )       { sprintf( numString, "0x%2.2"PRIx64, numU64 ); }
                else if( numU64 <= 0xffff )     { sprintf( numString, "0x%4.4"PRIx64, numU64 ); }
                else if( numU64 <= 0xffffffff ) { sprintf( numString, "0x%8.8"PRIx64, numU64 ); }
                else                            { sprintf( numString, "0x%16.16"PRIx64, numU64 ); }
            }
            else
            {
                sprintf( numString, "%"PRIu64, numU64 );
            }
        }
    }
    else if( JL_NUM_TYPE_SIGNED == numType )
    {
        int64_t numS64 = 0;
        jlStatus = JlGetObjectNumberS64( NumberObject, &numS64 );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            sprintf( numString, "%"PRId64, numS64 );
        }
    }
    else if( JL_NUM_TYPE_FLOAT == numType )
    {
        // Now try getting as a double
        double numF64 = 0.0;
        jlStatus = JlGetObjectNumberF64( NumberObject, &numF64 );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            // Note: Output is printing using %.16g which gives the best compromise between precision and sensibly
            // printing out most numbers. While %.17g would give full precision it will cause simple numbers such
            // as 2.1 to print as 2.1000000000000001 due to the limitations of the floating point format.
            sprintf( numString, "%.16g", numF64 );
        }
    }
    else
    {
        jlStatus = JL_STATUS_WRONG_TYPE;
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        jlStatus = JlBufferAdd( JsonBuffer, numString, strlen(numString) );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputBoolObject
//
//  Outputs a boolean object to the json buffer.
//  Warning: This does not check that the object is a boolean object
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputBoolObject
    (
        JlDataObject const*     BoolObject,
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags,
        uint32_t                StackDepth
    )
{
    JL_STATUS jlStatus;
    bool boolValue = false;

    jlStatus = JlGetObjectBool( BoolObject, &boolValue );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        char const* boolStringPtr = boolValue ? "true" : "false";
        jlStatus = JlBufferAdd( JsonBuffer, boolStringPtr, strlen(boolStringPtr) );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputListStart
//
//  Outputs the opening of a new list
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputListStart
    (
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags,
        uint32_t                StackDepth
    )
{
    JL_STATUS jlStatus;

    jlStatus = JlBufferAdd( JsonBuffer, "[", 1 );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        jlStatus = AddNewLineAndIndentation(
            JsonBuffer,
            OutputFlags & JL_OUTPUT_FLAGS_INDENT ? true : false,
            StackDepth+1 );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputListEnd
//
//  Outputs the closing of a list
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputListEnd
    (
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags,
        uint32_t                StackDepth
    )
{
    JL_STATUS jlStatus;

    jlStatus = AddNewLineAndIndentation(
        JsonBuffer,
        OutputFlags & JL_OUTPUT_FLAGS_INDENT ? true : false,
        StackDepth );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        jlStatus = JlBufferAdd( JsonBuffer, "]", 1 );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputListBetween
//
//  Outputs the comma within a list
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputListBetween
    (
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags,
        uint32_t                StackDepth
    )
{
    JL_STATUS jlStatus;

    jlStatus = JlBufferAdd( JsonBuffer, ",", 1 );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        jlStatus = AddNewLineAndIndentation(
            JsonBuffer,
            OutputFlags & JL_OUTPUT_FLAGS_INDENT ? true : false,
            StackDepth );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ProcessListOnStack
//
//  Processes list item on stack
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    ProcessListOnStack
    (
        ProcessStack*       Stack,
        uint32_t*           pStackDepth,
        JlBuffer*           JsonBuffer,
        JL_OUTPUT_FLAGS     OutputFlags
    )
{
    JL_STATUS jlStatus;
    ProcessStack* currentItem = &Stack[*pStackDepth];

    jlStatus = JL_STATUS_SUCCESS;

    if( !currentItem->StartedEnumerating )
    {
        // Start a list
        jlStatus = OutputListStart( JsonBuffer, OutputFlags, *pStackDepth );
        currentItem->StartedEnumerating = true;
        currentItem->ListEnumerator = NULL;
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        // Add next list item to stack
        JlDataObject* nextObject = NULL;
        jlStatus = JlGetObjectListNextItem( currentItem->Object, &nextObject, &currentItem->ListEnumerator );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            // Add item to stack
            ProcessStack* newItem = &Stack[*pStackDepth+1];
            memset( newItem, 0, sizeof(ProcessStack) );
            newItem->Object = nextObject;
            newItem->ObjectType = JlGetObjectType( nextObject );
            *pStackDepth += 1;

            if( currentItem->ProcessedFirstItem )
            {
                // Need to add a comma.
                jlStatus = OutputListBetween( JsonBuffer, OutputFlags, *pStackDepth );
            }
            currentItem->ProcessedFirstItem = true;
        }
        else if( JL_STATUS_END_OF_DATA == jlStatus )
        {
            // End list
            if( OutputFlags & JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS && currentItem->ProcessedFirstItem )
            {
                // Add trailing comma
                (void) JlBufferAdd( JsonBuffer, ",", 1 );
            }
            jlStatus = OutputListEnd( JsonBuffer, OutputFlags, *pStackDepth );
            *pStackDepth -=1;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputDictionaryStart
//
//  Outputs the opening of a new dictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputDictionaryStart
    (
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags,
        uint32_t                StackDepth
    )
{
    JL_STATUS jlStatus;

    jlStatus = JlBufferAdd( JsonBuffer, "{", 1 );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        jlStatus = AddNewLineAndIndentation(
            JsonBuffer,
            OutputFlags & JL_OUTPUT_FLAGS_INDENT ? true : false,
            StackDepth+1 );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputDictionaryEnd
//
//  Outputs the closing of a dictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputDictionaryEnd
    (
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags,
        uint32_t                StackDepth
    )
{
    JL_STATUS jlStatus;

    jlStatus = AddNewLineAndIndentation(
        JsonBuffer,
        OutputFlags & JL_OUTPUT_FLAGS_INDENT ? true : false,
        StackDepth );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        jlStatus = JlBufferAdd( JsonBuffer, "}", 1 );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputDictionaryBetween
//
//  Outputs the comma within a dictionary
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputDictionaryBetween
    (
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags,
        uint32_t                StackDepth
    )
{
    JL_STATUS jlStatus;

    jlStatus = JlBufferAdd( JsonBuffer, ",", 1 );
    if( JL_STATUS_SUCCESS == jlStatus )
    {
        jlStatus = AddNewLineAndIndentation(
            JsonBuffer,
            OutputFlags & JL_OUTPUT_FLAGS_INDENT ? true : false,
            StackDepth );
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  CanKeyNameBeBare
//
//  Returns true if the KeyName can be used as a bare keyword
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static
JL_STATUS
    CanKeyNameBeBare
    (
        char const*             KeyName
    )
{
    bool canBeBare = true;
    size_t length = strlen( KeyName );

    for( size_t i=0; i<length; i++ )
    {
        char const currentChar = KeyName[i];
        if(     ( currentChar >= 'a' && currentChar <= 'z' )
            ||  ( currentChar >= 'A' && currentChar <= 'Z' )
            ||  '_' == currentChar
            ||  '$' == currentChar )
        {
            // Valid char for bareword
        }
        else if(    i > 0
                 && ( currentChar >= '0' && currentChar <= '9' ) )
        {
            // Valid char (as long as it is not the first char
        }
        else
        {
            // Character can not be in a bare keyword
            canBeBare = false;
            break;
        }
    }

    return canBeBare;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  OutputDictionaryKey
//
//  Outputs the dictionary key name
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    OutputDictionaryKey
    (
        char const*             KeyName,
        JlBuffer*               JsonBuffer,
        JL_OUTPUT_FLAGS         OutputFlags
    )
{
    JL_STATUS jlStatus;
    bool noQuotes = false;
    char* quoteCharStr = "\"";

    if( OutputFlags & JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS )
    {
        // See if this qualifies as bare keyword
        if( CanKeyNameBeBare( KeyName ) )
        {
            noQuotes = true;
        }
    }
    if( !noQuotes && OutputFlags & JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES )
    {
        quoteCharStr = "\'";
    }

    if( !noQuotes )
    {
        jlStatus = JlBufferAdd( JsonBuffer, quoteCharStr, 1 );
    }

    jlStatus = JlBufferAdd( JsonBuffer, KeyName, strlen(KeyName) );

    if( !noQuotes )
    {
        jlStatus = JlBufferAdd( JsonBuffer, quoteCharStr, 1 );
    }
    jlStatus = JlBufferAdd( JsonBuffer, ": ", OutputFlags & JL_OUTPUT_FLAGS_INDENT ? 2 : 1 );

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ProcessDictionaryOnStack
//
//  Processes dictionary item on stack
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    ProcessDictionaryOnStack
    (
        ProcessStack*       Stack,
        uint32_t*           pStackDepth,
        JlBuffer*           JsonBuffer,
        JL_OUTPUT_FLAGS     OutputFlags
    )
{
    JL_STATUS jlStatus;
    ProcessStack* currentItem = &Stack[*pStackDepth];

    jlStatus = JL_STATUS_SUCCESS;

    if( !currentItem->StartedEnumerating )
    {
        // Start a dictionary
        jlStatus = OutputDictionaryStart( JsonBuffer, OutputFlags, *pStackDepth );
        currentItem->StartedEnumerating = true;
        currentItem->DictionaryEnumerator = NULL;
    }

    if( JL_STATUS_SUCCESS == jlStatus )
    {
        // Add next dictionary item to stack
        JlDataObject* nextObject = NULL;
        char const* nextObjectKey = NULL;
        jlStatus = JlGetObjectDictionaryNextItem( currentItem->Object, &nextObject, &nextObjectKey, &currentItem->DictionaryEnumerator );
        if( JL_STATUS_SUCCESS == jlStatus )
        {
            // Add item to stack
            ProcessStack* newItem = &Stack[*pStackDepth+1];
            memset( newItem, 0, sizeof(ProcessStack) );
            newItem->Object = nextObject;
            newItem->ObjectType = JlGetObjectType( nextObject );
            *pStackDepth += 1;

            if( currentItem->ProcessedFirstItem )
            {
                // Need to add a comma.
                jlStatus = OutputDictionaryBetween( JsonBuffer, OutputFlags, *pStackDepth );
            }
            currentItem->ProcessedFirstItem = true;

            // Output Key name
            if( JL_STATUS_SUCCESS == jlStatus )
            {
                jlStatus = OutputDictionaryKey( nextObjectKey, JsonBuffer, OutputFlags );
            }
        }
        else if( JL_STATUS_END_OF_DATA == jlStatus )
        {
            // End list
            if( OutputFlags & JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS && currentItem->ProcessedFirstItem )
            {
                // Add trailing comma
                (void) JlBufferAdd( JsonBuffer, ",", 1 );
            }
            jlStatus = OutputDictionaryEnd( JsonBuffer, OutputFlags, *pStackDepth );
            *pStackDepth -=1;
        }
    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ProcessTheStack
//
//  Processes the tree stack. This must be called with the root object as the first element in the stack. This will
//  add items to the stack as it proceeds down the chain. This will build up a JlBuffer containing the JSON as it
//  processes each item. This returns when the root item is popped off the stack.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    ProcessTheStack
    (
        ProcessStack*   Stack,
        JlBuffer*       JsonBuffer,
        JL_OUTPUT_FLAGS OutputFlags
    )
{
    JL_STATUS jlStatus;
    uint32_t stackDepth = 1;

    jlStatus = JL_STATUS_SUCCESS;

    while( stackDepth > 0  &&  JL_STATUS_SUCCESS == jlStatus )
    {
        // Process current deepest stack item
        ProcessStack* currentItem = &Stack[stackDepth];

        if( JL_DATA_TYPE_STRING == currentItem->ObjectType )
        {
            // String object. Output the string
            jlStatus = OutputStringObject( currentItem->Object, JsonBuffer, OutputFlags, stackDepth );
            stackDepth -= 1;
        }
        else if( JL_DATA_TYPE_NUMBER == currentItem->ObjectType )
        {
            // Number object. Output the number
            jlStatus = OutputNumberObject( currentItem->Object, JsonBuffer, OutputFlags, stackDepth );
            stackDepth -= 1;
        }
        else if( JL_DATA_TYPE_BOOL == currentItem->ObjectType )
        {
            // Boolean object. Output the boolean
            jlStatus = OutputBoolObject( currentItem->Object, JsonBuffer, OutputFlags, stackDepth );
            stackDepth -= 1;
        }
        else if( JL_DATA_TYPE_LIST == currentItem->ObjectType )
        {
            jlStatus = ProcessListOnStack( Stack, &stackDepth, JsonBuffer, OutputFlags );
        }
        else if( JL_DATA_TYPE_DICTIONARY == currentItem->ObjectType )
        {
            jlStatus = ProcessDictionaryOnStack( Stack, &stackDepth, JsonBuffer, OutputFlags );
        }

    }

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    )
{
    JL_STATUS jlStatus;

    jlStatus = JlOutputJsonEx(
        DataObject,
        IndentedFormat ? JL_OUTPUT_FLAGS_INDENT : JL_OUTPUT_FLAGS_NONE,
        pJsonStringBuffer );

    return jlStatus;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  JlOutputJsonEx
//
//  Outputs JSON representing the JlDataObject. OutputFlags controls the output. If no flags are set then Json1
//  without formatting will be output. Flags can be combined.
//  Possible flags:
//      JL_OUTPUT_FLAGS_INDENT - Spaces and new lines will be inserted to make the layout readable.
//      JL_OUTPUT_FLAGS_J5_USE_BARE_KEYWORDS - Bare keywords will be used in dictionaries when possible
//      JL_OUTPUT_FLAGS_J5_SINGLE_QUOTES - Use single quotes instead of double quotes for strings.
//      JL_OUTPUT_FLAGS_J5_TRAILING_COMMAS - Place a trailing comma in lists and dictionaries.
//      JL_OUTPUT_FLAGS_J5_ALLOW_HEX - Allow hex numbers to be written if they are marked as hex in data object.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JL_STATUS
    JlOutputJsonEx
    (
        JlDataObject const*     DataObject,
        JL_OUTPUT_FLAGS         OutputFlags,
        char**                  pJsonStringBuffer
    )
{
    JL_STATUS jlStatus;

    if(     NULL != DataObject
        &&  NULL != pJsonStringBuffer )
    {
        *pJsonStringBuffer = NULL;

        JlBuffer* flexiBuffer = JlBufferCreate( );
        if( NULL != flexiBuffer )
        {
            ProcessStack* stack = JlAlloc( sizeof(ProcessStack) * MAX_JSON_DEPTH );
            if( NULL != stack )
            {
                // Place object on stack in position 1. Note we do not use stack[0] to store anything as we
                // are using position 0 to indicate stack is empty
                memset( &stack[1], 0, sizeof(stack[1]) );
                stack[1].Object = DataObject;
                stack[1].ObjectType = JlGetObjectType( DataObject );

                jlStatus = ProcessTheStack( stack, flexiBuffer, OutputFlags );
                if( JL_STATUS_SUCCESS == jlStatus )
                {
                    if( JL_OUTPUT_FLAGS_INDENT & OutputFlags )
                    {
                        // If indented formatting is on then always end last line with a newline char
                        JlBufferAdd( flexiBuffer, "\n", 1 );
                    }

                    // Copy out the string in the flexibuffer into a newly allocated string.
                    size_t stringLen = JlBufferGetDataSize( flexiBuffer );
                    char const* dataBuffer =  JlBufferGetDataBuffer( flexiBuffer );
                    char* string = JlAlloc( stringLen + 1 );
                    if( NULL != string )
                    {
                        // Copy out string
                        memcpy( string, dataBuffer, stringLen );
                        string[stringLen] = 0;
                        *pJsonStringBuffer = string;
                    }
                    else
                    {
                        jlStatus = JL_STATUS_OUT_OF_MEMORY;
                    }
                }

                JlFree( stack );
            }
            else
            {
                jlStatus = JL_STATUS_OUT_OF_MEMORY;
            }

            JlBufferFree( &flexiBuffer );
        }
        else
        {
            jlStatus = JL_STATUS_OUT_OF_MEMORY;
        }
    }
    else
    {
        jlStatus = JL_STATUS_INVALID_PARAMETER;
    }

    return jlStatus;
}
