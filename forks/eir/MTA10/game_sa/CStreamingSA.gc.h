/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.gc.h
*  PURPOSE:     Streaming garbage collection logic
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_GC_
#define _STREAMING_GC_

namespace Streaming
{
    // Globals used by the GC manager.
    extern bool allowInfiniteStreaming;
    extern bool strictNodeDistribution;
    extern bool garbageCollectOnDemand;
    extern bool allowStreamingNodeStealing;

    void            ResetGarbageCollection( void );

    // Active entity management.
    streamingEntityReference_t* __cdecl AddActiveEntity( CEntitySAInterface *entity );
    void __cdecl                        RemoveActiveEntity( streamingEntityReference_t *ref );

    void __cdecl    InitRecentGCNode( void );
    void __cdecl    SetRecentGCNode( streamingEntityReference_t *node );
};

// Garbage collection exports.
void __cdecl UnclogMemoryUsage( size_t mem_size );

// Module initialization.
void StreamingGC_Init( void );
void StreamingGC_Shutdown( void );

#endif //_STREAMING_GC_