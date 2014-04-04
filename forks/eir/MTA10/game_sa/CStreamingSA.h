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

#define MAX_DEFAULT_STREAMING_ENTITIES      2500    // increased to 2500 to match MTA:BLUE behavior

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

#define MAX_STREAMING_REQUESTS  16

// Allocated at ARRAY_StreamerRequest
struct streamingRequest //size: 152 (native)
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

    enum bufferingType : unsigned int
    {
        BUFFERING_IMG,
        BUFFERING_CACHE
    };

    int             ids[MAX_STREAMING_REQUESTS];        // 0
    size_t          bufOffsets[MAX_STREAMING_REQUESTS]; // 64, offset into the thread allocation buffer

    statusType      status;                             // 128
    int             statusCode;                         // 132
    unsigned int    offset;                             // 136, IMG archive offset descriptor
    unsigned int    blockCount;                         // 140
    unsigned int    count;                              // 144
    unsigned int    returnCode;                         // 148

    // These members do not exist in native GTA:SA, but in MTA.
    CFiberSA*       loaderFiber;

    // Ability to choose buffering behavior (from .img, from cache, etc)
    void*           loaderBuffer;
    bufferingType   bufBehavior;
};

namespace Streaming
{
    // Streaming uses a sorted entity activity list.
    // Rockstar did not include any sorting though.
    // * Yes, templates are hard to deduce from the assembly code,
    // but you see it is possible.
    struct streamingChainInfo
    {
        void InitFirst( void )
        {
        }
        
        void InitLast( void )
        {
        }

        bool operator < ( const streamingChainInfo& right )
        {
            // There is nothing to sort.
            return false;
        }

        class CEntitySAInterface *entity;

        inline void Execute( void )
        { }
    };
    typedef CRenderChainInterface <streamingChainInfo> streamingEntityChain_t;
    typedef streamingEntityChain_t::renderChain streamingEntityReference_t;

    extern streamingEntityChain_t gcEntityChain;

    inline streamingEntityChain_t&  GetStreamingEntityChain( void )
    {
        return gcEntityChain;
    }

    // Public functions
    void __cdecl RequestModel( modelId_t id, unsigned int flags );
    void __cdecl FreeModel( modelId_t id );
    void __cdecl RequestDirectResource( modelId_t model, unsigned int blockOffset, unsigned int blockCount, unsigned int imgId, unsigned int reqFlags );
    void __cdecl RequestSpecialModel( modelId_t model, const char *tex, unsigned int channel );
    void __cdecl CleanUpLoadQueue( void );
    void __cdecl Update( void );
    void __cdecl FlushRequestList( void );

    // MTA extensions.
    bool    IsInsideStreamingUpdate( void );

    // General MTA management.
    void    Reset( void );
};

class CStreamingSA : public CStreaming
{
public:
                    CStreamingSA                    ( void );
                    ~CStreamingSA                   ( void );

    // Resource management exports.
    void            RequestModel                    ( modelId_t id, unsigned int flags );
    void            FreeModel                       ( modelId_t id );
    void            LoadAllRequestedModels          ( bool onlyPriority = false, const char *debugLoc = NULL );
    bool            HasModelLoaded                  ( modelId_t id );
    bool            IsModelLoading                  ( modelId_t id );
    void            WaitForModel                    ( modelId_t id );
    void            RequestAnimations               ( int idx, unsigned int flags );
    bool            HaveAnimationsLoaded            ( int idx );
    void            RequestVehicleUpgrade           ( modelId_t model, unsigned int flags );
    bool            HasVehicleUpgradeLoaded         ( int model );
    void            RequestSpecialModel             ( modelId_t model, const char *tex, unsigned int channel );

    void            CacheIMGFile                    ( const char *name );
    bool            IsIMGFileCached                 ( const char *name ) const;
    void            FreeIMGFileCache                ( const char *name );

    // Utility methods.
    unsigned int    GetActiveStreamingEntityCount   ( void ) const;
    unsigned int    GetFreeStreamingEntitySlotCount ( void ) const;
    bool            IsEntityGCManaged               ( CEntity *entity ) const;

    entityList_t    GetActiveStreamingEntities      ( void );

    CEntitySA*      GetStreamingFocusEntity         ( void );

    void            SetWorldStreamingEnabled        ( bool enabled );
    bool            IsWorldStreamingEnabled         ( void ) const;

    void            SetInfiniteStreamingEnabled     ( bool enabled );
    bool            IsInfiniteStreamingEnabled      ( void ) const;
    void            SetStrictNodeDistribution       ( bool enabled );
    bool            IsStrictNodeDistributionEnabled ( void ) const;
    void            SetGarbageCollectOnDemand       ( bool enabled );
    bool            IsGarbageCollectOnDemandEnabled ( void ) const;
    void            SetStreamingNodeStealingAllowed ( bool enabled );
    bool            IsStreamingNodeStealingAllowed  ( void ) const;

    void            EnableFiberedLoading            ( bool enable );
    bool            IsFiberedLoadingEnabled         ( void ) const;

    void            SetFiberedPerfMultiplier        ( double mult );
    double          GetFiberedPerfMultiplier        ( void ) const;

    void            GetStreamingInfo                ( streamingInfo& info ) const;

    bool            IsInsideLoadAllRequestedModels  ( void ) const;

    // Useful resource system event callbacks.
    void            SetRequestCallback              ( streamingRequestCallback_t callback );
    void            SetLoadCallback                 ( streamingLoadCallback_t callback );
    void            SetFreeCallback                 ( streamingFreeCallback_t callback );
};

#include "CStreamingSA.init.h"
#include "CStreamingSA.utils.h"
#include "CStreamingSA.runtime.h"
#include "CStreamingSA.sectorize.h"
#include "CStreamingSA.collision.h"
#include "CStreamingSA.clump.h"
#include "CStreamingSA.textures.h"
#include "CStreamingSA.ipl.h"
#include "CStreamingSA.loader.h"
#include "CStreamingSA.cache.h"
#include "CStreamingSA.gc.h"

#ifdef _MTA_BLUE

#include "CStreamingSA.iplfixes.h"

#endif //_MTA_BLUE

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

extern class CRwObjectSA *g_replObjectNative[DATA_TEXTURE_BLOCK];

#endif