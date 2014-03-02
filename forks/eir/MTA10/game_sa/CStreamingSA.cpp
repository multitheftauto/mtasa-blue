/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.cpp
*  PURPOSE:     Data streamer
*  DEVELOPERS:  Jax <>
*               jenksta <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

namespace
{
    //
    // Used in LoadAllRequestedModels to record state
    //
    struct SPassStats
    {
        bool bLoadingBigModel;
        DWORD numPriorityRequests;
        DWORD numModelsRequested;
        DWORD memoryUsed;

        void Record ( void )
        {
            #define VAR_CStreaming_bLoadingBigModel         0x08E4A58
            #define VAR_CStreaming_numPriorityRequests      0x08E4BA0
            #define VAR_CStreaming_numModelsRequested       0x08E4CB8
            #define VAR_CStreaming_memoryUsed               0x08E4CB4

            bLoadingBigModel    = *(BYTE*)VAR_CStreaming_bLoadingBigModel != 0;
            numPriorityRequests = *(DWORD*)VAR_CStreaming_numPriorityRequests;
            numModelsRequested  = *(DWORD*)VAR_CStreaming_numModelsRequested;
            memoryUsed          = *(DWORD*)VAR_CStreaming_memoryUsed;
        }
    };
};

CIPLFilePool **ppIPLFilePool = (CIPLFilePool**)CLASS_CIPLFilePool;

extern CBaseModelInfoSAInterface **ppModelInfo;

CColModelSA *g_colReplacement[DATA_TEXTURE_BLOCK];
CColModelSAInterface *g_originalCollision[DATA_TEXTURE_BLOCK];

#define ARRAY_PEDSPECMODEL      0x008E4C00
#define VAR_PEDSPECMODEL        0x008E4BB0
#define VAR_MEMORYUSAGE         0x008E4CB4
#define VAR_NUMMODELS           0x008E4CB8
#define VAR_NUMPRIOMODELS       0x008E4BA0

static streamingRequestCallback_t streamingRequestCallback = NULL;
static streamingFreeCallback_t streamingFreeCallback = NULL;

#include "CStreamingSA.utils.hxx"

/*=========================================================
    Streaming::RequestModel

    Arguments:
        id - model info index to request
        flags - priority flags for this request
    Purpose:
        Hook so that every engine resource request is handled
        through MTA code.
    Binary offsets:
        (see CStreamingSA::RequestModel)
=========================================================*/
struct ModelRequestDispatch : ModelCheckDispatch <true>
{
    unsigned int& m_flags;

    ModelRequestDispatch( unsigned int& flags ) : m_flags( flags )
    {
    }

    bool __forceinline DoBaseModel( modelId_t id )
    {
        CBaseModelInfoSAInterface *model = ppModelInfo[id];
        int animIndex = model->GetAnimFileIndex();

        // Request the models textures
        Streaming::RequestModel( model->usTextureDictionary + DATA_TEXTURE_BLOCK, m_flags );

        // Get animation if necessary
        if ( animIndex != -1 )
            Streaming::RequestModel( animIndex + DATA_ANIM_BLOCK, 0x08 );

        return true;
    }

    bool __forceinline DoTexDictionary( modelId_t id )
    {
        CTxdInstanceSA *txd = (*ppTxdPool)->Get( id );

        // Crashfix
        if ( !txd )
            return false;

#ifdef MTA_DEBUG
        OutputDebugString( SString( "loaded texDictionary %u\n", id ) );
#endif

        // I think it loads textures, lol
        if ( txd->m_parentTxd != 0xFFFF )
            Streaming::RequestModel( txd->m_parentTxd + DATA_TEXTURE_BLOCK, m_flags );

        return true;
    }
};

void __cdecl Streaming::RequestModel( modelId_t id, unsigned int flags )
{
    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;

    if ( id > MAX_RESOURCES-1 )
        return;

    switch( info->m_eLoading )
    {
    case MODEL_LOADING:
        if ( flags & 0x10 && !(info->m_flags & 0x10) )
        {
            (*(DWORD*)VAR_NUMPRIOMODELS)++;

            info->m_flags |= FLAG_PRIORITY;
        } 
        break;
    case MODEL_UNAVAILABLE:
        // We only call the client if we request unavailable items
        if ( streamingRequestCallback )
            streamingRequestCallback( id );

#ifdef RENDERWARE_VIRTUAL_INTERFACES
        // Model support fix: quick load a model if we already have it for replacement; prevents memory leaks and boosts speed
	    // as opposed to letting the original/replaced game resources load
        if ( id < DATA_TEXTURE_BLOCK )
        {
            CBaseModelInfoSAInterface *minfo = ppModelInfo[id];

            // Fix for invalid model requests
            if ( !minfo )
                return;

            eRwType rwType = minfo->GetRwModelType();

            if ( CRwObjectSA *obj = g_replObjectNative[id] )
            {
                // Apply the model
                switch( rwType )
                {
                case RW_ATOMIC:
                    ((CAtomicModelInfoSA*)minfo)->SetAtomic( ((CRpAtomicSA*)obj)->CreateInstance( id ) ); // making a copy is essential for model instance isolation
                    break;
                case RW_CLUMP:
                    ((CClumpModelInfoSAInterface*)minfo)->SetClump( RpClumpClone( (RpClump*)obj->GetObject() ) );
                    break;
                }

                // Set us loaded, instantly.
                info->m_eLoading = MODEL_LOADED;

                // We should notify our environment about the successful loading
                if ( streamingLoadCallback )
                    streamingLoadCallback( id );

                return;
            }
        }
#endif //RENDERWARE_VIRTUAL_INTERFACES
        break;
    default:
        flags &= ~FLAG_PRIORITY;
        break;
    }

    info->m_flags |= (unsigned char)flags;

    // Refresh the model loading?
    if ( info->m_eLoading == MODEL_LOADED )
    {
        if ( !info->IsOnLoader() )
            return;

        // Remove loaded model from its queue (it has to reside in one)
        info->PopFromLoader();

#ifdef RENDERWARE_VIRTUAL_INTERFACES
        if ( g_replObjectNative[id] != NULL )
            return;
#endif //RENDERWARE_VIRTUAL_INTERFACES

        if ( id < MAX_MODELS )
        {
            CBaseModelInfoSAInterface *model = ppModelInfo[id];

            switch( model->GetModelType() )
            {
            case MODEL_VEHICLE:
            case MODEL_PED:
                // We do not reload vehicles or peds
                // They need their model info intact, as they read data from it.
                return;
            }
        }

        // Push it onto a different queue I have no idea about.
        // This might have to do with Garbage Collection.
        // So, if we request with 0x06 flag, we prevent models from being collected?
        if ( !( info->m_flags & (0x02 | 0x04) ) )
            info->PushIntoLoader( *(CModelLoadInfoSA**)0x008E4C60 );
    }
    else if ( info->m_eLoading == MODEL_UNAVAILABLE )
    {
        // This resource has to be instantiated to be ready again.
        if ( !DefaultDispatchExecute( id, ModelRequestDispatch( flags ) ) )
            return;

        // Wire in a MTA team fix regarding clothes replacing.
        // What a dirty hook :p
        if ( OnCStreaming_RequestModel_Mid( flags, (ClothesReplacing::SImgGTAItemInfo*)info ) )
        {
            // MTA team wants to skip async loading by doing things directly.
            // Well, okay.
            LoadModel( ClothesReplacing::pReturnBuffer, ClothesReplacing::iReturnFileId, 0 );
            return;
        }

        // Push onto the to-be-loaded queue
        info->PushIntoLoader( *(CModelLoadInfoSA**)0x008E4C58 );

        // Tell the loader that there is a resource waiting
        (*(DWORD*)VAR_NUMMODELS)++;

        if ( flags & 0x10 )
            (*(DWORD*)VAR_NUMPRIOMODELS)++;

        // If available, we reload the model
        info->m_flags = flags;

        info->m_eLoading = MODEL_LOADING;
    }
}

/*=========================================================
    Streaming::FreeModel

    Arguments:
        id - model info index to request
        flags - priority flags for this request
    Purpose:
        Hook so that every engine resource request is handled
        through MTA code.
    Binary offsets:
        (see CStreamingSA::FreeModel)
=========================================================*/
struct ModelFreeDispatch : ModelCheckDispatch <false>   // by default we do not let invalid things pass
{
    bool __forceinline DoBaseModel( modelId_t id )
    {
        CBaseModelInfoSAInterface *model = ppModelInfo[id];
        int unk;
        unsigned int *unk2;

        // Model management fix: we unlink the collision so GTA:SA does not destroy it during
        // RwObject deletion
        if ( g_colReplacement[id] && model->GetRwModelType() == RW_CLUMP )
            model->pColModel = NULL;

        model->DeleteRwObject();

        switch( model->GetModelType() )
        {
        case MODEL_ATOMIC:
#ifdef _DEBUG
            OutputDebugString( SString( "deleted mesh-type model %u\n", id ) );
#endif
            break;
        case MODEL_PED:
            unk = *(int*)VAR_PEDSPECMODEL;
            unk2 = (unsigned int*)ARRAY_PEDSPECMODEL;

            while ( unk2 < (unsigned int*)ARRAY_PEDSPECMODEL + 5 )
            {
                if (*unk2 == id)
                {
                    *unk2 = 0xFFFFFFFF;

                    unk--;
                }

                unk2++;
            }

            *(int*)VAR_PEDSPECMODEL = unk;

            break;
        case MODEL_VEHICLE:
#ifdef MTA_DEBUG
            OutputDebugString( SString( "deleted vehicle model %u\n", id ) );
#endif

            // The_GTA: I removed the unused "vehicles" maximum clean-up code here.
            // It was never used in GTA:SA anyway.
            break;
        }

#ifdef RENDERWARE_VIRTUAL_INTERFACES
        // Model support bugfix: if we have a replacement for this model, we should not
        // bother about management in the CStreaming class, so quit here
        if ( g_replObjectNative[id] )
            return false;
#endif //RENDERWARE_VIRTUAL_INTERFACES

        return true;
    }

    bool __forceinline DoTexDictionary( modelId_t id )
    {
#ifdef MTA_DEBUG
        OutputDebugString( SString( "Deleted texDictionary %u\n", id ) );
#endif

        // Deallocate textures
        (*ppTxdPool)->Get( id )->Deallocate();
        return true;
    }

    bool __forceinline DoCollision( modelId_t id )
    {
        // Destroy all collisions associated with the COL library
        FreeCOLLibrary( (unsigned char)id );
        return true;
    }

    bool __forceinline DoIPL( modelId_t id )
    {
        // This function destroys buildings/IPLs!
        ((void (__cdecl*)( modelId_t ))0x00404B20)( id );
        return true;
    }

    bool __forceinline DoPathFind( modelId_t id )
    {
        __asm
        {
            mov eax,id

            push eax
            mov ecx,CLASS_CPathFind
            mov eax,0x0044D0F0
            call eax
        }

        return true;
    }

    bool __forceinline DoAnimation( modelId_t id )
    {
        // Animations...?
        pGame->GetAnimManager()->RemoveAnimBlock( id );
        return true;
    }

    bool __forceinline DoScript( modelId_t id )
    {
        __asm
        {
            mov eax,id

            mov ecx,0x00A47B60
            push eax
            mov eax,0x004708E0
            call eax
        }
    }
};

// Just about the same, but with a different base model logic.
struct ModelAbortBigRequestDispatcher : ModelFreeDispatch
{
    bool DoBaseModel( modelId_t id )
    {
        RwFlushLoader();
        return true;
    }
};

void __cdecl Streaming::FreeModel( modelId_t id )
{
    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;

    if ( id > MAX_RESOURCES-1 )
        return;

    // Unavailable resources do not need termination
    if ( info->m_eLoading == MODEL_UNAVAILABLE )
        return;

    // Perform the freeing logic
    if ( info->m_eLoading == MODEL_LOADED && DefaultDispatchExecute( id, ModelFreeDispatch() ) )
    {
        // Only decrease if the free-request was successful!
        *(DWORD*)VAR_MEMORYUSAGE -= info->m_blockCount * 2048;
    }

    if ( info->IsOnLoader() )
    {
        if ( info->m_eLoading == MODEL_LOADING )
        {
            // LoadAllRequestedModels wants to keep track of the number of requests ongoing
            (*(DWORD*)VAR_NUMMODELS)--;

            if ( info->m_flags & FLAG_PRIORITY )
            {
                info->m_flags &= ~FLAG_PRIORITY;

                // It also wants the count of priority requests, as they are prefered.
                (*(DWORD*)VAR_NUMPRIOMODELS)--;
            }
        }

        // Remove us from any loading queue (garbage collect or to-be-loaded)
        info->PopFromLoader();
    }
    else if ( info->m_eLoading == MODEL_QUEUE )
    {
        streamingRequest& primary = Streaming::GetStreamingRequest( 0 );
        streamingRequest& secondary = Streaming::GetStreamingRequest( 1 );

        // Invalidate any running syncSemaphore requests for this model id
        // Data which has been read from the IMG archive will be ignored.
        for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTS; n++ )
        {
            if ( primary.ids[n] == id )
                primary.ids[n] = -1;

            if ( secondary.ids[n] == id )
                secondary.ids[n] = -1;
        }
    }
    else if ( info->m_eLoading == MODEL_RELOAD )
    {
        // Abort an ongoing/flattened-out loading process
        DefaultDispatchExecute( id, ModelAbortBigRequestDispatcher() );
    }

    // Mark that all resources have been terminated.
    info->m_eLoading = MODEL_UNAVAILABLE;

    // Notify user environments
    if ( streamingFreeCallback )
        streamingFreeCallback( id );
}

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
static volatile bool _isLoadingRequests = false;

void __cdecl Streaming::LoadAllRequestedModels( bool onlyPriority )
{
    using namespace Streaming;

    if ( _isLoadingRequests )
        return;

    _isLoadingRequests = true;
    PulseStreamingRequests();
    
    unsigned int pulseCount = std::max( 10u, *(unsigned int*)0x008E4CB8 * 2 );
    unsigned int threadId = 0;

    for ( ; pulseCount != 0; pulseCount-- )
    {
        // Check whether activity is required at all
        if ( GetLastQueuedLoadInfo() == *(CModelLoadInfoSA**)0x008E4C58 &&
            GetStreamingRequest( 0 ).status == streamingRequest::STREAMING_NONE &&
            GetStreamingRequest( 1 ).status == streamingRequest::STREAMING_NONE )
            break;

        if ( pulseCount == 0 )
            break;

        if ( *(bool*)0x008E4A58 )
            threadId = 0;

        streamingRequest& requester = GetStreamingRequest( threadId );

        // Cancel any pending activity (if PulseStreamingRequests did not finish it)
        if ( requester.status != streamingRequest::STREAMING_NONE )
        {
            CancelSyncSemaphore( threadId );

            // Tell the requester about it
            requester.statusCode = 100;
        }

        // Try to finish the loading procedure
        if ( requester.status == streamingRequest::STREAMING_BUFFERING )
        {
            ProcessStreamingRequest( threadId );

            // This once again enforces this coroutine like loading logic.
            // It expects resources to at least take two pulses to load properly.
            // The system breaks if more pulses are required.
            if ( requester.status == streamingRequest::STREAMING_LOADING )
                ProcessStreamingRequest( threadId );
        }

        // If we expect to load only priority and there is no priority models
        // to load, we can cancel here.
        if ( onlyPriority && *(unsigned int*)0x008E4BA0 == 0 )
            break;

        // We can only perform this loading logic if all models have acquired their resources
        // (no big models are being loaded)
        if ( !*(bool*)0x008E4A58 )
        {
            streamingRequest& otherRequester = GetStreamingRequest( 1 - threadId );

            // Pulse the other streaming request if we can
            if ( otherRequester.status == streamingRequest::STREAMING_NONE )
                PulseStreamingRequest( 1 - threadId );

            // Check that we did not begin loading a big model
            // Pulse the primary requester if we can
            if ( !*(bool*)0x008E4A58 && requester.status == streamingRequest::STREAMING_NONE )
                PulseStreamingRequest( threadId );
        }

        // If we have no more activity, we can break here
        if ( GetStreamingRequest( 0 ).status == streamingRequest::STREAMING_NONE &&
             GetStreamingRequest( 1 ).status == streamingRequest::STREAMING_NONE )
            break;

        // Switch the thread id (cycle through them)
        threadId = 1 - threadId;
    }
    
    PulseStreamingRequests();
    _isLoadingRequests = false;
}

CStreamingSA::CStreamingSA( void )
{
    // Initialize the accelerated streaming structures
    memset( g_colReplacement, 0, sizeof(g_colReplacement) );
    memset( g_originalCollision, 0, sizeof(g_originalCollision) );

    // Hook the model requester
    HookInstall( FUNC_CStreaming__RequestModel, (DWORD)Streaming::RequestModel, 6 );
    HookInstall( 0x004089A0, (DWORD)Streaming::FreeModel, 6 );
    HookInstall( 0x00410730, (DWORD)FreeCOLLibrary, 5 );
    HookInstall( 0x0040C6B0, (DWORD)LoadModel, 5 );
    //HookInstall( 0x00407AD0, (DWORD)CheckAnimDependency, 5 );
    //HookInstall( 0x00409A90, (DWORD)CheckTXDDependency, 5 );  // you better do not fuck around with securom
    
    HookInstall( FUNC_LoadAllRequestedModels, (DWORD)Streaming::LoadAllRequestedModels, 5 );
    HookInstall( 0x00408E20, (DWORD)ProcessLoadQueue, 5 );
    HookInstall( 0x0040E170, (DWORD)ProcessStreamingRequest, 5 );
    HookInstall( 0x0040CBA0, (DWORD)PulseStreamingRequest, 5 );
    HookInstall( 0x0040E460, (DWORD)PulseStreamingRequests, 5 );

    StreamingLoader_Init();
    StreamingRuntime_Init();
}

CStreamingSA::~CStreamingSA( void )
{
    StreamingRuntime_Shutdown();
    StreamingLoader_Shutdown();
}

/*=========================================================
    CStreamingSA::RequestModel

    Arguments:
        id - model info index to request
        flags - priority flags for this request
    Purpose:
        Requests a model to be loaded. If a model was previously
        loading, it is flagged priority automatically. Associated
        resources of this model also get requested (TXD, anim, ...).
        If the requested model is already loaded, it is flagged to
        be reloaded.
        If the model id belongs to an upgrade, perform the upgrade
        requester routine.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004087E0
=========================================================*/
inline static bool IsUpgradeModelId( modelId_t dwModelID )
{
    return dwModelID >= 1000 && dwModelID <= 1193;
}

void CStreamingSA::RequestModel( modelId_t id, unsigned int flags )
{
    if ( IsUpgradeModelId( id ) )
        RequestVehicleUpgrade( id, flags );
    else
        Streaming::RequestModel( id, flags );
}

/*=========================================================
    CStreamingSA::FreeModel

    Arguments:
        id - model info index to free
    Purpose:
        Terminates all resources associated with the resource
        described by id. This will destroy models, textures,
        buildings, collisions, animations, paths and (maybe)
        scripts. Flattened out loading procedures are aborted
        (MODEL_RELOAD).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004089A0
=========================================================*/
void CStreamingSA::FreeModel( modelId_t id )
{
    Streaming::FreeModel( id );
}

/*=========================================================
    CStreamingSA::LoadAllRequestedModels

    Arguments:
        onlyPriority - appears to favour prioritized models if true
        debugLoc - string debug information about calling location
    Purpose:
        Cycles through the streaming loading system to process
        loader queues (load and termination requests).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040EA10
=========================================================*/
void CStreamingSA::LoadAllRequestedModels( bool onlyPriority, const char *debugLoc )
{
    Streaming::LoadAllRequestedModels( onlyPriority );
}

/*=========================================================
    CStreamingSA::HasModelLoaded

    Arguments:
        id - model info index in question
    Purpose:
        Returns whether the specified resource is already
        loaded.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004044C0
=========================================================*/
bool CStreamingSA::HasModelLoaded( modelId_t id )
{
    if ( IsUpgradeModelId( id ) )
        return HasVehicleUpgradeLoaded( id );
    else
        return Streaming::GetModelLoadInfo( id ).m_eLoading == MODEL_LOADED;
}

/*=========================================================
    CStreamingSA::IsModelLoading

    Arguments:
        id - model info index in question
    Purpose:
        Returns whether the specified model info is loading.
        If you need it loaded, you should call LoadAllRequestedModels
        to give execution time to the streaming loader.
=========================================================*/
bool CStreamingSA::IsModelLoading( modelId_t id )
{
    eLoadingState state = Streaming::GetModelLoadInfo( id ).m_eLoading;
    return state == MODEL_LOADING || state == MODEL_RELOAD;
}

/*=========================================================
    CStreamingSA::WaitForModel (todo) (MTA extension)

    Arguments:
        id - model info index to wait for
    Purpose:
        Blocks thread execution until a resource has loaded.
=========================================================*/
void CStreamingSA::WaitForModel( modelId_t id )
{
    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;

    if ( id > MAX_RESOURCES-1 )
        return;
}

/*=========================================================
    CStreamingSA::RequestSpecialModel

    Arguments:
        model - model info index
        tex - TXD container name
        channel - ???
    Purpose:
        Requests a model and destroys conflicting entities.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00409D10
=========================================================*/
void CStreamingSA::RequestAnimations( int idx, unsigned int flags )
{
    idx += DATA_ANIM_BLOCK;
    Streaming::RequestModel( idx, flags );
}

/*=========================================================
    CStreamingSA::HaveAnimationsLoaded

    Arguments:
        idx - animation block index
    Purpose:
        Checks whether animations have loaded.
=========================================================*/
bool CStreamingSA::HaveAnimationsLoaded( int idx )
{
    idx += DATA_ANIM_BLOCK;
    return HasModelLoaded( idx );
}

/*=========================================================
    CStreamingSA::RequestVehicleUpgrade

    Arguments:
        model - upgrade model index
        flags - loader attribute flags
    Purpose:
        Requests the loading of a vehicle upgrade.
=========================================================*/
void CStreamingSA::RequestVehicleUpgrade( modelId_t model, unsigned int flags )
{
    ((void (__cdecl*)( modelId_t, unsigned int ))FUNC_RequestVehicleUpgrade)( model, flags );
}

/*=========================================================
    CStreamingSA::HasVehicleUpgradeLoaded

    Arguments:
        model - model info index of the upgrade
    Purpose:
        Returns whether the specified vehicle upgrade model has
        loaded already. This function fails if the model id is
        not belonging to an upgrade.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00407820
=========================================================*/
bool CStreamingSA::HasVehicleUpgradeLoaded( int model )
{
    if ( Streaming::GetModelLoadInfo( model ).m_eLoading != MODEL_LOADED )
        return false;
    
    unsigned short id = g_upgStore->FindModelAssociation( model );

    return id == 0xFFFF || Streaming::GetModelLoadInfo( id ).m_eLoading == MODEL_LOADED;
}

/*=========================================================
    CStreamingSA::RequestSpecialModel

    Arguments:
        model - model info index
        tex - TXD container name
        channel - ???
    Purpose:
        Requests a model and destroys conflicting entities.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00409D10
=========================================================*/
void CStreamingSA::RequestSpecialModel( modelId_t model, const char *tex, unsigned int channel )
{
    ((void (__cdecl*)( modelId_t model, const char *tex, unsigned int channel ))FUNC_CStreaming_RequestSpecialModel)( model, tex, channel );
}

void CStreamingSA::SetRequestCallback( streamingRequestCallback_t callback )
{
    streamingRequestCallback = callback;
}

void CStreamingSA::SetFreeCallback( streamingFreeCallback_t callback  )
{
    streamingFreeCallback = callback;
}