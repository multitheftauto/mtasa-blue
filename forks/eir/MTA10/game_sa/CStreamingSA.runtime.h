/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.runtime.h
*  PURPOSE:     GTA:SA internal FileSystem streaming interface
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_INTERNAL_
#define _STREAMING_INTERNAL_

// Hardcoded limit is 256 (due to stream index being stored in last byte of offset descriptor)
#define MAX_GTA_STREAM_HANDLES  128

namespace Streaming
{
    inline unsigned int     GetFileHandle( unsigned int fileId, unsigned int fileOffset )     { return ( fileId << 24 ) | fileOffset; }

    // Streaming resource loader API
    unsigned int __cdecl    OpenStream                  ( const char *path );
    int __cdecl             ReadStream                  ( unsigned int idx, void *buffer, unsigned int offset, unsigned int blockCount );
    unsigned int __cdecl    GetStreamNextReadOffset     ( void );
    unsigned int __cdecl    GetSyncSemaphoreStatus      ( unsigned int idx );
    unsigned int __cdecl    CancelSyncSemaphore         ( unsigned int idx );
    void __cdecl            CancelAllStreaming          ( void );

    void __cdecl            Init                        ( unsigned int numSync );
    void __cdecl            Terminate                   ( void );
};

void StreamingRuntime_Init( void );
void StreamingRuntime_Shutdown( void );

typedef char streamName[64];

// Global variables of all opened streaming files.
// "streaming" == asynchronous data/resource loading
extern HANDLE gtaStreamHandles[MAX_GTA_STREAM_HANDLES];
extern streamName gtaStreamNames[MAX_GTA_STREAM_HANDLES];

#endif //_STREAMING_INTERNAL_