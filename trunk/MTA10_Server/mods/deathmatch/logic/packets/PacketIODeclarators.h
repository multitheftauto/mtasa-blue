/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/PacketIODeclarators.h
*  PURPOSE:     Packet IO declaration macros
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETIODECLARATORS_H
#define __PACKETIODECLARATORS_H

class NetServerBitStreamInterface;

typedef unsigned short IO_STRINGTYPE;

enum IOCONTEXT_TYPE
{
    IOCONTEXT_READ,
    IOCONTEXT_WRITE
};

typedef struct __iocontext_t
{
    const IOCONTEXT_TYPE type;
    const char *         locator;
} IOCONTEXT;

/**
 * Returns the minimum of the two arguments.
 */
#define IO_MIN(a,b) \
    (( a < b ) ? a : b )

/**
 * Defines a code block which should only execute for
 * write operations.  Do not abuse this macro.
 */
#define IO_WRITE_BLOCK(codeblock) \
    if (IOCONTEXT_WRITE == ctx.type) \
    { \
        codeblock \
    }

/**
 * Defines a code block which should only execute for
 * read operations.  Do not abuse this macro.
 */
#define IO_READ_BLOCK(codeblock) \
    if (IOCONTEXT_READ == ctx.type) \
    { \
        codeblock \
    } 

/**
 * Essentially "throws" an error.
 */
#define IO_RAISE_ERROR(num,message) \
    printf("ERROR[%s/%d]: %s", ctx.locator, num, message) 

/**
 * Initializes a local variable with a value depending on the
 * context under which the variable is used.
 */
#define IO_INIT_LOCAL(type,varname,init_read,init_write) \
    type varname = (IOCONTEXT_READ == ctx.type) ? init_read : init_write 

/**
 * Writes a UINT to an implicit NetBitStreamInterface object.
 */
#define IO_UINT(intval) \
    (IOCONTEXT_WRITE == ctx.type) ? bitStream.Write(intval) : bitStream.Read(intval) 

inline bool io_variable_string_write_max(
    NetServerBitStreamInterface& bitStream, 
    const char * string, 
    IO_STRINGTYPE len,
    IO_STRINGTYPE maxlen)
{
    IO_STRINGTYPE stLenActual = IO_MIN(len, maxlen);
    bitStream.Write(stLenActual);
    bitStream.Write(const_cast<char*>(string), stLenActual);
    return true;
}

inline bool io_variable_string_read_max(
    NetServerBitStreamInterface& bitStream,
    const char * string,
    IO_STRINGTYPE maxlen)
{
    IO_STRINGTYPE stActualLen = 0;
    bool result = bitStream.Read(stActualLen) && 
        bitStream.Read(const_cast<char*>(string), (stActualLen=IO_MIN(stActualLen, maxlen)));
    const_cast<char*>(string)[stActualLen] = '\0';
    return result;
}

/**
 * Writes a variable-sized string of bytes to a NetBitStreamInterface 
 * object.
 */
#define IO_VARIABLE_STRING_MAX(string,maxlen) \
    ((IOCONTEXT_WRITE == ctx.type) ? \
        io_variable_string_write_max(bitStream, string, strlen(string), maxlen) \
    : \
        io_variable_string_read_max(bitStream, string, maxlen)) 
/**
 * Declares new functions which are used for both reading and writing.
 * Each function has its own context (read, write).
 *
 * Also constructs a const context so that all IO_* macros may be used.
 *
 * Note that we will be basically duplicating code here.
 */
#define IO_DECLARE_FUNCTION(class_name,code) \
    bool class_name::Write(NetServerBitStreamInterface& bitStream) const\
    { \
        IOCONTEXT ctx = { IOCONTEXT_WRITE, #class_name }; \
        code \
    } \
    bool class_name::Read(NetServerBitStreamInterface& bitStream) \
    { \
        IOCONTEXT ctx = { IOCONTEXT_READ, #class_name }; \
        code \
    } 


#endif
