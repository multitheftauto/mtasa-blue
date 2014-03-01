/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RwStream.h
*  PURPOSE:     RenderWare RwStream definition
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_STREAM_
#define _RENDERWARE_STREAM_

/*****************************************************************************/
/** RenderWare I/O                                                          **/
/*****************************************************************************/

// RenderWare type definitions
typedef int             (__cdecl*RwIOCallbackClose) (void *data);
typedef size_t          (__cdecl*RwIOCallbackRead)  (void *data, void *buffer, size_t length);
typedef size_t          (__cdecl*RwIOCallbackWrite) (void *data, const void *buffer, size_t length);
typedef void*           (__cdecl*RwIOCallbackSeek)  (void *data, unsigned int offset);

#define RW_STREAM_WRITEBUF_SIZE     0x200

// RenderWare enumerations
enum RwStreamType : unsigned int
{
    STREAM_TYPE_NULL = 0,
    STREAM_TYPE_FILE = 1,       // this is practically undefined. nowhere it says that it is a file.
    STREAM_TYPE_FILENAME = 2,
    STREAM_TYPE_BUFFER = 3,
    STREAM_TYPE_CALLBACK = 4
};
enum RwStreamMode : unsigned int
{
    STREAM_MODE_NULL = 0,
    STREAM_MODE_READ = 1,
    STREAM_MODE_WRITE = 2,
    STREAM_MODE_APPEND = 3
};

// RenderWare base types
struct RwBuffer
{
    void*           ptr;
    unsigned int    size;
};
struct RwBufferedStream
{
    unsigned int        position;
    unsigned int        size;
    void*               dataPtr;
};
union RwStreamTypeData
{
    RwBufferedStream        buffered;
    struct
    {
        void*               file;
    };
    struct
    {
        RwIOCallbackClose   callbackClose;
        RwIOCallbackRead    callbackRead;
        RwIOCallbackWrite   callbackWrite;
        RwIOCallbackSeek    callbackSeek;
        void*               ptr_callback;
    };
};
struct RwStream //size: 36
{
    RwStreamType        type;       // 0
    RwStreamMode        mode;       // 4
    int                 pos;        // 8
    RwStreamTypeData    data;       // 12
    int                 allocated;  // 32
};
struct RwBlocksInfo
{
    unsigned short      count;
    unsigned short      unk;
};
struct RwChunkHeader
{
    unsigned int        id;
    size_t              size;
    unsigned int        unk2;
    unsigned int        unk3;
    unsigned int        isComplex;
};

// Buffered stream globals.
inline void             RwStreamBufferedInit        ( RwBufferedStream& data, RwStreamMode mode, const RwBuffer *buf );
inline unsigned int     RwStreamBufferedRead        ( RwBufferedStream& data, void *buf, unsigned int size );
inline unsigned int     RwStreamBufferedWrite       ( RwBufferedStream& data, const void *buf, unsigned int size );
inline bool             RwStreamBufferedSkip        ( RwBufferedStream& data, unsigned int count );
inline bool             RwStreamBufferedSeek        ( RwBufferedStream& data, long seek, int type );
inline bool             RwStreamBufferedShutdown    ( RwBufferedStream& data, unsigned int mode, RwBuffer *buf );

// Function exports.
RwStream* __cdecl       RwStreamInitialize          ( void *memory, int isAllocated, RwStreamType type, RwStreamMode mode, void *ud );
RwStream* __cdecl       RwStreamOpen                ( RwStreamType type, RwStreamMode mode, void *ud );
unsigned int __cdecl    RwStreamReadBlocks          ( RwStream *stream, void *buf, unsigned int size );
RwStream* __cdecl       RwStreamWriteBlocks         ( RwStream *stream, const void *buf, unsigned int size );
RwStream* __cdecl       RwStreamSkip                ( RwStream *stream, unsigned int offset );
RwStream* __cdecl       RwStreamSeek                ( RwStream *stream, long offset, int type );
int __cdecl             RwStreamClose               ( RwStream *stream, void *ud );

#if 0
// Utilities. (todo)
RwStream* __cdecl       RwStreamReadChunkHeaderInfo ( RwStream *stream, RwChunkHeader& header );
int __cdecl             RwStreamFindChunk           ( RwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut );
#endif

// Module initialization.
void                    RwStream_Init               ( void );
void                    RwStream_Shutdown           ( void );

#endif //_RENDERWARE_STREAM_