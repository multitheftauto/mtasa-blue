/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.h
*  PURPOSE:     Header file for data streamer class
*  DEVELOPERS:  Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CStreamingSA_H
#define __CStreamingSA_H

#include <game/CStreaming.h>
#include "Common.h"

#define DATA_TEXTURE_BLOCK      20000
#define DATA_COLL_BLOCK         25000
#define DATA_IPL_BLOCK          25255
#define DATA_PATHFIND_BLOCK     25511
#define DATA_ANIM_BLOCK         25575
#define DATA_RECORD_BLOCK       25755

#define FUNC_CStreaming__RequestModel                       0x4087E0
#define FUNC_LoadAllRequestedModels                         0x40EA10

#define FUNC_CStreaming__HasVehicleUpgradeLoaded            0x407820
#define FUNC_RequestVehicleUpgrade                          0x408C70

#define FUNC_CStreaming_RequestAnimations                   0x407120
#define FUNC_CStreaming_RequestSpecialModel                 0x409d10

#define MAX_RESOURCES                                       26300
#define MAX_MODELS                                          20000

#define ARRAY_StreamerRequest   0x008E4A60
#define MAX_STREAMING_REQUESTS  16

// Allocated at ARRAY_StreamerRequest
struct streamingRequest //size: 152
{
public:
    enum statusType : unsigned int
    {
        STREAMING_NONE,
        STREAMING_BUFFERING,
        STREAMING_LOADING,
        STREAMING_WAITING,
        STREAMING_READY
    };

    int             ids[MAX_STREAMING_REQUESTS];        // 0
    size_t          bufOffsets[MAX_STREAMING_REQUESTS]; // 64, offset into the thread allocation buffer

    statusType      status;                             // 128
    int             statusCode;                         // 132
    unsigned int    offset;                             // 136, IMG archive offset descriptor
    unsigned int    blockCount;                         // 140
    unsigned int    count;                              // 144
    unsigned int    returnCode;                         // 148
};

namespace Streaming
{
    // Allocated dynamically in the streaming initialization
    struct syncSemaphore    //size: 48
    {
        unsigned int    blockOffset;            // 0
        unsigned int    blockCount;             // 4
        void*           buffer;                 // 8
        BYTE            pad;                    // 12
        bool            terminating;            // 13
        bool            threadActive;           // 14, true if the streaming thread is working on this
        BYTE            pad2;                   // 15
        unsigned int    resultCode;             // 16
        HANDLE          semaphore;              // 20
        HANDLE          file;                   // 24
        OVERLAPPED      overlapped;             // 28
    };

    // There is a maximum of 2 streaming requests internally in the engine.
    // Those slots are parallel to the maximum syncSemaphores.
    // streamingRequest contains model ids which request data through the
    // IMG resource loader. It is like a region of memory requested from
    // the file of specific size. All of the resources inside this region
    // can be requested very quickly. That is instead of requesting every
    // resource individually (because HDDs/disk drives are slow).
    inline streamingRequest&    GetStreamingRequest( unsigned int id )
    {
        if ( id > 2 )
            __asm int 3

        return *( (streamingRequest*)ARRAY_StreamerRequest + id );
    }

    // Public functions
    void __cdecl RequestModel( modelId_t id, unsigned int flags );
    void __cdecl FreeModel( modelId_t id );
    void __cdecl LoadAllRequestedModels( bool onlyPriority );
};

class CStreamingSA : public CStreaming
{
public:
                    CStreamingSA                ( void );
                    ~CStreamingSA               ( void );

    void            RequestModel                ( modelId_t id, unsigned int flags );
    void            FreeModel                   ( modelId_t id );
    void            LoadAllRequestedModels      ( bool onlyPriority = false, const char *debugLoc = NULL );
    bool            HasModelLoaded              ( modelId_t id );
    bool            IsModelLoading              ( modelId_t id );
    void            WaitForModel                ( modelId_t id );
    void            RequestAnimations           ( int idx, unsigned int flags );
    bool            HaveAnimationsLoaded        ( int idx );
    void            RequestVehicleUpgrade       ( modelId_t model, unsigned int flags );
    bool            HasVehicleUpgradeLoaded     ( int model );
    void            RequestSpecialModel         ( modelId_t model, const char *tex, unsigned int channel );

    void            SetRequestCallback          ( streamingRequestCallback_t callback );
    void            SetLoadCallback             ( streamingLoadCallback_t callback );
    void            SetFreeCallback             ( streamingFreeCallback_t callback );
};

#include "CStreamingSA.init.h"
#include "CStreamingSA.utils.h"
#include "CStreamingSA.runtime.h"

// Internal class used to store model indices in
// Somewhat deprecated type.
class ModelIdContainer
{
public:
    static const unsigned int max = 23;

    // Binary offsets: (1.0 US and 1.0 EU): 0x00611B90
    ModelIdContainer( void )
    {
        memset( ids, 0xFF, sizeof( ids ) );
    }

    // Binary offsets: (1.0 US and 1.0 EU): 0x00611BB0
    void Append( unsigned short item )
    {
        for ( unsigned int n = 0; n < max; n++ )
        {
            unsigned short id = ids[n];

            if ( id == 0xFFFF )
            {
                ids[n] = item;
                return;
            }
        }
    }

    // Binary offsets: (1.0 US and 1.0 EU): 0x00611BD0
    void Remove( unsigned short item )
    {
        unsigned int n = 0;

        for ( ; n < max; n++ )
        {
            unsigned short id = ids[n];

            if ( id == item )
                goto found;
        }

        return;

found:
        for ( ; n < max - 1; n++ )
            ids[n] = ids[n + 1];

        ids[max - 1] = 0xFFFF;
    }

    // Binary offsets: (1.0 US and 1.0 EU): 0x00611C20
    unsigned short GetValue( unsigned int idx )
    {
        assert( idx < max );

        return ids[idx];
    }

    unsigned int Find( unsigned short item )
    {
        for ( unsigned int n = 0; n < max; n++ )
        {
            if ( ids[n] == item )
                return n;
        }

        return max;
    }

    // Binary offsets: (1.0 US and 1.0 EU): 0x00611C30
    unsigned int Count( void )
    {
        return Find( 0 );
    }

private:
    unsigned short ids[max];
};

// Important loader flags
#define FLAG_PRIORITY           0x10
#define FLAG_NODEPENDENCY       0x0E

#ifdef RENDERWARE_VIRTUAL_INTERFACES
extern class CRwObjectSA *g_replObjectNative[DATA_TEXTURE_BLOCK];
#endif //RENDERWARE_VIRTUAL_INTERFACES
extern class CColModelSA *g_colReplacement[DATA_TEXTURE_BLOCK];
extern class CColModelSAInterface *g_originalCollision[DATA_TEXTURE_BLOCK];

#endif