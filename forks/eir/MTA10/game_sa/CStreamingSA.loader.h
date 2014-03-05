/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.loader.h
*  PURPOSE:     Main routines for resource acquisition
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_LOADER_
#define _STREAMING_LOADER_

// This is the global constant that sets the amount of slicers the Streaming system should use.
// It can be set to any arbitrary value. 2 is the amount Rockstar Games has chosen.
// I have no idea what amount gives the best performance.
#define MAX_STREAMING_REQUESTERS            3

#define STREAMING_DEFAULT_FIBERED_LOADING   ( true )
#define STREAMING_DEFAULT_FIBERED_PERFMULT  ( 0.6f )

// Used by streaming
bool __cdecl CheckTXDDependency( modelId_t id );
bool __cdecl CheckAnimDependency( modelId_t id );
bool __cdecl LoadModel( void *buf, modelId_t id, unsigned int threadId );

namespace Streaming
{
    // Important globals.
    extern bool isLoadingBigModel;
    extern unsigned int numPriorityRequests;
    extern void* threadAllocationBuffers[];
    extern bool enableFiberedLoading;

    extern volatile bool insideLoadAllRequestedModels;
    extern unsigned int activeStreamingThread;

    // MTA extension: global that contains all execution environments of the
    // MTA:Eir Streaming system. Can be NULL if the fibered loader is not
    // used/active.
    extern CExecutiveGroupSA *fiberGroup;

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
    // streamingRequest contains model ids which request data through
    // the synSemaphores.
    // Note: since we have localized the streaming system, we can change
    // this structure.
    extern streamingRequest resourceRequesters[];

    inline streamingRequest&    GetStreamingRequest( unsigned int id )
    {
        return resourceRequesters[id];
    }

    // Returns the syncSemaphore index associated with the streaming slicer.
    // In native GTA:SA, slicer index == syncSemaphore index
    inline unsigned int GetStreamingRequestSyncSemaphoreIndex( unsigned int id )
    {
        return id + 32;
    }

    // MTA extension functions.
    void EnterFiberMode( void );
    void LeaveFiberMode( void );

    void EnableFiberedLoading( bool enable );
    bool IsFiberedLoadingEnabled( void );

    void SetLoaderPerfMultiplier( double multiplier );
    double GetLoaderPerfMultiplier( void );

    // Loader routines.
    void __cdecl LoadAllRequestedModels( bool onlyPriority );
    void __cdecl PulseLoader( void );
};

// Streaming loader exports.
modelId_t __cdecl ProcessLoadQueue( unsigned int offset, bool favorPriority );
bool __cdecl ProcessStreamingRequest( unsigned int id );
void __cdecl PulseStreamingRequests( void );
void __cdecl PulseStreamingRequest( unsigned int reqId );
void __cdecl PulseStreamingLoader( void );

// Module Initialization.
void StreamingResources_Init( void );
void StreamingResources_Shutdown( void );

#endif //_STREAMING_LOADER_