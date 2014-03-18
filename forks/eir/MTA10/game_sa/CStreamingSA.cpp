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
            #define VAR_CStreaming_numModelsRequested       0x08E4CB8
            #define VAR_CStreaming_memoryUsed               0x08E4CB4

            bLoadingBigModel    = Streaming::isLoadingBigModel;
            numPriorityRequests = Streaming::numPriorityRequests;
            numModelsRequested  = *(DWORD*)VAR_CStreaming_numModelsRequested;
            memoryUsed          = *(DWORD*)VAR_CStreaming_memoryUsed;
        }
    };
};

extern CBaseModelInfoSAInterface **ppModelInfo;

CColModelSA *g_colReplacement[MAX_MODELS];
CColModelSAInterface *g_originalCollision[MAX_MODELS];

#define VAR_MEMORYUSAGE         0x008E4CB4
#define VAR_NUMMODELS           0x008E4CB8

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
            Streaming::RequestModel( animIndex + DATA_ANIM_BLOCK, m_flags );

        return true;
    }

    bool __forceinline DoTexDictionary( modelId_t id )
    {
        CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( id );

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
            Streaming::numPriorityRequests++;

            info->m_flags |= FLAG_PRIORITY;
        } 
        break;
    case MODEL_UNAVAILABLE:
        // We only call the client if we request unavailable items
        if ( streamingRequestCallback )
            streamingRequestCallback( id );

        // Model support fix: quick load a model if we already have it for replacement; prevents memory leaks and boosts speed
	    // as opposed to letting the original/replaced game resources load
        if ( id < DATA_TEXTURE_BLOCK )
        {
            CBaseModelInfoSAInterface *minfo = ppModelInfo[id];

            // Fix for invalid model requests (jff)
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

        // Prevent procedure of models which have been replaced.
        // This stops them from being garbage collected.
        if ( g_replObjectNative[id] != NULL )
            return;

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
            info->PushIntoLoader( GetLastGarbageCollectModel() );
    }
    else if ( info->m_eLoading == MODEL_UNAVAILABLE )
    {
        // This resource has to be instantiated to be ready again.
        if ( !DefaultDispatchExecute( id, ModelRequestDispatch( flags ) ) )
            return;

#ifdef _MTA_BLUE
        // Wire in a MTA team fix regarding clothes replacing.
        // What a dirty hook :p
        if ( OnCStreaming_RequestModel_Mid( flags, (ClothesReplacing::SImgGTAItemInfo*)info ) )
        {
            // MTA team wants to skip async loading by doing things directly.
            // Well, okay.
            LoadModel( ClothesReplacing::pReturnBuffer, ClothesReplacing::iReturnFileId, 0 );
            return;
        }
#endif //_MTA_BLUE

        // Push onto the to-be-loaded queue
        info->PushIntoLoader( *(CModelLoadInfoSA**)0x008E4C58 );

        // Tell the loader that there is a resource waiting
        (*(DWORD*)VAR_NUMMODELS)++;

        if ( flags & 0x10 )
            Streaming::numPriorityRequests++;

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
        {
            CColModelSAInterface *col = model->pColModel;
            model->pColModel = NULL;

            model->DeleteRwObject();

            model->SetCollision( col, false );
        }
        else
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
            // It cannot be used in MTA:SA anyway, as it is a very small limit in
            // comparison to what we load with Lua.
            break;
        }

        // Model support bugfix: if we have a replacement for this model, we should not
        // bother about management in the CStreaming class, so quit here
        if ( g_replObjectNative[id] )
            return false;

        return true;
    }

    bool __forceinline DoTexDictionary( modelId_t id )
    {
#ifdef MTA_DEBUG
        OutputDebugString( SString( "Deleted texDictionary %u\n", id ) );
#endif

        // Deallocate textures
        TextureManager::GetTxdPool()->Get( id )->Deallocate();
        return true;
    }

    bool __forceinline DoCollision( modelId_t id )
    {
        // Destroy all collisions associated with the COL library
        Streaming::GetCOLEnvironment().UnloadSector( id );
        return true;
    }

    bool __forceinline DoIPL( modelId_t id )
    {
        // This function destroys buildings/IPLs!
        Streaming::GetIPLEnvironment().UnloadSector( id );
        return true;
    }

    bool __forceinline DoPathFind( modelId_t id )
    {
        PathFind::GetInterface().FreeContainer( id );
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

        return true;
    }
};

// Just about the same, but with a different base model logic.
struct ModelAbortBigRequestDispatcher : ModelFreeDispatch
{
    bool DoBaseModel( modelId_t id )
    {
        // To be honest, I have no idea what the correct name is.
        // If you have a clue, contact me (The_GTA).
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
        *(DWORD*)VAR_MEMORYUSAGE -= info->GetSize();
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
                Streaming::numPriorityRequests--;
            }
        }

        // Remove us from any loading queue (garbage collect or to-be-loaded)
        info->PopFromLoader();
    }
    else if ( info->m_eLoading == MODEL_QUEUE )
    {
        // Invalidate any running syncSemaphore requests for this model id
        // Data which has been read from the IMG archive will be ignored.
        for ( unsigned int i = 0; i < MAX_STREAMING_REQUESTERS; i++ )
        {
            streamingRequest& requester = GetStreamingRequest( i );

            for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTS; n++ )
            {
                if ( requester.ids[n] == id )
                    requester.ids[n] = -1;
            }
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
    Streaming::RequestDirectResource

    Arguments:
        model - model id slot to request to
        imgId - .IMG container to request from
        blockCount - size of the requested resource
        imgOffset - offset in the .IMG container
        reqFlags - flags passed to RequestModel
    Purpose:
        Requests a new resource from an .IMG container to
        a model slot. If the resource has already been loaded
        at the given slot, it simply requests it. Otherwise,
        the previous resource is unloaded and then the new
        resource is requested.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040A080
=========================================================*/
void __cdecl Streaming::RequestDirectResource( modelId_t model, unsigned int blockOffset, unsigned int blockCount, unsigned int imgId, unsigned int reqFlags )
{
    CModelLoadInfoSA& info = Streaming::GetModelLoadInfo( model );

    // MTA: check if we cached special resources (like player.img data)
    // If so, use said data for resource lookup.
    void *dataPtr;

    if ( StreamingCache::GetCachedIMGData( imgId, blockOffset, blockCount, dataPtr ) )
    {
        // Terminate anything that was loaded previously as this slot.
        FreeModel( model );

        // Set invalid block data to notify that the IMG data
        // does not originate from an .IMG container.
        info.SetOffset( 0, 0 );

        // Quick load our resources and return.
        LoadModel( dataPtr, model, 0 );
        return;
    }

    unsigned int oldOffset;
    unsigned int oldBlockCount;

    // If the resource already exists at said slot, just request it.
    if ( info.GetOffset( oldOffset, oldBlockCount ) && oldOffset == Streaming::GetFileHandle( imgId, blockOffset ) && oldBlockCount == blockCount )
    {
        RequestModel( model, reqFlags );
        return;
    }

    // Terminate the previous resource instance.
    FreeModel( model );

    // Set new resource information.
    info.SetOffset( blockOffset, blockCount );
    info.m_imgID = imgId;

    // Load new resource information from our .IMG container.
    RequestModel( model, reqFlags );
}

/*=========================================================
    Streaming::RequestSpecialModel

    Arguments:
        model - slot to request the model resource into
        tex - texture dictionary name of the special model
        reqFlags - flags given to RequestModel
    Purpose:
        Loads a new model resource into the model slot during
        runtime. Makes sure that no entity in-game is conflicting
        with the new resource by removing active entities that
        use it from the world.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00409D10
=========================================================*/
struct ClearPedRefs
{
    __forceinline ClearPedRefs( unsigned int model ) : m_model( model )
    { }

    void __forceinline OnEntry( CPedSAInterface *ped, unsigned int index )
    {
        if ( ped->GetModelIndex() == m_model && ped->IsPlayer() )
        {
            
        }
    }

    unsigned int m_model;
};

void __cdecl Streaming::RequestSpecialModel( modelId_t model, const char *tex, unsigned int reqFlags )
{
    // todo.
    ((void (__cdecl*)( modelId_t model, const char *tex, unsigned int reqFlags ))FUNC_CStreaming_RequestSpecialModel)( model, tex, reqFlags );

#if 0
    unsigned int modelHash = pGame->GetKeyGen()->GetUppercaseKey( tex );

    CBaseModelInfoSAInterface *model = ppModelInfo[model];

    if ( model->GetHashKey() == modelHash )
    {
        unsigned int offset, blockCount;

        if ( GetModelLoadInfo( model ).GetOffset( offset, blockCount ) )
        {
            RequestModel( model, reqFlags );
            return;
        }
    }

    // If the model is being used by anything, attempt to clear
    // all references of it.
    if ( model->GetRefCount() != 0 )
    {
        
    }
#endif
}

/*=========================================================
    Streaming::CleanUpLoadQueue

    Purpose:
        Loops through all models requests on the load queue
        and removes the ones which have expired. The removal
        of model requests depends on the flags that have been
        set to the load info.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040C1E0
=========================================================*/
void __cdecl Streaming::CleanUpLoadQueue( void )
{
    CModelLoadInfoSA *iter = GetLastQueuedLoadInfo();

    while ( iter != *(CModelLoadInfoSA**)0x008E4C58 )
    {
        CModelLoadInfoSA *nextInfo = Streaming::GetPrevLoadInfo( iter );

        if ( !IS_ANY_FLAG( iter->m_flags, 0x1E ) )
        {
            Streaming::FreeModel( iter->GetIndex() );
        }

        iter = nextInfo;
    }
}

/*=========================================================
    Streaming::IsInsideStreamingUpdate (MTA extension)

    Purpose:
        Returns a boolean whether the streaming update routine
        has been called and resides on the stack.
=========================================================*/
static bool insideStreamingUpdate = false;

bool Streaming::IsInsideStreamingUpdate( void )
{
    return insideStreamingUpdate;
}

/*=========================================================
    Streaming::SetStreamingEntity (MTA extension)

    Arguments:
        entity - the entity which shall be the streaming focus
    Purpose:
        Sets the active entity that is used for streaming.
        Around this entity the world will load.
=========================================================*/
inline void GetStreamingEntityPosition( CVector& pos )
{
    // MTA fix: if we are inside of the streaming update routine,
    // we should return the center of world.
    if ( World::GetCenterOfWorld( pos ) )
        return;

    // Otherwise attempt to read the position of an active streaming entity.
    CEntitySAInterface *activeStreamingEntity = World::GetStreamingEntity();

    if ( activeStreamingEntity )
    {
        activeStreamingEntity->GetPosition( pos );
        return;
    }

    // Default to player ped position.
    FindPlayerCoords( pos, -1 );
}

/*=========================================================
    Streaming::Update

    Purpose:
        Updates the world streaming status. It requests collision
        data of camera-surrounding entities. Special vehicles
        are requested depending on zone to optimize gameplay.
        Objects which have come into sight but have their models
        not loaded yet are requested.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040E670
=========================================================*/
static bool cleanUpPendingModels = true;    // Binary offsets: (1.0 US and 1.0 EU): 0x008E4CA4

void __cdecl Streaming::Update( void )
{
    // todo: what is this?
    // appears to store the latest number of models loaded.
    *(unsigned int*)0x00B729BC = *(unsigned int*)VAR_NUMMODELS;

    // If the game is paused or the streamer is disabled,
    // we cannot update the streaming status.
    if ( *(bool*)0x00B7CB49 || *(bool*)0x00B7CB48 )
        return;

    // Calculate the height above ground.
    CCameraSAInterface& camera = Camera::GetInterface();
    const CVector& camPos = camera.Placeable.GetPosition();

    float aboveGroundHeight = camPos.fZ - camera.GetGroundLevel( 0 );

    if ( !*(bool*)0x009654B0 && !*(bool*)0x00B76850 )
    {
        if ( aboveGroundHeight < 50.0f )
        {
            if ( *(bool*)0x00B745C1 )
            {
                Streamer::RequestAdvancedSquaredSectorEntities( camPos, 0 );
            }
        }
        else if ( *(unsigned int*)VAR_currArea == 0 )
        {
            Streamer::RequestSquaredSectorEntities( camPos, 0 );
        }
    }

    unsigned int unk = *(unsigned int*)0x00B7CB4C;

    if ( ( unk & 0x7F ) == 106 )
    {
        *(bool*)0x009654BC = false;

        if ( aboveGroundHeight < 500 )
            *(bool*)0x009654BC = PathFind::GetInterface().IsSectorCarNodeInRadius( camPos.fX, camPos.fY, 80.0f );
    }

    CVector playerPos;
    GetStreamingEntityPosition( playerPos );

#ifdef DO_WORLD_STREAMING
    if ( !*(bool*)0x009654B0 && !*(bool*)0x00B5F852 && *(unsigned int*)VAR_currArea == 0 && !*(bool*)0x00A43088 )
    {
        if ( aboveGroundHeight < 50.0f )
        {
            Streaming::StreamPedsAndVehicles( playerPos );

            if ( !Streaming::IsStreamingBusy() )
            {
                ((void (__cdecl*)( void ))0x0040B700)();
                ((void (__cdecl*)( const CVector& pos ))0x0040A560)( playerPos );
            }
        }
    }
#endif //DO_WORLD_STREAMING

    // MTA extension: update fiber timing before going into pulsing the loaders.
    if ( CExecutiveGroupSA *group = fiberGroup )
    {
        // todo
    }

    // Process loading requests.
    Streaming::PulseLoader();

    // Stream buildings and collision files!
    // This is one heavy routine.
    COLEnv_t& COLEnv = Streaming::GetCOLEnvironment();
    IPLEnv_t& IPLEnv = Streaming::GetIPLEnvironment();

    if ( CVehicleSAInterface *remoteVehicle = PlayerInfo::GetInfo( -1 ).pRemoteVehicle )
    {
        const CVector& remotePos = remoteVehicle->Placeable.GetPosition();

        COLEnv.SetLoadPosition( playerPos );
        COLEnv.StreamSectors( remotePos, false );
        COLEnv.PrioritizeLocalStreaming( remotePos );

        IPLEnv.SetLoadPosition( playerPos );
        IPLEnv.StreamSectors( remotePos, false );
        IPLEnv.PrioritizeLocalStreaming( remotePos );
    }
    else
    {
        COLEnv.StreamSectors( playerPos, false );
        COLEnv.PrioritizeLocalStreaming( playerPos );

        IPLEnv.StreamSectors( playerPos, false );
        IPLEnv.PrioritizeLocalStreaming( playerPos );
    }

    if ( cleanUpPendingModels )
        Streaming::CleanUpLoadQueue();
}

// We need to do some work before calling the streaming update function.
// Derived from CMultiplayerSA hacks.
void __cdecl HOOK_CStreaming_Update( void )
{
    // We enter the streaming update, so notify our environment.
    insideStreamingUpdate = true;

    // Enter the precious routine!
    Streaming::Update();

    // We leave, so unset the presency value.
    insideStreamingUpdate = false;
}

/*=========================================================
    CStreamingSA::GetStreamingFocusEntity

    Purpose:
        Returns the entity that streaming is centered around.
        If the entity could not be resolved to a valid MTA
        entity or it is default (i.e. player), NULL is returned.
=========================================================*/
CEntitySA* CStreamingSA::GetStreamingFocusEntity( void )
{
    return pGame->GetPools()->GetEntity( World::GetStreamingEntity() );
}

/*=========================================================
    CStreamingSA::SetWorldStreamingEnabled

    Arguments:
        enabled - boolean to switch on or off
    Purpose:
        Enables or disables the GTA:SA world building
        streaming. If disabled, the GTA:SA engine will stop
        requesting the world around the player. The buildings
        around the player will be deallocated upon disabling.
=========================================================*/
void CStreamingSA::SetWorldStreamingEnabled( bool enabled )
{
    // todo.
}

/*=========================================================
    CStreamingSA::IsWorldStreamingEnabled

    Arguments:
        enabled - boolean to switch on or off
    Purpose:
        Returns whether the world around the player should
        be loaded.
=========================================================*/
bool CStreamingSA::IsWorldStreamingEnabled( void ) const
{
    // todo.
    return true;
}

/*=========================================================
    Streaming::FlushRequestList

    Purpose:
        Loops through the long to-be-loaded queue and aborts
        the loading of all models. Models which reside on the
        queue are free'd.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040E4E0
=========================================================*/
void Streaming::FlushRequestList( void )
{
    CModelLoadInfoSA *info = Streaming::GetQueuedLoadInfo();

    // Bugfix: safety check so we secure against empty load lists.
    if ( info )
    {
        modelId_t modelId = info->GetIndex();
        CModelLoadInfoSA *endItem = *(CModelLoadInfoSA**)0x008E4C54;

        while ( info != endItem )
        {
            // We must save the next parameters as Streaming::FreeModel will tamper
            // the CModelLoadInfoSA structure.
            modelId_t nextModelIndex = info->m_primaryModel;
            CModelLoadInfoSA *nextInfo = Streaming::GetNextLoadInfo( info );

            // Free the resources associated with every model inside the to-be-loaded queue.
            Streaming::FreeModel( modelId );

            // Advance to the next model info.
            modelId = nextModelIndex;
            info = nextInfo;
        }
    }

    // Pulse the streaming requests.
    // Assumingly, this cleans the tubes from remaining requests.
    PulseStreamingRequests();
}

/*=========================================================
    _Streaming_Init

    Purpose:
        Initializes the GTA:SA streaming system. This is the
        heart of the engine.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B8AD0
=========================================================*/
// Initialize the streaming entity garbage collection manager.
// This is the thing that caused the draw distance bug:
// The node list here could only keep 1000 entries, hence only
// 1000 objects, dummies and building could be visible at a time.
// Let us fix that.
// Binary offsets: (1.0 US and 1.0 EU): 0x009654F0
Streaming::streamingEntityChain_t Streaming::gcEntityChain( MAX_DEFAULT_STREAMING_ENTITIES );  // screw the alligators

static void __cdecl _Streaming_Init( void )
{
    new ((void*)ARRAY_CModelLoadInfo) CModelLoadInfoSA[26316];

    *(CModelLoadInfoSA**)ARRAY_ModelLoadCache = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo;

    // Initialize resource linked lists.
    CModelLoadInfoSA& gcResourceLink = *(CModelLoadInfoSA*)0x00965460;
    CModelLoadInfoSA& unkResourceLink = *(CModelLoadInfoSA*)0x00965474;
    CModelLoadInfoSA& toBeLoadedQueue = *(CModelLoadInfoSA*)0x00965488;
    CModelLoadInfoSA& lastModelLoadInfo = *(CModelLoadInfoSA*)0x0096549C;

    *(CModelLoadInfoSA**)0x008E4C60 = &gcResourceLink;
    *(CModelLoadInfoSA**)0x008E4C5C = &unkResourceLink;
    *(CModelLoadInfoSA**)0x008E4C58 = &toBeLoadedQueue;
    *(CModelLoadInfoSA**)0x008E4C54 = &lastModelLoadInfo;

    // Set up the resource connections.
    gcResourceLink.m_primaryModel = 26313;
    gcResourceLink.m_secondaryModel = 0xFFFF;
    unkResourceLink.m_primaryModel = 0xFFFF;
    unkResourceLink.m_secondaryModel = 26312;
    toBeLoadedQueue.m_primaryModel = 26315;
    toBeLoadedQueue.m_secondaryModel = 0xFFFF;
    lastModelLoadInfo.m_primaryModel = 0xFFFF;
    lastModelLoadInfo.m_secondaryModel = 26314;

    // ???
    *(void**)0x008E4B98 = NULL;
    *(void**)0x008E4B94 = NULL;

    // ???
    for ( modelId_t index = 0; index < 8; index++ )
        Streaming::GetModelLoadInfo( index + 374 ).m_eLoading = MODEL_LOADED;

    // Init global streaming status.
    Streaming::biggestResourceBlockCount = 0;
    *(bool*)0x009654B0 = false;
    *(unsigned int*)0x008E4CB4 = 0;
    Streaming::streamingWaitModel = 0xFFFFFFFF;
    Streaming::isLoadingBigModel = false;
    cleanUpPendingModels = true;
    *(bool*)0x008E4B9D = false;
    *(bool*)0x008E4B9C = false;

    // Initialize streaming requesters.
    for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTERS; n++ )
    {
        streamingRequest& requester = Streaming::GetStreamingRequest( n );

        requester.status = streamingRequest::STREAMING_NONE;

        for ( unsigned int i = 0; i < MAX_STREAMING_REQUESTS; i++ )
            requester.ids[i] = 0xFFFFFFF;

        // MTA extension stuff.
        // todo.
        requester.loaderFiber = NULL;
    }

    // Do preparations for resources which prematurely loaded.
    // Initialize all models.
    for ( unsigned int n = 0; n < MAX_MODELS; n++ )
    {
        CBaseModelInfoSAInterface *model = ppModelInfo[n];

        if ( model && model->pRwObject )
        {
            CModelLoadInfoSA& info = Streaming::GetModelLoadInfo( n );

            info.m_flags = 2;
            info.m_eLoading = MODEL_LOADED;
        }
    }

    // Initialize all textures.
    for ( unsigned int n = 0; n < MAX_TXD; n++ )
    {
        CTxdInstanceSA *inst = TextureManager::GetTxdPool()->Get( n );

        if ( inst && inst->m_txd )
            Streaming::GetModelLoadInfo( DATA_TEXTURE_BLOCK, n ).m_eLoading = MODEL_LOADED;
    }

    // Initialize the imfamous vehicleModelCache.
    new ((void*)0x008E4C24) ModelIdContainer;

    // This is a list of recently loaded ped models.
    // Rockstar thought this list be pretty short.
    for ( unsigned int n = 0; n < 8; n++ )
        *((unsigned int*)ARRAY_PEDSPECMODEL + n) = 0xFFFFFFFF;

    *(unsigned int*)0x008E4BB0 = 0; // the count of active ped models

    // ???
    for ( unsigned int n = 0; n < 18; n++ )
        *((unsigned int*)0x008E4BB8 + n) = 0;

    // Initialize the container where all missing model infos during loading
    // of the archives are stored. These resources can still be loaded through
    // RequestSpecialModel.
    *(Streaming::CMissingModelInfoSA**)0x008E48D0 = new Streaming::CMissingModelInfoSA( 550 );

    *(unsigned int*)0x008E4C20 = 0xFFFFFFFF;
    *(unsigned short*)0x008E4BAC = 0;
    *(unsigned int*)0x008E4BA4 = 0xFFFFFFFF;
    Streaming::numPriorityRequests = 0;

#if 0
    // Initialize the script manager.
    __asm
    {
        mov ecx,0x00A47B60
        mov eax,0x00470660
        call eax
    }
#endif

    // Load the streaming archives.
    Streaming::LoadArchives();

    // By now the biggest block count should be initialized properly.
    // Make sure it is divisible through the number of slicers.
    int biggestBlockCount = Streaming::biggestResourceBlockCount;

    {
        int remainder = biggestBlockCount % MAX_STREAMING_REQUESTERS;

        if ( remainder != 0 )
            biggestBlockCount += MAX_STREAMING_REQUESTERS - remainder;
    }

    // Allocate the buffer that is used to read .IMG chunks into.
    void *buf = RwAllocAligned( biggestBlockCount * 2048, 2048 );

    // Divide the biggest block count by the number of streaming requesters.
    biggestBlockCount /= MAX_STREAMING_REQUESTERS;
    
    Streaming::biggestResourceBlockCount = biggestBlockCount;

    // Set up the sections of the allocation into the global streaming buffers,
    // each for every streaming requester.
    for ( unsigned int n = 0; n < MAX_STREAMING_REQUESTERS; n++ )
        Streaming::threadAllocationBuffers[n] = (void*)( (char*)buf + n * biggestBlockCount * 2048 );

    // Set streaming globals to defaults.
    // These can be modified using stream.ini
    *(unsigned int*)0x008A5A80 = 0x3200000; // max streaming memory
    *(unsigned int*)0x008A5A84 = 22;        // max loaded vehicle models, close to the maximum due to limitations.

    // Here was the initialization of the native-scope streaming gc chain.

    // MTA extension: enter the current streaming mode.
    Streaming::EnterFiberMode();

    // Set up fibered business.
    Streaming::SetLoaderPerfMultiplier( STREAMING_DEFAULT_FIBERED_PERFMULT );

    // We successfully loaded.
    *(bool*)0x009654B8 = true;
}

/*=========================================================
    _Streaming_Shutdown

    Purpose:
        Deallocates all resources used by the streaming system
        internally.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004084B0
=========================================================*/
static void __cdecl _Streaming_Shutdown( void )
{
    // Leave streaming mode to deallocate resources.
    Streaming::LeaveFiberMode();

    // Deallocate the threading resource memory.
    RwFreeAligned( Streaming::threadAllocationBuffers[0] );

    // Set the threading buffer size to zero blocks.
    Streaming::biggestResourceBlockCount = 0;

    // Destroy the stack holding all missing model ids.
    delete *(Streaming::CMissingModelInfoSA**)0x008E48D0;
}

void Streaming::Reset( void )
{
    // Reset to GTA:SA defaults.
    // Could have been modified by mods.
    ResetGarbageCollection();
    
    EnableFiberedLoading( STREAMING_DEFAULT_FIBERED_LOADING );

    SetLoaderPerfMultiplier( STREAMING_DEFAULT_FIBERED_PERFMULT );
}

CStreamingSA::CStreamingSA( void )
{
    // Initialize the accelerated streaming structures
    memset( g_colReplacement, 0, sizeof(g_colReplacement) );
    memset( g_originalCollision, 0, sizeof(g_originalCollision) );

    // Hook the model requester
    HookInstall( 0x005B8AD0, (DWORD)_Streaming_Init, 5 );
    HookInstall( 0x004084B0, (DWORD)_Streaming_Shutdown, 5 );
    HookInstall( FUNC_CStreaming__RequestModel, (DWORD)Streaming::RequestModel, 6 );
    HookInstall( 0x004089A0, (DWORD)Streaming::FreeModel, 6 );
    HookInstall( 0x0040A080, (DWORD)Streaming::RequestDirectResource, 5 );
    HookInstall( 0x0040C6B0, (DWORD)LoadModel, 5 );
    HookInstall( 0x0040E670, (DWORD)HOOK_CStreaming_Update, 5 );
    //HookInstall( 0x00407AD0, (DWORD)CheckAnimDependency, 5 );
    //HookInstall( 0x00409A90, (DWORD)CheckTXDDependency, 5 );  // you better do not fuck around with securom
    
    HookInstall( FUNC_LoadAllRequestedModels, (DWORD)Streaming::LoadAllRequestedModels, 5 );
    HookInstall( 0x00408E20, (DWORD)ProcessLoadQueue, 5 );
    HookInstall( 0x0040E170, (DWORD)ProcessStreamingRequest, 5 );
    HookInstall( 0x0040CBA0, (DWORD)PulseStreamingRequest, 5 );
    HookInstall( 0x0040E460, (DWORD)PulseStreamingRequests, 5 );

    // Initialize sub modules
    StreamingUtils_Init();
    StreamingLoader_Init();
    StreamingRuntime_Init();
    StreamingResources_Init();
    StreamingCache_Init();
    StreamingIPL_Init();
    StreamingCOL_Init();
}

CStreamingSA::~CStreamingSA( void )
{
    // Shutdown sub modules
    StreamingCOL_Shutdown();
    StreamingIPL_Shutdown();
    StreamingCache_Shutdown();
    StreamingResources_Shutdown();
    StreamingRuntime_Shutdown();
    StreamingLoader_Shutdown();
    StreamingUtils_Shutdown();
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
        debugLoc - string containing debugging information
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
    Streaming::RequestSpecialModel( model, tex, channel );
}

void CStreamingSA::SetRequestCallback( streamingRequestCallback_t callback )
{
    streamingRequestCallback = callback;
}

void CStreamingSA::SetFreeCallback( streamingFreeCallback_t callback  )
{
    streamingFreeCallback = callback;
}

void CStreamingSA::GetStreamingInfo( CStreaming::streamingInfo& info ) const
{
    info.usedMemory = *(DWORD*)VAR_MEMORYUSAGE;
    info.maxMemory = *(size_t*)0x008A5A80;
    info.numberOfRequests = *(DWORD*)VAR_NUMMODELS;
    info.numberOfPriorityRequests = Streaming::numPriorityRequests;
    info.numberOfSlicers = MAX_STREAMING_REQUESTERS;
    info.numberOfRequestsPerSlicer = MAX_STREAMING_REQUESTS;
    info.activeStreamingThread = Streaming::activeStreamingThread;
    info.isBusy = Streaming::IsStreamingBusy();
    info.isLoadingBigModel = Streaming::isLoadingBigModel;
}