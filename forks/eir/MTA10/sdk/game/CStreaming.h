/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CStreaming.h
*  PURPOSE:     Game streaming interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CStreaming_H
#define __CStreaming_H

#define PLAYER_IMG_FILENAME     "models\\player.img"

// Set the default model id type
// This should be the most efficient way to travel model ids.
// It can either be unsigned int or unsigned short.
// To provide compatibility with GTA:SA, we use unsigned int.
// Some say (http://stackoverflow.com/questions/5069489/performance-of-built-in-types-char-vs-short-vs-int-vs-float-vs-double)
// int were faster than short. Okay.
// The rule of thumb is: use "unsigned short" for storing model ids (cache efficiency), use "modelId_t" for model ids on stack.
typedef unsigned int modelId_t;

typedef void (__cdecl*streamingRequestCallback_t)( modelId_t id );
typedef void (__cdecl*streamingLoadCallback_t)( modelId_t id );
typedef void (__cdecl*streamingFreeCallback_t)( modelId_t id );

class CStreaming
{
public:
    typedef std::list <CEntity*> entityList_t;  // select a fast container here.

    struct streamingInfo
    {
        unsigned int usedMemory;
        unsigned int maxMemory;
        unsigned int numberOfRequests;
        unsigned int numberOfPriorityRequests;
        unsigned int numberOfSlicers;
        unsigned int numberOfRequestsPerSlicer;
        unsigned int activeStreamingThread;
        bool isBusy;
        bool isLoadingBigModel;
    };

    // Resource management exports.
    virtual void            RequestModel                    ( modelId_t id, unsigned int flags ) = 0;
    virtual void            FreeModel                       ( modelId_t id ) = 0;
    virtual void            LoadAllRequestedModels          ( bool onlyPriority = false, const char *debugLoc = NULL ) = 0;
    virtual bool            HasModelLoaded                  ( modelId_t id ) = 0;
    virtual bool            IsModelLoading                  ( modelId_t id ) = 0;
    virtual void            WaitForModel                    ( modelId_t id ) = 0;
    virtual void            RequestAnimations               ( int idx, unsigned int flags ) = 0;
    virtual bool            HaveAnimationsLoaded            ( int idx ) = 0;
    virtual bool            HasVehicleUpgradeLoaded         ( int model ) = 0;
    virtual void            RequestSpecialModel             ( modelId_t model, const char *tex, unsigned int channel ) = 0;

    virtual void            CacheIMGFile                    ( const char *name ) = 0;
    virtual bool            IsIMGFileCached                 ( const char *name ) const = 0;
    virtual void            FreeIMGFileCache                ( const char *name ) = 0;

    // Utility methods.
    virtual unsigned int    GetActiveStreamingEntityCount   ( void ) const = 0;
    virtual unsigned int    GetFreeStreamingEntitySlotCount ( void ) const = 0;
    virtual bool            IsEntityGCManaged               ( CEntity *entity ) const = 0;

    virtual entityList_t    GetActiveStreamingEntities      ( void ) = 0;

    virtual CEntity*        GetStreamingFocusEntity         ( void ) = 0;

    virtual void            SetWorldStreamingEnabled        ( bool enabled ) = 0;
    virtual bool            IsWorldStreamingEnabled         ( void ) const = 0;

    virtual void            SetInfiniteStreamingEnabled     ( bool enabled ) = 0;
    virtual bool            IsInfiniteStreamingEnabled      ( void ) const = 0;
    virtual void            SetStrictNodeDistribution       ( bool enabled ) = 0;
    virtual bool            IsStrictNodeDistributionEnabled ( void ) const = 0;
    virtual void            SetGarbageCollectOnDemand       ( bool enabled ) = 0;
    virtual bool            IsGarbageCollectOnDemandEnabled ( void ) const = 0;
    virtual void            SetStreamingNodeStealingAllowed ( bool enabled ) = 0;
    virtual bool            IsStreamingNodeStealingAllowed  ( void ) const = 0;

    virtual void            EnableFiberedLoading            ( bool enable ) = 0;
    virtual bool            IsFiberedLoadingEnabled         ( void ) const = 0;

    virtual void            SetFiberedPerfMultiplier        ( double mult ) = 0;
    virtual double          GetFiberedPerfMultiplier        ( void ) const = 0;

    virtual void            GetStreamingInfo                ( streamingInfo& info ) const = 0;

    virtual bool            IsInsideLoadAllRequestedModels  ( void ) const = 0;

    // Callbacks from the system.
    virtual void            SetRequestCallback              ( streamingRequestCallback_t callback ) = 0;
    virtual void            SetLoadCallback                 ( streamingLoadCallback_t callback ) = 0;
    virtual void            SetFreeCallback                 ( streamingFreeCallback_t callback ) = 0;
};

#endif //__CStreaming_H
