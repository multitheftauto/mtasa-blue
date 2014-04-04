/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.loader.cpp
*  PURPOSE:     Main routines for resource acquisition
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include <intrin.h>
#include "gamesa_renderware.h"

#include "CStreamingSA.utils.hxx"

static RtDictSchema *const animDict =   (RtDictSchema*)0x008DED50;
static CModelLoadInfoSA *const VAR_ModelLoadInfo = (CModelLoadInfoSA*)0x008E4CC0;

namespace Streaming
{
    streamingLoadCallback_t  streamingLoadCallback = NULL;

    bool isLoadingBigModel = false;                                 // Binary offsets: (1.0 US and 1.0 EU): 0x008E4A58
    streamingRequest resourceRequesters[MAX_STREAMING_REQUESTERS];  // Binary offsets: (1.0 US and 1.0 EU): 0x008E4AA0
    unsigned int numPriorityRequests = 0;                           // Binary offsets: (1.0 US and 1.0 EU): 0x008E4BA0
    void* threadAllocationBuffers[MAX_STREAMING_REQUESTERS];        // Binary offsets: (1.0 US and 1.0 EU): 0x008E4CAC

    // MTA only features.
    bool enableFiberedLoading = STREAMING_DEFAULT_FIBERED_LOADING;  // has to be enabled by API.
    double loaderPerfMultiplier = 0.0f;
};

/*=========================================================
    CheckTXDDependency

    Arguments:
        id - index of the TXD instance to check dependency on resources of
    Purpose:
        Returns whether the given TXD instance is required by any
        model or not. If this function returns false, the resource
        loader should not bother about loading that TXD instance to
        save streaming memory. Thus, this function is a TXD instance
        Garbage Collector check.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00409A90
=========================================================*/
// Generic dependency routine
template <class checkType, class dependType>
inline bool CheckDependency( dependType id )
{
    using namespace Streaming;
    checkType checker;
    
    // Check the queued requests
    {
        CModelLoadInfoSA *item = GetQueuedLoadInfo();
        CModelLoadInfoSA *endItem = *(CModelLoadInfoSA**)0x008E4C54;    // toBeLoadedQueue
        modelId_t itemId = item->GetIndex();

        for ( ; item != endItem; item = &GetModelLoadInfo( itemId = item->m_primaryModel ) )
        {
            // The_GTA: I optimized the id calculation, so it only calculates it once (for the first requested item)
            // An example of how compilers cannot be perfect. Developers should always try to optimize their code.
            // Do not listen to people who want to prevent you from optimizing.
            if ( checker( itemId, id ) )
                return true;
        }
    }

    // Check the streaming requests.
    // The_GTA: I fixed a bug in here which made invalid memory requests if anything else
    // than models or textures were requested in here. I do not know how R* could mess this up.
    // Hard to explain, since their code does not make sense :P
    for ( unsigned char n = 0; n < MAX_STREAMING_REQUESTERS; n++ )
    {
        streamingRequest& requester = Streaming::GetStreamingRequest( n );

        for ( unsigned char i = 0; i < MAX_STREAMING_REQUESTS; i++ )
        {
            modelId_t itemId = requester.ids[i];

            if ( itemId != -1 && checker( itemId, id ) )
                return true;
        }
    }

    // There is no dependency on that instance.
    // Hence we do not require that instance loaded.
    // If we want instances loaded in MTA anyway, it could be interesting to add our
    // own hook for dependency logic here!
    return false;
}

struct ModelTXDDependency
{
    inline bool operator() ( modelId_t model, unsigned short txdId )
    {
        return model < DATA_TEXTURE_BLOCK && (unsigned short)ppModelInfo[model]->usTextureDictionary == txdId ||
              idOffset( model, DATA_TEXTURE_BLOCK ) < MAX_TXD &&
              TextureManager::GetTxdPool()->Get( idOffset( model, DATA_TEXTURE_BLOCK ) )->m_parentTxd == txdId;
    }
};

bool __cdecl CheckTXDDependency( modelId_t id )
{
    return CheckDependency <ModelTXDDependency> ( id );
}

/*=========================================================
    CheckAnimDependency

    Arguments:
        id - index of the TXD instance to check dependency on resources of
    Purpose:
        Returns whether the given animation is required by any
        model or not. If this function returns false, the resource
        loader should not bother about loading it (saves streaming memory).
        Thus, this function is an animation Garbage Collector check.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00407AD0
=========================================================*/
struct ModelAnimDependency
{
    inline bool operator() ( modelId_t model, int animId )
    {
        return model < DATA_TEXTURE_BLOCK && ppModelInfo[model]->GetAnimFileIndex() == animId;
    }
};

bool __cdecl CheckAnimDependency( modelId_t id )
{
    // It is pretty crazy how you can optimize it, right? :P
    // Instead of doubling the code, you can use templates!
    // And, it is very smart to use generic callbacks, too.
    // Read about functors (like ModelAnimDependency), Cazomino05.
    return CheckDependency <ModelAnimDependency> ( id );
}

/*=========================================================
    LoadModel

    Arguments:
        buf - memory buffer which contains the object
        id - index of the model/resource info
        threadId - index of the thread which requested the load (unused)
    Purpose:
        Attempts to load a requested resource into the model info
        at id. It gets executed after a thread loaded the resource
        contents from the IMG file. Returns whether the loading
        was successful.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040C6B0
    Note:
        This function has been updated so it is compatible with
        the MTA model loading system. Collisions are now properly assigned
        to vehicles whose collisions were replaced by MTA.

        SCM script loading has been temporarily disabled, since MTA
        does not use it.
=========================================================*/
struct rwLoadHandler
{
    inline void Initialize( void )
    {
        // Create a stream
        m_stream = RwStreamInitialize( (void*)0x008E48AC, false, STREAM_TYPE_BUFFER, STREAM_MODE_READ, &m_rwbuf );
    }

    inline void Terminate( void )
    {
        // Close the stream.
        RwStreamClose( m_stream, &m_rwbuf );
    }

    inline rwLoadHandler( void *buf, size_t bufSize )
    {
        // Set up the RenderWare buffer
        m_rwbuf.ptr = buf;
        m_rwbuf.size = bufSize;

        Initialize();
    }

    inline ~rwLoadHandler( void )
    {
        Terminate();
    }

    inline RwStream* ResetStream( void )
    {
        Terminate();
        Initialize();

        return m_stream;
    }

    inline RwStream* GetRwStream( void )
    {
        return m_stream;
    }

    inline void* GetBuffer( void )
    {
        return m_rwbuf.ptr;
    }

    inline size_t GetBufferSize( void )
    {
        return m_rwbuf.size;
    }

    RwBuffer m_rwbuf;
    RwStream *m_stream;
};

template <typename loadHandler>
struct corotLoadFlavor
{
    __forceinline corotLoadFlavor( loadHandler& handler ) : m_loadHandler( handler )
    {
    }

    __forceinline bool LoadBaseModel( modelId_t id, CBaseModelInfoSAInterface *info )
    {
        eRwType type = info->GetRwModelType();
        bool success;

        if ( type == RW_ATOMIC )
        {
            RwStream *stream = m_loadHandler.GetRwStream();
            RtDict *dict;

            RwChunkHeader header;
            RwStreamReadChunkHeaderInfo( stream, header );

            // Check if there is animation attached
            if ( header.id == 0x2B )
                animDict->current = dict = RtDictSchemaStreamReadDict( animDict, stream );
            else
                dict = NULL;

            // At this point, GTA_SA utilizes a weird stream logic
            // I have fixed it here (bad clean-up of stream pointers)
            stream = m_loadHandler.ResetStream();

            success = LoadClumpFile( stream, id );

            if ( dict )
                RtDictDestroy( dict );
        }
        else
            success = LoadClumpFilePersistent( m_loadHandler.GetRwStream(), id );

        return success;
    }

    __forceinline bool LoadTexDictionary( modelId_t texId, CTxdInstanceSA *txdInst, CModelLoadInfoSA& loadInfo )
    {
        bool successLoad = txdInst->LoadTXD( m_loadHandler.GetRwStream() );

        if ( successLoad )
            txdInst->InitParent();

        return successLoad;
    }

    __forceinline bool LoadPathFind( modelId_t id )
    {
        PathFind::GetInterface().ReadContainer( m_loadHandler.GetRwStream(), id );
        return true;
    }

    __forceinline bool LoadAnimation( modelId_t id )
    {
        pGame->GetAnimManager()->LoadAnimFile( m_loadHandler.GetRwStream(), true, NULL );
        pGame->GetAnimManager()->CreateAnimAssocGroups();
        return true;
    }

    __forceinline bool LoadRecording( modelId_t id )
    {
        ((void (__cdecl*)( RwStream *stream, modelId_t id, size_t size ))0x0045A8F0)( m_loadHandler.GetRwStream(), id, m_loadHandler.GetBufferSize() );
        return true;
    }

    loadHandler& m_loadHandler;
};

template <typename loadHandler>
struct semiCorotLoadFlavor : corotLoadFlavor <loadHandler>
{
    __forceinline semiCorotLoadFlavor( loadHandler& handler ) : corotLoadFlavor( handler )
    {
    }

    __forceinline bool LoadTexDictionary( modelId_t txdId, CTxdInstanceSA *txdInst, CModelLoadInfoSA& loadInfo )
    {
        bool successLoad;

        if ( Streaming::isLoadingBigModel )
        {
            successLoad = LoadTXDFirstHalf( txdId, m_loadHandler.GetRwStream() );

            if ( !successLoad )
                return false;

            loadInfo.m_eLoading = MODEL_RELOAD;
        }
        else
        {
            successLoad = corotLoadFlavor::LoadTexDictionary( txdId, txdInst, loadInfo );
        }

        return successLoad;
    }
};

template <typename loadHandler, typename loadFlavor>
struct ModelLoadDispatch : public ModelCheckDispatch <false>
{
    __forceinline ModelLoadDispatch( loadHandler& handler, loadFlavor& flavor, CModelLoadInfoSA& loadInfo ) : m_loadHandler( handler ), m_loadFlavor( flavor ), m_loadInfo( loadInfo )
    {
        m_failureRequest = true;
    }

    __forceinline bool DoBaseModel( modelId_t id )
    {
        CBaseModelInfoSAInterface *info = ppModelInfo[id];
        int animIndex = info->GetAnimFileIndex();

        CTxdInstanceSA *txdInst = TextureManager::GetTxdPool()->Get( info->usTextureDictionary );
        CAnimBlockSAInterface *animBlock;

        // Fail if the model texture dictionary is not loaded yet.
        if ( txdInst->m_txd == NULL )
            return false;

        if ( animIndex != 0xFFFFFFFF )
        {
            animBlock = pGame->GetAnimManager()->GetAnimBlock( animIndex );

            if ( !animBlock->IsLoaded() )
                return false;
        }
        else
            animBlock = NULL;

        // Reference the resources
        txdInst->Reference();
        
        // ... and anim block
        if ( animBlock )
            animBlock->Reference();

        txdInst->SetCurrent();

        // Process to the resource loading.
        bool success = m_loadFlavor.LoadBaseModel( id, info );

        if ( m_loadInfo.m_eLoading != MODEL_RELOAD )
        {
            txdInst->DereferenceNoDestroy();

            if ( animBlock )
                animBlock->Dereference();

            if ( !success )
                return false;

            if ( info->GetModelType() == MODEL_VEHICLE )
                success = ((bool (__cdecl*)( unsigned int id ))0x00408000)( id );
        }

        return success;
    }

    __forceinline bool DoTexDictionary( modelId_t txdId )
    {
        CTxdInstanceSA *txdInst = TextureManager::GetTxdPool()->Get( txdId );

        if ( txdInst->m_parentTxd != 0xFFFF )
        {
            if ( !TextureManager::GetTxdPool()->Get( txdInst->m_parentTxd )->m_txd )
                return false;
        }

        // Check whether we depend on this TXD instance (unless we specifically want it without dependency)
        if ( !( m_loadInfo.m_flags & FLAG_NODEPENDENCY ) && !CheckTXDDependency( txdId ) )
        {
            m_failureRequest = false;
            return false;
        }

        return m_loadFlavor.LoadTexDictionary( txdId, txdInst, m_loadInfo );
    }

    __forceinline bool DoCollision( modelId_t id )
    {
        return LoadCOLLibrary( id, (const char*)m_loadHandler.GetBuffer(), m_loadHandler.GetBufferSize() );
    }

    __forceinline bool DoIPL( modelId_t id )
    {
        return Streaming::GetIPLEnvironment().LoadSector( id, (const char*)m_loadHandler.GetBuffer(), m_loadHandler.GetBufferSize() );
    }

    __forceinline bool DoPathFind( modelId_t id )
    {
        return m_loadFlavor.LoadPathFind( id );
    }

    __forceinline bool DoAnimation( modelId_t id )
    {
        if ( m_loadInfo.m_flags & FLAG_NODEPENDENCY || CheckAnimDependency( id ) )
        {
            if ( m_loadInfo.m_blockCount != 0 )
            {
                return m_loadFlavor.LoadAnimation( id );
            }
        }
        
        m_failureRequest = false;
        return false;
    }

    __forceinline bool DoRecording( modelId_t id )
    {
        return m_loadFlavor.LoadRecording( id );
    }

    bool m_failureRequest;
    loadHandler& m_loadHandler;
    loadFlavor& m_loadFlavor;
    CModelLoadInfoSA& m_loadInfo;
};

inline void OnModelLoaded( modelId_t id )
{
    // Do some security checks.
    // You never know what could go wrong!
    if ( id < MAX_MODELS )
    {
        // If this fails, the engine tried to load the original model while it was replaced.
        // This must not happen. It would cause a streaming system memory leak.
        assert( g_replObjectNative[id] == NULL );
    }

    // Grab our load info.
    CModelLoadInfoSA& loadInfo = Streaming::GetModelLoadInfo( id );

    // ... we are done loading!
    loadInfo.m_eLoading = MODEL_LOADED;
    (*(unsigned int*)0x008E4CB4) += loadInfo.GetSize();     // increase the streaming memory statistics

    // Tell modifications (i.e. deathmatch) that we finished loading.
    if ( Streaming::streamingLoadCallback )
        Streaming::streamingLoadCallback( id );
}

struct ModelPostLoadDispatch : public ModelCheckDispatch <true>
{
    __forceinline ModelPostLoadDispatch( CModelLoadInfoSA& loadInfo ) : m_loadInfo( loadInfo )
    { }

    __forceinline bool DoBaseModel( modelId_t id )
    {
        CBaseModelInfoSAInterface *info = ppModelInfo[id];
        eModelType type = info->GetModelType();

        if ( type != MODEL_VEHICLE && type != MODEL_PED )   // Well, there also is weapon model info?
        {
            if ( CAtomicModelInfoSA *atomInfo = info->GetAtomicModelInfo() )
                atomInfo->ucAlpha = ( m_loadInfo.m_flags & 0x24 ) ? 0xFF : 0;

            if ( !( m_loadInfo.m_flags & 0x06 ) )
                m_loadInfo.PushIntoLoader( Streaming::GetLastGarbageCollectModel() );
        }

        return true;
    }

    __forceinline void AddToGarbageCollection( void )
    {
        if ( !( m_loadInfo.m_flags & 0x06 ) )
            m_loadInfo.PushIntoLoader( Streaming::GetLastGarbageCollectModel() );
    }

    __forceinline bool DoCollision( modelId_t id )
    {
        AddToGarbageCollection();
        return true;
    }

    __forceinline bool DoAnimation( modelId_t id )
    {
        AddToGarbageCollection();
        return true;
    }

    __forceinline bool DoScript( modelId_t id )
    {
        AddToGarbageCollection();
        return true;
    }

    CModelLoadInfoSA& m_loadInfo;
};

template <typename loaderDispatch>
inline bool HandleLoaderDispatch( modelId_t id, CModelLoadInfoSA& loadInfo, loaderDispatch& loadDispatch )
{
    bool success = DefaultDispatchExecute( id, loadDispatch );

    if ( !success )
    {
        Streaming::FreeModel( id );

        // It could re-request the resource.
        if ( loadDispatch.m_failureRequest )
            Streaming::RequestModel( id, loadInfo.m_flags );
    }

    return success;
}

bool __cdecl LoadModel( void *buf, modelId_t id, unsigned int threadId )
{
    CModelLoadInfoSA& loadInfo = Streaming::GetModelLoadInfo( id );

    // Do the loading.
    bool success = false;
    {
        rwLoadHandler lHandler( buf, loadInfo.GetSize() );  // we are loading from .IMG chunks.

        typedef semiCorotLoadFlavor <rwLoadHandler> loadTechnique;

        ModelLoadDispatch <rwLoadHandler, loadTechnique> loadDispatch( lHandler, loadTechnique( lHandler ), loadInfo );

        success = HandleLoaderDispatch( id, loadInfo, loadDispatch );
    }

    if ( success )
    {
        // Do post loading handling
        DefaultDispatchExecute( id, ModelPostLoadDispatch( loadInfo ) );

        // If we do not require a second loader run...
        if ( loadInfo.m_eLoading != MODEL_RELOAD )
            OnModelLoaded( id );
    }

    return true;
}

/*=========================================================
    CompleteStreamingRequest

    Arguments:
        id - index of the streamingRequest
    Purpose:
        Finishes all data requesting of the specified streamingRequest.
        After that the streamingRequest may be ready for loading.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004076C0
=========================================================*/
modelId_t Streaming::streamingWaitModel = -1;       // Binary offsets: (1.0 US and 1.0 EU): 0x008E4B90

__forceinline bool FinishResourceLoading( unsigned int idx )
{
    if ( ProcessStreamingRequest( idx ) )
    {
        while ( Streaming::GetStreamingRequest( idx ).status == streamingRequest::STREAMING_LOADING )
            ProcessStreamingRequest( idx );

        return true;
    }
    return false;
}

void __cdecl CompleteStreamingRequest( unsigned int idx )
{
    using namespace Streaming;

    streamingRequest& requester = GetStreamingRequest( idx );

    while ( streamingWaitModel != -1 )
    {
        streamingRequest::statusType status = requester.status;

        if ( status != streamingRequest::STREAMING_NONE )
        {
            if ( status == streamingRequest::STREAMING_BUFFERING )
            {
                // Have we successfully processed the request?
                if ( FinishResourceLoading( idx ) )
                {
                    // We no longer wait for a resource to load
                    streamingWaitModel = -1;
                    break;
                }
                
                continue;
            }
            else if ( status == streamingRequest::STREAMING_LOADING )
                continue;
            else
            {
                requester.count++;

                unsigned int syncStatus = GetSyncSemaphoreStatus( Streaming::GetStreamingRequestSyncSemaphoreIndex( idx ) );

                if ( syncStatus == 0xFF || syncStatus == 0xFA )
                    continue;
            }

            // Perform the read request
            ReadStream( Streaming::GetStreamingRequestSyncSemaphoreIndex( idx ), Streaming::threadAllocationBuffers[idx], requester.offset, requester.blockCount );

            // Change our status
            requester.status = streamingRequest::STREAMING_BUFFERING;
            requester.statusCode = -0xFF;
        }
    }

    // Call some random updater function
    // This function was prolly used by the loading bar in the past.
    ((void (__cdecl*)( void ))0x00590320)();
}

/*=========================================================
    LoadBigModel

    Arguments:
        buf - memory buffer which contains the resource
        id - model info index
    Purpose:
        Loads a big model resource and inserts it into the model
        info described by id. Returns whether the loading process
        was successful.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00408CB0
    Note:
        Currently, this function knows it will complete the
        loading of the resource it is requested to finish.
        This means that by calling this function in
        ProcessStreamingRequest, isLoadingBigModel will be
        set to false as well as all of its ids (except 0) will
        be cleared.
=========================================================*/
bool __cdecl LoadBigModel( char *buf, modelId_t id )
{
    using namespace Streaming;

    CModelLoadInfoSA *loadInfo = &GetModelLoadInfo( id );

    // Check whether we are ready to load in the first place.
    if ( loadInfo->m_eLoading != MODEL_RELOAD )
    {
        if ( id < MAX_MODELS )
            ppModelInfo[id]->Dereference();

        return false;
    }

    // Set up the RenderWare stream
    RwBuffer rwbuf;
    rwbuf.ptr = buf;
    rwbuf.size = loadInfo->m_blockCount * 2048;

    RwStream *stream = RwStreamInitialize( (void*)0x008E48AC, false, STREAM_TYPE_BUFFER, STREAM_MODE_READ, &rwbuf );

    bool success;

    // Dispatch by request type.
    if ( id < DATA_TEXTURE_BLOCK )
    {
        // Actually, this is unused; so screw it :P
        success = false;
    }
    else if ( id < DATA_TEXTURE_BLOCK + MAX_TXD )
    {
        CTxdInstanceSA *txdInst = TextureManager::GetTxdPool()->Get( idOffset( id, DATA_TEXTURE_BLOCK ) );

        // Reference it during the loading
        txdInst->Reference();

        success = LoadTXDContinue( idOffset( id, DATA_TEXTURE_BLOCK ), stream );

        // Remove the reference again. Let the garbage collector unload it if necessary.
        txdInst->DereferenceNoDestroy();
    }
    else
        success = true; // succeeds by default

    RwStreamClose( stream, &rwbuf );

    // Increase the actual streaming memory usage
    *(unsigned int*)0x008E4CB4 += rwbuf.size;

    // Big model has been successfully loaded?
    loadInfo->m_eLoading = MODEL_LOADED;

    // Check whether the loading was actually successful.
    if ( !success )
    {
        // We failed. Try again!
        FreeModel( id );
        RequestModel( id, loadInfo->m_flags );
    }
    // Notify our environment that we loaded another model.
    else if ( streamingLoadCallback )
        streamingLoadCallback( id );

    return success;
}

/*=========================================================
    ProcessStreamingRequest

    Arguments:
        id - index of the streamingRequest
    Purpose:
        Handles a streaming request table and attempts to load
        associated models while freeing others to fit the
        maximum streaming memory. This function is the core
        utility of the GTA:SA model streaming management.
        Returns whether processing was successful. It fails if
        the loading of the syncSemaphore is still pending.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040E170
    Note:
        The original GTA:SA function is a mess and an example of
        how GTA:SA is not a great engine. The GTA community figured
        about this anyway, so that should be proof.
=========================================================*/
struct ModelUnclogDispatch : public ModelCheckDispatch <true>
{
    __forceinline void UnclogMemory( modelId_t id )
    {
        UnclogMemoryUsage( Streaming::GetModelLoadInfo( id ).GetSize() );
    }

    __forceinline bool DoBaseModel( modelId_t id )
    {
        UnclogMemory( id );
        return true;
    }

    __forceinline bool DoCollision( modelId_t id )
    {
        UnclogMemory( id );
        return true;
    }

    // Not IPL sector request.

    __forceinline bool DoPathFind( modelId_t id )
    {
        UnclogMemory( id );
        return true;
    }

    __forceinline bool DoAnimation( modelId_t id )
    {
        UnclogMemory( id );
        return true;
    }

    __forceinline bool DoRecording( modelId_t id )
    {
        UnclogMemory( id );
        return true;
    }

    __forceinline bool DoScript( modelId_t id )
    {
        UnclogMemory( id );
        return true;
    }

    __forceinline bool DoOther( modelId_t id )
    {
        UnclogMemory( id );
        return true;
    }
};

struct semiCorotPulseManager
{
    __forceinline bool DoLoading( void *buf, modelId_t mid, unsigned int slicerIndex )
    {
        return LoadModel( buf, mid, slicerIndex );
    }

    inline bool IsFibered( void )
    {
        return false;
    }
};

template <typename pulseManager>
__forceinline void ProcessSlicerInstances( unsigned int slicerIndex, pulseManager& pulseMan )
{
    streamingRequest& requester = Streaming::GetStreamingRequest( slicerIndex );
    bool loadFibered = pulseMan.IsFibered();

    for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTS; n++ )
    {
        modelId_t mid = requester.ids[n];

        if ( mid != -1 )
        {
            CModelLoadInfoSA& loadInfo = Streaming::GetModelLoadInfo( mid );

            // I removed the broken vehicle stream limit from this function.
            // That stream.ini "vehicles" feature was never properly implemented into GTA:SA 1.0.
            // 1) It was used for big vehicle models, which were never loaded using the appropriate
            // API in the first place.
            // 2) Keeping that Rockstar code hinders MTA logic, as MTA logic expects no limits.
            //  * location: 0x0040E1F1
            DefaultDispatchExecute( mid, ModelUnclogDispatch() );

            // Perform the loading
            {
                // MTA extension: we use a destination buffer inside of the requester here
                // instead of the global threading buffer. This way we can easily implement
                // loading from cached data.
                void *buf = (char*)requester.loaderBuffer + requester.bufOffsets[n] * 2048;

                pulseMan.DoLoading( buf, mid, slicerIndex );
            }

            bool resetSlot = true;

            if ( !loadFibered )
            {
                // Do we have to continue loading this model?
                bool continueLoading = ( loadInfo.m_eLoading == MODEL_RELOAD );

                resetSlot = !continueLoading;
                
                // MTA extension: If fibered-loading is enabled, we can load the resources at any
                // index of the slicer.
                unsigned int loadAtIndex = 0;

                if ( continueLoading )
                {
                    // The_GTA: this appears to simulate coroutine behaviour.
                    // To improve the performance of the whole system, I propose a CExecutiveManagerSA
                    // class which will host fibers using frame pulses. It will be adapted from
                    // Lua coroutines, but will have multiple ways of yielding (time, count, etc).
                    // It will ultimatively remove lag-spikes due to resource loading.
                    requester.status = streamingRequest::STREAMING_LOADING;

                    if ( n != loadAtIndex )
                    {
                        // Reposition the request.
                        requester.bufOffsets[loadAtIndex] = requester.bufOffsets[n];
                        requester.ids[loadAtIndex] = mid;
                    }
                }

                if ( n != loadAtIndex )
                    resetSlot = true;
            }

            // Only clear if not zero (because 0 is slot of big model request?)
            if ( resetSlot )
                requester.ids[n] = 0xFFFF;  // clear the request slot
        }
    }
}

inline void YieldStreamingFiber( CFiberSA *fiber )
{
    if ( !Streaming::insideLoadAllRequestedModels )
        fiber->yield_proc();
}

struct corotRwLoadHandler
{
    struct corotStreamData
    {
        RwBufferedStream data;
        RwBuffer buf;
        CFiberSA *fiber;
    };

    static int streamClose( void *fp )
    {
        corotStreamData *data = (corotStreamData*)fp;

        RwStreamBufferedShutdown( data->data, STREAM_MODE_READ, NULL );
        return true;
    }

    static size_t streamRead( void *fp, void *buffer, size_t length )
    {
        corotStreamData *data = (corotStreamData*)fp;

        YieldStreamingFiber( data->fiber );

        return RwStreamBufferedRead( data->data, buffer, length );
    }

    static size_t streamWrite( void *fp, const void *buffer, size_t length )
    {
        corotStreamData *data = (corotStreamData*)fp;

        YieldStreamingFiber( data->fiber );

        return RwStreamBufferedWrite( data->data, buffer, length );
    }

    static void* streamSkip( void *fp, unsigned int offset )
    {
        corotStreamData *data = (corotStreamData*)fp;

        YieldStreamingFiber( data->fiber );

        bool success = RwStreamBufferedSkip( data->data, offset );

        return ( success ) ? fp : NULL;
    }

    char _streamBuf[sizeof(RwStream)];

    corotStreamData streamData;

    inline void Initialize( void )
    {
        // Create a stream
        RwStreamTypeData typeData;
        typeData.callbackClose = streamClose;
        typeData.callbackRead = streamRead;
        typeData.callbackWrite = streamWrite;
        typeData.callbackSeek = streamSkip;
        typeData.ptr_callback = &streamData;

        // Initialize the buffered stream.
        RwStreamBufferedInit( streamData.data, STREAM_MODE_READ, &streamData.buf );

        m_stream = RwStreamInitialize( _streamBuf, false, STREAM_TYPE_CALLBACK, STREAM_MODE_READ, &typeData );
    }

    inline void Terminate( void )
    {
        // Close the stream.
        RwStreamClose( m_stream, &streamData );
    }

    inline corotRwLoadHandler( void *buf, size_t bufSize, CFiberSA *fiber )
    {
        // Set up the RenderWare buffer
        streamData.buf.ptr = buf;
        streamData.buf.size = bufSize;
        streamData.fiber = fiber;

        Initialize();
    }

    inline ~corotRwLoadHandler( void )
    {
        Terminate();
    }

    inline RwStream* ResetStream( void )
    {
        Terminate();
        Initialize();

        return m_stream;
    }

    inline RwStream* GetRwStream( void )
    {
        return m_stream;
    }

    inline void* GetBuffer( void )
    {
        return streamData.buf.ptr;
    }

    inline size_t GetBufferSize( void )
    {
        return streamData.buf.size;
    }

    RwStream *m_stream;
};

struct corotPulseManager
{
    __forceinline corotPulseManager( CFiberSA *fiber ) : m_fiber( fiber )
    {
    }

    __forceinline bool DoLoading( void *buf, modelId_t id, unsigned int slicerIndex )
    {
        CModelLoadInfoSA& loadInfo = Streaming::GetModelLoadInfo( id );

        // Do the loading.
        bool success = false;
        {
            typedef corotRwLoadHandler loadHandler;
            typedef corotLoadFlavor <loadHandler> loadTechnique;

            loadHandler lHandler( buf, loadInfo.GetSize(), m_fiber );   // we are loading from .IMG chunks.

            ModelLoadDispatch <loadHandler, loadTechnique> loadDispatch( lHandler, loadTechnique( lHandler ), loadInfo );

            success = HandleLoaderDispatch( id, loadInfo, loadDispatch );
        }

        if ( success )
        {
            // Do post loading handling
            DefaultDispatchExecute( id, ModelPostLoadDispatch( loadInfo ) );

            // If we do not require a second loader run...
            if ( loadInfo.m_eLoading != MODEL_RELOAD )
                OnModelLoaded( id );
        }
        
        return success;
    }

    inline bool IsFibered( void )
    {
        return true;
    }

    CFiberSA *m_fiber;
};

void __stdcall LoaderFiberRuntime( CFiberSA *fiber, void *ud )
{
    unsigned int slicerIndex = (unsigned int)ud;
    streamingRequest& requester = Streaming::GetStreamingRequest( slicerIndex );

    while ( true )
    {
        // Run through the slicer.
        corotPulseManager pulseMan( fiber );

        requester.status = streamingRequest::STREAMING_LOADING;

        ProcessSlicerInstances( slicerIndex, pulseMan );

        requester.status = streamingRequest::STREAMING_NONE;

        // Yield for sure.
        fiber->yield();
    }
}

bool __cdecl ProcessStreamingRequest( unsigned int id )
{
    using namespace Streaming;

    streamingRequest& requester = Streaming::GetStreamingRequest( id );

    // Perform security checks depending on buffering behavior.
    if ( requester.bufBehavior == streamingRequest::BUFFERING_IMG )
    {
        // Are we waiting for resources?
        if ( unsigned int status = GetSyncSemaphoreStatus( Streaming::GetStreamingRequestSyncSemaphoreIndex( id ) ) )
        {
            if ( status == 0xFF || status == 0xFA )
                return false;   // the data request has not finished yet (safe).

            // Make our initial status
            requester.returnCode = status;
            requester.status = streamingRequest::STREAMING_WAITING;

            // We cannot wait for a model if we already wait for one!
            // Let this request fail then.
            if ( streamingWaitModel != -1 )
                return false;

            streamingWaitModel = id;

            CompleteStreamingRequest( id );
            return true;
        }
    }

    // This system depends on whether we load fibered or not.
    // NOTE: we may not change state during execution of this function!
    bool loadFibered = enableFiberedLoading;

    bool isLoading = ( requester.status == streamingRequest::STREAMING_LOADING );

    if ( !loadFibered )
        requester.status = streamingRequest::STREAMING_NONE;

    if ( !loadFibered && isLoading )
    {
        // Continue the loading procedure.
        // MTA extension: we use a slicer buffer variable for freedom to choose loading location.
        LoadBigModel( (char*)requester.loaderBuffer + requester.bufOffsets[0] * 2048, requester.ids[0] );

        // Mark that we are done.
        // The remaining slots of this requester will be cleared below.
        requester.ids[0] = -1;
    }
    else
    {
        // This logic loops through all requests in the streaming slicer and attempts to load
        // them in the first go. If loading has succeeded, the slot inside the slicer is cleared.
        // If a resource has not loaded in the first go, it is queued in the first slot of the
        // slicer.
        // The problem with this logic is that it is very limited.
        /*
            - isLoadingBigModel enforces that only the first slicer enters this function.
            - isLoadingBigModel is the main quantifier that decides whether a resource is
              split up in coroutine-fashion loading.
        */
        // I need to do the following:
        /*
            - unlink the relation of coroutine-loading from isLoadingBigModel.
        */
        // So.. I have to find a way to fix this.

        // Attempt to load the queue
        if ( loadFibered )
        {
            // Enter the fiber runtime.
            requester.loaderFiber->resume();
        }
        else
        {
            // Do it the semi-coroutine way.
            ProcessSlicerInstances( id, semiCorotPulseManager() );
        }
    }

    // Is a big model loading? But is the requester in an inappropriate status?
    // This is triggered when big-sized resources have been loaded but they were
    // not split into muliple pulses (i.e. big vehicle models). If a big resource
    // is loading, we are the primary requester (slicer 0).
    if ( isLoadingBigModel && requester.status != streamingRequest::STREAMING_LOADING )
    {
        // Reset big model loading
        isLoadingBigModel = false;

        // Clear the secondary queues
        for ( unsigned int n = 1; n < MAX_STREAMING_REQUESTERS; n++ )
            memset( GetStreamingRequest( n ).ids, 0xFF, sizeof(int) * MAX_STREAMING_REQUESTS );
    }

    // Processing succeeded!
    return true;
}

/*=========================================================
    PulseStreamingRequests

    Purpose:
        Updates the general status of the primary and secondary
        streaming requests. Called before and after
        LoadAllRequestedModels.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040E460
=========================================================*/
void __cdecl PulseStreamingRequests( void )
{
    using namespace Streaming;

    // Finish loading of secondary requesters.
    // This makes sure that big model requests (if they happen) do not fail.
    for ( unsigned int n = 1; n < MAX_STREAMING_REQUESTERS; n++ )
    {
        if ( GetStreamingRequest( n ).status == streamingRequest::STREAMING_LOADING )
            ProcessStreamingRequest( n );
    }

    // Process through the requesters in ascending order.
    // Ensures that all running requests progress.
    for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTERS; n++ )
    {
        streamingRequest& requester = GetStreamingRequest( n );

        // Load resources from the slicer if it requires them.
        if ( requester.status == streamingRequest::STREAMING_BUFFERING )
        {
            CancelSyncSemaphore( GetStreamingRequestSyncSemaphoreIndex( n ) );
            ProcessStreamingRequest( n );
        }

        // Load more resources onto the slicer if it is loading.
        if ( requester.status == streamingRequest::STREAMING_LOADING )
            ProcessStreamingRequest( n );
    }
}

/*=========================================================
    ProcessLoadQueue

    Arguments:
        offset - IMG archive offset to match against the result
        favorPriority - true if priority models should be handled first
    Purpose:
        Returns the model id of the model id which matches the
        given offset best.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00408E20
=========================================================*/
inline bool IsModelLoaded( const unsigned short offset, modelId_t id )
{
    eLoadingState status = Streaming::GetModelLoadInfo( offset, id ).m_eLoading;

    return status == MODEL_LOADED || status == MODEL_QUEUE;
}

inline bool EnsureResourceAvailability( const unsigned short offset, modelId_t id, unsigned char flags )
{
    if ( !IsModelLoaded( offset, id ) )
    {
        // Load any yet not finished dependencies
        Streaming::RequestModel( offset + id, flags );
        return true;    // we have to wait
    }

    return false; // ready for processing
}

inline bool AreAnimationDependenciesLoaded( modelId_t id )
{
    // Are we loading animations at all?
    return !*(bool*)0x00B5F852;// && Streaming::GetModelLoadInfo( 7 ).m_eLoading == MODEL_LOADED;

    // The_GTA: I removed the check which prevented animation loading if skin 7 was
    // not previously loaded. This should allow ped loading without the previous limitation.
}

struct ModelLoadQueueDispatch : ModelCheckDispatch <true, false>
{
    CModelLoadInfoSA*& m_item;

    __forceinline ModelLoadQueueDispatch( CModelLoadInfoSA*& item ) : m_item( item )
    {
    }

    bool __forceinline DoBaseModel( modelId_t id )
    {
        // Make sure all dependencies of this model are loaded
        CBaseModelInfoSAInterface *model = ppModelInfo[id];

        // Check out texture loading status
        if ( model->usTextureDictionary == -1 || !EnsureResourceAvailability( DATA_TEXTURE_BLOCK, model->usTextureDictionary, m_item->m_flags ) )
        {
            int animId = model->GetAnimFileIndex();

            return animId == -1 || !EnsureResourceAvailability( DATA_ANIM_BLOCK, animId, 0x08 );
        }

        // We need to wait, do not call success handler
        return false;
    }

    bool __forceinline DoTexDictionary( modelId_t id )
    {
        CTxdInstanceSA *txdInst = TextureManager::GetTxdPool()->Get( id );
        unsigned short parentId = txdInst->m_parentTxd;

        return parentId == 0xFFFF || !EnsureResourceAvailability( DATA_TEXTURE_BLOCK, parentId, 0x08 );
    }

    bool __forceinline DoAnimation( modelId_t id )
    {
        // Are we loading animations at all?
        return AreAnimationDependenciesLoaded( id );
    }
};

modelId_t __cdecl ProcessLoadQueue( unsigned int offset, bool favorPriority )
{
    using namespace Streaming;

    CModelLoadInfoSA *item = GetQueuedLoadInfo();

    unsigned int lastOffset = 0xFFFFFFFF;
    modelId_t lastId = -1;

    if ( item != *(CModelLoadInfoSA**)0x008E4C54 )
    {
        unsigned int upperOffset = 0xFFFFFFFF;
        modelId_t rangeId = -1;

        modelId_t itemId = item->GetIndex();

        do
        {
            if ( !favorPriority || Streaming::numPriorityRequests == 0 || item->m_flags & 0x10 )
            {
                // Are we allowed to return this value?
                if ( DefaultDispatchExecute( itemId, ModelLoadQueueDispatch( item ) ) )
                {
                    // Try to find a return value.
                    unsigned int itemOffset = item->GetStreamOffset();

                    if ( itemOffset < lastOffset )
                    {
                        lastId = itemId;
                        lastOffset = itemOffset;
                    }

                    if ( itemOffset < upperOffset && itemOffset >= offset )
                    {
                        upperOffset = itemOffset;
                        rangeId = itemId;
                    }
                }
            }

            item = &GetModelLoadInfo( itemId = item->m_primaryModel );
        }
        while ( item != *(CModelLoadInfoSA**)0x008E4C54 );

        if ( rangeId != -1 )
            return rangeId;
    }

    // Did we not result in anything and are there priority requests?
    if ( lastId == -1 && Streaming::numPriorityRequests != 0 )
    {
        // Reset the number of priority requests
        Streaming::numPriorityRequests = 0;

        // Try again with all requests included
        return ProcessLoadQueue( offset, false );
    }

    return lastId;
}

/*=========================================================
    PulseStreamingRequest

    Arguments:
        id - index of the streaming request
    Purpose:
        Main handler function of streaming request logic. Goes through
        the requested queue and puts them into the streaming request
        id and size array. If the resource fails the dependency tests,
        it is not loaded. If the streaming request waits for syncSemaphore
        activity, the syncSemaphore is notified.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040CBA0
=========================================================*/
inline bool CheckModelDependency( modelId_t id )
{
    if ( idOffset( id, DATA_TEXTURE_BLOCK ) < MAX_TXD )
        return CheckTXDDependency( idOffset( id, DATA_TEXTURE_BLOCK ) );
    else if ( idOffset( id, DATA_ANIM_BLOCK ) < 180 )
        return CheckAnimDependency( idOffset( id, DATA_ANIM_BLOCK ) );

    return true;
}

inline bool HaveModelDependenciesLoaded( CBaseModelInfoSAInterface *model )
{
    unsigned short txdId = (unsigned short)model->usTextureDictionary;

    if ( txdId != 0xFFFF && !IsModelLoaded( DATA_TEXTURE_BLOCK, txdId ) )
        return false;

    int animId = model->GetAnimFileIndex();

    return animId == -1 || IsModelLoaded( DATA_ANIM_BLOCK, animId );
}

struct ModelStreamingPulseDispatch : ModelCheckDispatch <false>
{
    CBaseModelInfoSAInterface*& m_model;
    unsigned int& m_blockCount;
    bool m_isVehicleLoading;
    bool m_isPedLoading;

    ModelStreamingPulseDispatch( CBaseModelInfoSAInterface*& model, unsigned int& blockCount ) :
        m_model( model ),
        m_blockCount( blockCount )
    {
        m_isVehicleLoading = false;
        m_isPedLoading = false;
    }

    bool __forceinline DoBaseModel( modelId_t id )
    {
        m_model = ppModelInfo[id];

        // We may only load one ped model at once
        return ( !m_isPedLoading || m_model->GetModelType() != MODEL_PED ) &&
        // We may only load one vehicle model at once
               ( !m_isVehicleLoading || m_model->GetModelType() != MODEL_VEHICLE ) &&
        // By now we require the dependencies loaded!
               HaveModelDependenciesLoaded( m_model );
    }

    bool __forceinline DoAnimation( modelId_t id )
    {
        // Make sure the animation dependencies are present.
        return AreAnimationDependenciesLoaded( id );
    }

    bool __forceinline CheckValidity( void )
    {
        return !m_isVehicleLoading || m_blockCount <= 200;
    }

    bool __forceinline DoCollision( modelId_t id )
    {
        return CheckValidity();
    }

    bool __forceinline DoIPL( modelId_t id )
    {
        return CheckValidity();
    }

    bool __forceinline DoRecording( modelId_t id )
    {
        return CheckValidity();
    }

    bool __forceinline DoTexDictionary( modelId_t id )
    {
        return CheckValidity();
    }

    bool __forceinline DoPathFind( modelId_t id )
    {
        return CheckValidity();
    }

    void __forceinline AfterPerform( modelId_t id )
    {
        if ( id < MAX_MODELS )
        {
            if ( m_model->GetModelType() == MODEL_PED )
                m_isPedLoading = true;
            else if ( m_model->GetModelType() == MODEL_VEHICLE )
                m_isVehicleLoading = true;
        }
        else if ( m_blockCount > 200 )
        {
            // This confuses me. Is it really describing a vehicle loading?
            // Maybe R* thought that only vehicle models could have such high poly counts.
            m_isVehicleLoading = true;
        }
    }
};

template <typename loaderDispatchType>
static bool __forceinline AddModelToSlicerLoading_ordered( streamingRequest& requester, int slotIndex, modelId_t modelId, loaderDispatchType& dispatch, unsigned int& threadBufferOffset, unsigned int& blockCount )
{
    CModelLoadInfoSA *loadInfo = &Streaming::GetModelLoadInfo( modelId );

    if ( loadInfo->m_eLoading != MODEL_LOADING )
        return false;

    loadInfo->GetBlockCount( blockCount );

    // If there are priority requests waiting to be loaded and this is
    // not a priority request, we cannot afford continuing
    if ( Streaming::numPriorityRequests && !( loadInfo->m_flags & FLAG_PRIORITY ) )
        return false;

    // Only valid for modelId < DATA_TEXTURE_BLOCK
    if ( !DefaultDispatchExecute( modelId, dispatch ) )
        return false;

    // Write our request into the streaming requester
    requester.bufOffsets[slotIndex] = threadBufferOffset;
    requester.ids[slotIndex] = modelId;

    // Set the new offset
    threadBufferOffset += blockCount;

    // If the request overshoots the thread allocation buffer at its offset...
    if ( threadBufferOffset > (unsigned int)Streaming::biggestResourceBlockCount )
    {
        if ( slotIndex > 0 )
        {
            threadBufferOffset -= blockCount;
            return false;
        }
    }

    // The_GTA: Here was some sort of debug table which stored all model ids that reached this
    // code location. I have left it out since this information was not further used during runtime.
    // Must have been a left-over from quick debugging. (1.0 US and 1.0 EU: 0x0040CE59)

    dispatch.AfterPerform( modelId );

    // Put it into the direct loading queue
    loadInfo->m_eLoading = MODEL_QUEUE;

    // Remove it from the long queue
    loadInfo->PopFromLoader();

    // Decrease the number of models that are in the long queue
    (*(unsigned int*)0x008E4CB8)--;

    // Unset priority status since we loaded
    if ( loadInfo->m_flags & FLAG_PRIORITY )
    {
        // Decrease number of priority requests
        Streaming::numPriorityRequests--;

        loadInfo->m_flags &= ~FLAG_PRIORITY;
    }

    return true;
}

void __cdecl PulseStreamingRequest( unsigned int reqId )
{
    using namespace Streaming;

    // The thread allocation buffer supports a chain of resource buffers.
    // The streaming requests keep track of buffer offsets (that is where
    // resource data is written to in the thread allocation buffer).
    // This also means that if this memory block count is overshot, we cause a
    // buffer overflow. R* indeed employed a smart loading mechanism!
    unsigned int threadBufferOffset = 0;

    // The_GTA: My speculation about the GetNextReadOffset is that it is
    // a performance improvement based on investigation. After all, reading
    // the exactly next offset in a file should be faster than jumping all
    // over the place!
    // This optimization could be disabled on SSD or RAM caching.
    unsigned int offset = GetStreamNextReadOffset();
    unsigned int blockCount = 0;

    modelId_t modelId = ProcessLoadQueue( offset, true );

    if ( modelId == -1 )
        return;

    CBaseModelInfoSAInterface *model;
    ModelStreamingPulseDispatch dispatch( model, blockCount );

    // Decide where to load the data from.
    streamingRequest::bufferingType bufferLoc;
    void *bufferPtr = NULL;
    {
        CModelLoadInfoSA *loadInfo = &GetModelLoadInfo( modelId );

        // Check for a model which matches dependency
        for ( ; !( loadInfo->m_flags & FLAG_NODEPENDENCY ) && !CheckModelDependency( modelId ); loadInfo = &GetModelLoadInfo( modelId ) )
        {
            // Since this model failed the dependency check, burn it!
            FreeModel( modelId );

            // Request the offset of this info (and blockCount)
            loadInfo->GetOffset( offset, blockCount );

            // Try to grab another modelId
            modelId = ProcessLoadQueue( offset, true );

            if ( modelId == -1 )
                return;
        }

        if ( modelId == -1 )
            return;

        // MTA extension: Attempt to load data from the cache so we do not have to request it from the disk.
        {
            void *cachedDataPtr = NULL;

            if ( StreamingCache::GetCachedIMGData( loadInfo->m_imgID, loadInfo->m_blockOffset, loadInfo->m_blockCount, cachedDataPtr ) )
            {
                bufferLoc = streamingRequest::BUFFERING_CACHE;
                bufferPtr = cachedDataPtr;
            }
        }

        // Attempt to load from disk if everything else fails.
        if ( bufferPtr == NULL )
        {
            // Get our offset information
            loadInfo->GetOffset( offset, blockCount );

            // Check whether it really is a big block
            if ( blockCount > (unsigned int)Streaming::biggestResourceBlockCount )
            {
                // We cannot request big models on the second requester.
                // If the secondary requesters are active, we have to let them finish first.
                for ( unsigned int n = 1; n < MAX_STREAMING_REQUESTERS; n++ )
                {
                    if ( reqId == n || GetStreamingRequest( n ).status != streamingRequest::STREAMING_NONE )
                        return;
                }

                // We are loading a big model, eh
                isLoadingBigModel = true;
            }

            bufferLoc = streamingRequest::BUFFERING_IMG;
            bufferPtr = Streaming::threadAllocationBuffers[reqId];
        }
    }

    // Attempt to perform loading on multiple resources in one go.
    streamingRequest& requester = GetStreamingRequest( reqId );

    int n = 0;

    if ( bufferLoc == streamingRequest::BUFFERING_IMG )
    {
        for ( ; n < MAX_STREAMING_REQUESTS; n++ )
        {
            if ( modelId == -1 || !AddModelToSlicerLoading_ordered( requester, n, modelId, dispatch, threadBufferOffset, blockCount ) )
                break;

            // Try to load the next model id in order.
            // The order is established in .IMG archive loading.
            modelId = GetModelLoadInfo( modelId ).m_lastID;
        }
    }
    else if ( bufferLoc == streamingRequest::BUFFERING_CACHE )
    {
        // In loading from the cache, we only support loading of one resource at a time.
        // We could change this in the future once caching becomes more sophisticated.
        if ( modelId != -1 )
            AddModelToSlicerLoading_ordered( requester, n, modelId, dispatch, threadBufferOffset, blockCount );
    }

    // GTA:SA code checked for n < MAX_STREAMING_REQUESTS; that is established here,
    // so I removed the check.
    for ( ; n < MAX_STREAMING_REQUESTS; n++ )
        requester.ids[n] = -1;

    // Push data into the buffer.
    if ( bufferLoc == streamingRequest::BUFFERING_IMG )
    {
        // Notify the synchronous semaphore.
        // Is this a normal request or just a big model one?
        ReadStream( Streaming::GetStreamingRequestSyncSemaphoreIndex( reqId ), bufferPtr, offset, threadBufferOffset );
    }

    // Update the requester
    requester.status = streamingRequest::STREAMING_BUFFERING;
    requester.statusCode = 0;
    requester.blockCount = threadBufferOffset;
    requester.offset = offset;
    requester.count = 0;

    // MTA extension: tell it about the request type.
    requester.loaderBuffer = bufferPtr;

    // I do not know what this is.
    *(bool*)0x009654C4 = false;
}

/*=========================================================
    Streaming::EnableFiberedLoading (MTA extension)

    Arguments:
        enabled - boolean to decide to turn fibered loading on or off
    Purpose:
        Turns fibered streaming loading on or off depending
        on the given parameter. Will perform streaming
        cycles to ensure system integrity when changing
        to fibered loading or to original runtime.
=========================================================*/
namespace Streaming
{
    CExecutiveGroupSA *fiberGroup = NULL;
};

void Streaming::LeaveFiberMode( void )
{
    CExecutiveManagerSA *fiberMan = pGame->GetExecutiveManager();

    // Terminate the current runtime.
    if ( enableFiberedLoading )
    {
        // Kill all fibers and destroy the group.
        for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTERS; n++ )
        {
            streamingRequest& requester = GetStreamingRequest( n );

            if ( CFiberSA *fiber = requester.loaderFiber )
            {
                fiberMan->CloseFiber( fiber );

                requester.loaderFiber = NULL;
            }
        }

        // Store the performance multiplier (in case it changed)
        loaderPerfMultiplier = fiberGroup->perfMultiplier;

        delete fiberGroup;

        fiberGroup = NULL;
    }
    else
    {
        // anything?
    }
}

void Streaming::EnterFiberMode( void )
{
    CExecutiveManagerSA *fiberMan = pGame->GetExecutiveManager();

    // Set up the new runtime.
    if ( enableFiberedLoading )
    {
        // Allocate the global execution group.
        fiberGroup = fiberMan->CreateGroup();
        fiberGroup->SetPerfMultiplier( loaderPerfMultiplier );

        // Allocate fibers for all slicers.
        for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTERS; n++ )
        {
            streamingRequest& requester = GetStreamingRequest( n );

            CFiberSA *newFiber = fiberMan->CreateFiber( LoaderFiberRuntime, (void*)n );
            
            fiberGroup->AddFiber( newFiber );

            requester.loaderFiber = newFiber;
        }
    }
    else
    {
        // anything?
    }
}

void Streaming::EnableFiberedLoading( bool enable )
{
    if ( enableFiberedLoading == enable )
        return;

    // Make sure the runtime does not load resources anymore.
    LoadAllRequestedModels( false );

    LeaveFiberMode();

    enableFiberedLoading = enable;

    EnterFiberMode();
}

bool Streaming::IsFiberedLoadingEnabled( void )
{
    return enableFiberedLoading;
}

void CStreamingSA::EnableFiberedLoading( bool enable )      { return Streaming::EnableFiberedLoading( enable ); }
bool CStreamingSA::IsFiberedLoadingEnabled( void ) const    { return Streaming::IsFiberedLoadingEnabled(); }

/*=========================================================
    Streaming::SetLoaderPerfMultiplier

    Arguments:
        multiplier - the perf multiplier to assign to the loader system
    Purpose:
        Sets the performance multiplier of all loading fibers.
        This multiplier decides how much of the frame time
        every fiber of the system may take to load resources.
=========================================================*/
void Streaming::SetLoaderPerfMultiplier( double multiplier )
{
    loaderPerfMultiplier = multiplier;

    if ( fiberGroup )
        fiberGroup->SetPerfMultiplier( multiplier );
}

double Streaming::GetLoaderPerfMultiplier( void )
{
    return ( fiberGroup ) ? fiberGroup->perfMultiplier : loaderPerfMultiplier;
}

void CStreamingSA::SetFiberedPerfMultiplier( double mult )      { Streaming::SetLoaderPerfMultiplier( mult ); }
double CStreamingSA::GetFiberedPerfMultiplier( void ) const     { return Streaming::GetLoaderPerfMultiplier(); }

/*=========================================================
    Streaming::LoadAllRequestedModels

    Arguments:
        onlyPriority - appears to favour prioritized models if true
    Purpose:
        Cycles through the streaming loading system to process
        loader queues (load and termination requests).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040EA10
=========================================================*/
namespace Streaming
{
    volatile bool insideLoadAllRequestedModels = false;   // GTA:SA code is not C++ exception friendly.
};

__forceinline unsigned int GetNextThreadId( unsigned int threadId )
{
    // Optimizations.
    if ( MAX_STREAMING_REQUESTERS == 2 )
        return 1 - threadId;

    // Switch the thread id (cycle through them)
    // Optimized modulo.
    if ( threadId == MAX_STREAMING_REQUESTERS - 1 )
        return 0;
    
    return threadId + 1;
}

inline void __cdecl ContinueResourceAcquisition( unsigned int threadId )
{
    // We can only perform this loading logic if all models have acquired their resources
    // (no big models are being loaded)
    if ( !Streaming::isLoadingBigModel )
    {
        // Pulse the other streaming requesters if we can
        // Always give the other requesters priority.
        unsigned int reqId = GetNextThreadId( threadId );

        for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTERS && !Streaming::isLoadingBigModel; n++ )
        {
            // While no big model is loading, we happily draw resources on the slicers.
            if ( Streaming::GetStreamingRequest( reqId ).status == streamingRequest::STREAMING_NONE )
                PulseStreamingRequest( reqId );

            // Advance the requester index
            reqId = GetNextThreadId( reqId );
        }
    }
}

__forceinline bool IsAnySlicerActivity( void )
{
    for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTERS; n++ )
    {
        if ( Streaming::GetStreamingRequest( n ).status != streamingRequest::STREAMING_NONE )
            return true;
    }

    return false;
}

void __cdecl Streaming::LoadAllRequestedModels( bool onlyPriority )
{
    using namespace Streaming;

    // Protect against running this function two times in parallel.
    // Either multi-threaded or stacked.
    if ( insideLoadAllRequestedModels )
        return;

    insideLoadAllRequestedModels= true;
    PulseStreamingRequests();
    
    unsigned int pulseCount = std::max( (unsigned int)10, *(unsigned int*)0x008E4CB8 * 2 );
    unsigned int threadId = 0;

    for ( ; pulseCount != 0; pulseCount-- )
    {
        // Check whether activity is required at all
        // Check all streaming slicers, that they got any work to do.
        // We made sure by pulsing the requesters, that resources are on them.
        if ( GetLastQueuedLoadInfo() == *(CModelLoadInfoSA**)0x008E4C58 && !IsAnySlicerActivity() )
            break;

        if ( isLoadingBigModel )
            threadId = 0;

        streamingRequest& requester = GetStreamingRequest( threadId );

        // Cancel any pending activity (if PulseStreamingRequests did not finish it)
        if ( requester.status != streamingRequest::STREAMING_NONE )
        {
            CancelSyncSemaphore( Streaming::GetStreamingRequestSyncSemaphoreIndex( threadId ) );

            // Tell the requester about it
            requester.statusCode = 100;
        }

        // Attempt to finish the loading procedure.
        if ( requester.status == streamingRequest::STREAMING_BUFFERING )
        {
            ProcessStreamingRequest( threadId );

            // This once again enforces this coroutine like loading logic.
            // It expects resources to at least take two pulses to load properly.
            // The system breaks if more pulses are required.
            while ( requester.status == streamingRequest::STREAMING_LOADING )
                ProcessStreamingRequest( threadId );
        }

        if ( !enableFiberedLoading )
        {
            // If we expect to load only priority and there is no priority models
            // to load, we can cancel here.
            if ( onlyPriority && numPriorityRequests == 0 )
                break;
        }

        // Attempt to draw more resources into the queue.
        ContinueResourceAcquisition( threadId );

        // If we have no more activity, we can break here
        if ( !IsAnySlicerActivity() )
            break;

        // Switch to next thread id.
        threadId = GetNextThreadId( threadId );
    }
    
    PulseStreamingRequests();
    insideLoadAllRequestedModels = false;
}

bool CStreamingSA::IsInsideLoadAllRequestedModels( void ) const     { return Streaming::insideLoadAllRequestedModels; }

/*=========================================================
    Streaming::PulseLoader

    Purpose:
        Is called every frame to process streaming requests.
        This way resources get loaded asynchronically by the
        engine.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040E3A0
=========================================================*/
namespace Streaming
{
    unsigned int activeStreamingThread = 0;
};

void __cdecl Streaming::PulseLoader( void )
{
    unsigned int curThread = activeStreamingThread;

    // If a big model is loading, we must process the primary
    // loader.
    if ( isLoadingBigModel )
    {
        curThread = 0;
        activeStreamingThread = 0;
    }

    streamingRequest& requester = GetStreamingRequest( curThread );

    // Pulse resource loading for the main thread.
    if ( requester.status != streamingRequest::STREAMING_NONE )
        ProcessStreamingRequest( curThread );

    // Draw resources into the queue.
    ContinueResourceAcquisition( curThread );

    // If the current requester is not loading anymore,
    // we can switch to another thread.
    if ( requester.status != streamingRequest::STREAMING_LOADING )
        activeStreamingThread = GetNextThreadId( curThread );
}

// Loader hacks for better performance.
// Return true to skip
bool _cdecl ShouldSkipLoadRequestedModels( DWORD calledFrom )
{
    if ( !StreamingCache::IsIMGFileCached( PLAYER_IMG_FILENAME ) )
        return false;

    // Skip LoadRequestedModels if called from:
    //      CClothesBuilder::ConstructGeometryArray      5A55A0 - 5A56B6
    //      CClothesBuilder::LoadAndPutOnClothes         5A5F70 - 5A6039
    //      CClothesBuilder::ConstructTextures           5A6040 - 5A6520
    return calledFrom > 0x5A55A0 && calledFrom < 0x5A6520;
}

void __cdecl HOOK_PulseStreamingLoader( void )
{
// hook from 015670A0/01567090 5 bytes
    if ( !ShouldSkipLoadRequestedModels( (DWORD)_ReturnAddress() ) )
    {
        // MTA extension: if we are loading using fibers, we must make
        // sure that loading at least finished. There are bugs inside
        // of the engine that do not expect a slow loader.
        if ( Streaming::enableFiberedLoading )
            Streaming::LoadAllRequestedModels( false );
        else
            Streaming::PulseLoader();
    }
}

/*=========================================================
    CStreamingSA::SetLoadCallback

    Arguments:
        callback - function to be called once a model successfully loaded
    Purpose:
        Specify an internal callback which should be issued once a
        model was successfully loaded.
=========================================================*/
void CStreamingSA::SetLoadCallback( streamingLoadCallback_t callback )
{
    Streaming::streamingLoadCallback = callback;
}

void StreamingResources_Init( void )
{
    // Hook fixes.
    HookInstall( 0x0040E3A0, (DWORD)HOOK_PulseStreamingLoader, 5 );
}

void StreamingResources_Shutdown( void )
{
}