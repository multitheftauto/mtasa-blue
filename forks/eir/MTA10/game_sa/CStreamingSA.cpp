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

#define ARRAY_MODELIDS          0x008E4A60  // unsure
#define ARRAY_LODMODELIDS       0x008E4AF8  // unsure

#define FLAG_PRIORITY           0x10

static streamingRequestCallback_t streamingRequestCallback = NULL;
static streamingFreeCallback_t streamingFreeCallback = NULL;

/*=========================================================
    HOOK_CStreaming__RequestModel

    Arguments:
        id - model info index to request
        flags - priority flags for this request
    Purpose:
        Hook so that every engine resource request is handled
        through MTA code.
    Binary offsets:
        (see CStreamingSA::RequestModel)
=========================================================*/
static void __cdecl HOOK_CStreaming__RequestModel( unsigned int id, unsigned int flags )
{
    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;

    if ( id > MAX_MODELS-1 )
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
                    if ( CColModelSA *col = g_colReplacement[id] )
                        col->Apply( id );

                    ((CClumpModelInfoSAInterface*)minfo)->SetClump( RpClumpClone( (RpClump*)obj->GetObject() ) );
                    break;
                }

                info->m_eLoading = MODEL_LOADED;
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
        if ( info->m_primaryModel == 0xFFFF )
            return;

        // Unfold loaded model
        info->PopFromLoader();

        if ( id < DATA_TEXTURE_BLOCK )
        {
            CBaseModelInfoSAInterface *model = ppModelInfo[id];

            switch( model->GetModelType() )
            {
            case MODEL_VEHICLE:
            case MODEL_PED:
                return;
            }
        }

        if ( !( info->m_flags & (0x02 | 0x04) ) )
            info->PushIntoLoader( *(CModelLoadInfoSA**)0x008E4C60 );

        return;
    }

    // Make sure we are really unloaded?
    switch( info->m_eLoading )
    {
    case MODEL_LOADING:
    case MODEL_LOD:
    case MODEL_RELOAD:
        // We are doing the job. No need to change stuff.
        return;
    case MODEL_LOADED:
    default:
        // We want to load again. The system should know that
        // the resource is ready.
        // The resource probably resides in the loader arrays
        // and is dynamically managed.
        goto reload;
    case MODEL_UNAVAILABLE:
        // This resource has to be instantiated to be ready again.
        // Proceed with the request here.
        break;
    }

    // Perform actions depending on request type
    if ( id < DATA_TEXTURE_BLOCK )
    {
        CBaseModelInfoSAInterface *model = ppModelInfo[id];
        int animIndex = model->GetAnimFileIndex();

        // Request the models textures
        HOOK_CStreaming__RequestModel( model->usTextureDictionary + DATA_TEXTURE_BLOCK, flags );

        // Get animation if necessary
        if ( animIndex != -1 )
            HOOK_CStreaming__RequestModel( animIndex + DATA_ANIM_BLOCK, 0x08 );
    }
    else if ( id < DATA_TEXTURE_BLOCK + MAX_TXD )
    {
        CTxdInstanceSA *txd = (*ppTxdPool)->Get( id - DATA_TEXTURE_BLOCK );

        // Crashfix
        if ( !txd )
            return;

#ifdef MTA_DEBUG
        OutputDebugString( SString( "loaded texDictionary %u\n", id - DATA_TEXTURE_BLOCK ) );
#endif

        // I think it loads textures, lol
        if ( txd->m_parentTxd != 0xFFFF )
            HOOK_CStreaming__RequestModel( txd->m_parentTxd + DATA_TEXTURE_BLOCK, flags );
    }

    // Push onto the to-be-loaded queue
    info->PushIntoLoader( *(CModelLoadInfoSA**)0x008E4C58 );

    // Tell the loader that there is resource waiting
    (*(DWORD*)VAR_NUMMODELS)++;

    if ( flags & 0x10 )
        (*(DWORD*)VAR_NUMPRIOMODELS)++;

reload:
    // If available, we reload the model
    info->m_flags = flags;

    info->m_eLoading = MODEL_LOADING;
}

/*=========================================================
    HOOK_CStreaming__FreeModel

    Arguments:
        id - model info index to request
        flags - priority flags for this request
    Purpose:
        Hook so that every engine resource request is handled
        through MTA code.
    Binary offsets:
        (see CStreamingSA::FreeModel)
=========================================================*/
static void __cdecl HOOK_CStreaming__FreeModel( unsigned int id )
{
    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;

    if ( id > MAX_MODELS-1 )
        return;

    // Unavailable resources do not need termination
    if ( info->m_eLoading == MODEL_UNAVAILABLE )
        return;

    if ( info->m_eLoading == MODEL_LOADED )
    {
        if ( id < DATA_TEXTURE_BLOCK )
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

                ((void (__cdecl*)( unsigned int ))0x004080F0)( id );

                break;
            }

#ifdef RENDERWARE_VIRTUAL_INTERFACES
            // Model support bugfix: if we have a replacement for this model, we should not
            // bother about management in the CStreaming class, so quit here
            if ( g_replObjectNative[id] )
                goto customJump;
#endif //RENDERWARE_VIRTUAL_INTERFACES
        }
        else if ( id < DATA_TEXTURE_BLOCK + MAX_TXD )
        {
#ifdef MTA_DEBUG
            OutputDebugString( SString( "Deleted texDictionary %u\n", id - DATA_TEXTURE_BLOCK ) );
#endif

            // Deallocate textures
            (*ppTxdPool)->Get( id - DATA_TEXTURE_BLOCK )->Deallocate();
        }
        else if ( id < 25255 )
        {
            // Destroy all collisions associated with the COL library
            FreeCOLLibrary( id - 25000 );
        }
        else if ( id < 25511 )
        {
            // This function destroys buildings/IPLs!
            __asm
            {
                mov eax,id
                sub eax,25255

                push eax
                mov eax,0x00404B20
                call eax
                pop eax
            }
        }
        else if ( id < DATA_ANIM_BLOCK )    // path finding...?
        {
            __asm
            {
                mov eax,id
                sub eax,25511

                push eax
                mov ecx,CLASS_CPathFind
                mov eax,0x0044D0F0
                call eax
            }
        }
        else if ( id < 25755 )
        {
            // Animations...?
            pGame->GetAnimManager()->RemoveAnimBlock( id - DATA_ANIM_BLOCK );
        }
        else if ( id >= 26230 )
        {
            __asm
            {
                mov eax,id
                sub eax,26230

                mov ecx,0x00A47B60
                push eax
                mov eax,0x004708E0
                call eax
            }
        }

        *(DWORD*)VAR_MEMORYUSAGE -= info->m_blockCount * 2048;
    }

#ifdef RENDERWARE_VIRTUAL_INTERFACES
customJump:
#endif //RENDERWARE_VIRTUAL_INTERFACES
    if ( info->m_primaryModel != 0xFFFF )
    {
        if ( info->m_eLoading == MODEL_LOADING )
        {
            (*(DWORD*)VAR_NUMMODELS)--;

            if ( info->m_flags & FLAG_PRIORITY )
            {
                info->m_flags &= ~FLAG_PRIORITY;

                (*(DWORD*)VAR_NUMPRIOMODELS)--;
            }
        }

        // Remove us from loading queue
        info->PopFromLoader();
    }
    else if ( info->m_eLoading == MODEL_LOD )
    {
        // Unknown
        for ( unsigned int n = 0; n < 30; n++ )
        {
            if (*((int*)ARRAY_MODELIDS + n) == (int)id)
                *((int*)ARRAY_MODELIDS + n) = -1;

            if (*((int*)ARRAY_LODMODELIDS + n) == (int)id)
                *((int*)ARRAY_LODMODELIDS + n) = -1;
        }
    }
    else if ( info->m_eLoading == MODEL_RELOAD )
    {
        // Abort an ongoing/flattened-out loading process
        if ( id < DATA_TEXTURE_BLOCK )
            RwFlushLoader();
        else if ( id < DATA_TEXTURE_BLOCK + MAX_TXD )
            (*ppTxdPool)->Get( id - DATA_TEXTURE_BLOCK )->Deallocate();
        else if ( id < 25255 )
            FreeCOLLibrary( id - 25000 );
        else if ( id < 25511 )
            ( (void (*)( unsigned int model ))0x00404B20 )( id - 25255 );
        else if ( id >= DATA_ANIM_BLOCK && id < 25755 )
            pGame->GetAnimManager()->RemoveAnimBlock( id - DATA_ANIM_BLOCK );
        else if ( id >= 26230 )
            ( (void (__stdcall*)( unsigned int model ))0x004708E0 )( id - 26230 );
    }

    // Mark that all resources have been terminated.
    info->m_eLoading = MODEL_UNAVAILABLE;

    // Notify user environments
    if ( streamingFreeCallback )
        streamingFreeCallback( id );
}

CStreamingSA::CStreamingSA( void )
{
    // Initialize the accelerated streaming structures
    memset( g_colReplacement, 0, sizeof(g_colReplacement) );
    memset( g_originalCollision, 0, sizeof(g_originalCollision) );

    // Hook the model requester
    HookInstall( FUNC_CStreaming__RequestModel, (DWORD)HOOK_CStreaming__RequestModel, 6 );
    HookInstall( 0x004089A0, (DWORD)HOOK_CStreaming__FreeModel, 6 );
    HookInstall( 0x00410730, (DWORD)FreeCOLLibrary, 5 );
    HookInstall( 0x0040C6B0, (DWORD)LoadModel, 5 );

    StreamingLoader_Init();
}

CStreamingSA::~CStreamingSA( void )
{
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
inline static bool IsUpgradeModelId( unsigned short dwModelID )
{
    return dwModelID >= 1000 && dwModelID <= 1193;
}

void CStreamingSA::RequestModel( unsigned short id, unsigned int flags )
{
    if ( IsUpgradeModelId( id ) )
        RequestVehicleUpgrade( id, flags );
    else
        HOOK_CStreaming__RequestModel( id, flags );
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
void CStreamingSA::FreeModel( unsigned short id )
{
    HOOK_CStreaming__FreeModel( id );
}

/*=========================================================
    CStreamingSA::LoadAllRequestedModels

    Arguments:
        onlyPriority - appears to favour prioritized models if true
    Purpose:
        Cycles through the streaming loading system to process
        loader queues (load and termination requests).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0040EA10
=========================================================*/
void CStreamingSA::LoadAllRequestedModels( bool onlyPriority )
{
    DWORD dwFunction = FUNC_LoadAllRequestedModels;
    _asm
    {
        movzx   eax,onlyPriority
        push    eax
        call    dwFunction
        add     esp, 4
    }
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
bool CStreamingSA::HasModelLoaded( unsigned int id )
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
bool CStreamingSA::IsModelLoading( unsigned int id )
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
void CStreamingSA::WaitForModel( unsigned int id )
{
    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;

    if ( id > MAX_MODELS-1 )
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
    RequestModel( idx, flags );
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
void CStreamingSA::RequestVehicleUpgrade( unsigned short model, unsigned int flags )
{
    DWORD dwFunc = FUNC_RequestVehicleUpgrade;
    _asm
    {
        mov     eax,flags
        push    eax
        movzx   eax,model
        push    eax
        call    dwFunc
        add     esp, 8
    }
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
void CStreamingSA::RequestSpecialModel( unsigned short model, const char *tex, unsigned int channel )
{
    DWORD dwFunc = FUNC_CStreaming_RequestSpecialModel;
    _asm
    {
        mov     eax,channel
        push    eax
        push    tex
        movzx   eax,model
        push    eax
        call    dwFunc
        add     esp, 0xC
    }
}

void CStreamingSA::SetRequestCallback( streamingRequestCallback_t callback )
{
    streamingRequestCallback = callback;
}

void CStreamingSA::SetFreeCallback( streamingFreeCallback_t callback  )
{
    streamingFreeCallback = callback;
}