/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.render.cpp
*  PURPOSE:     Entity render framework
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CBaseModelInfoSAInterface **ppModelInfo;

namespace EntityRender
{
    // Render chains used for sorted execution of instances.
    atomicRenderChain_t boatRenderChain( 50 );                          // Binary offsets: (1.0 US and 1.0 EU): 0x00C880C8; boatRenderChains, orig 20
    entityRenderChain_t defaultEntityRenderChain( 8000 );               // Binary offsets: (1.0 US and 1.0 EU): 0x00C88120; ???, orig 200
    entityRenderChain_t underwaterEntityRenderChain( 4000 );            // Binary offsets: (1.0 US and 1.0 EU): 0x00C88178; ???, orig 100, used to render underwater entities
    entityRenderChain_t alphaEntityRenderChain( 50 );                   // Binary offsets: (1.0 US and 1.0 EU): 0x00C881D0; ???, orig 50, used for "grasshouse" model

    // Runtime variables.
    bool blockAutomaticModelLoadingInVision = false;                    // Binary offsets: (1.0 US and 1.0 EU): 0x00B76851

    bool DoBlockModelVisionLoading( void )
    {
        //return blockAutomaticModelLoadingInVision;
        return false;   // disabled because MTA:SA is not compatible with it.
    }
}

/*=========================================================
    HOOK_InitRenderChains

    Purpose:
        Sets up native rendering chains that are used to sort
        the entities that should be displayed on the game world.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733A20
=========================================================*/
void __cdecl HOOK_InitRenderChains( void )
{
    // Removed chain initializations that we localized (see above).
    new ((void*)0x00C88224) pedRenderChain_t( 8000 );                   // ???, orig 100
}

// Include internal definitions.
#include "CEntitySA.render.hxx"

/*=========================================================
    RenderEntity

    Arguments:
        entity - the entity to render on the scene right now
    Purpose:
        Renders the given entity on the game world. This
        function is being called by GTA:SA to render
        non-fading entities. Vehicles are handled specially by
        temporarily modifying their materials to the vehicle's
        properties (color, etc).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00553260
    Note:
        MTA fixes have been added to this function. It now
        properly handles vehicle alpha rendering.
=========================================================*/
inline void RenderEntityNative( CEntitySAInterface *entity )
{
    using namespace EntityRender;

    // Reference the entity here, for safety.
    ReferenceEntityForRendering( entity );

    if ( AreScreenEffectsEnabled() )
    {
        NormalizeDayNight();

        entity->Render();

        RestoreDayNight();
    }
    else
        entity->Render();
}

bool CanVehicleRenderNatively( void )
{
    bool globalDoAlphaFix, globalRenderOpaque, globalRenderTranslucent, globalRenderDepth;

    globalDoAlphaFix = RenderCallbacks::IsAlphaSortingEnabled();

    RenderCallbacks::GetAlphaSortingParams( globalRenderOpaque, globalRenderTranslucent, globalRenderDepth );

    return !globalDoAlphaFix || globalRenderOpaque;
}

inline bool PedHasJetpack( CPedSAInterface *pedEntity )
{
    CPedSA *mtaPed = pGame->GetPools()->GetPed( pedEntity );

    if ( mtaPed )
    {
        CTask *primaryTask = mtaPed->GetPedIntelligence()->GetTaskManager()->GetSimplestActiveTask();

        if ( primaryTask && primaryTask->GetTaskType() == TASK_SIMPLE_JETPACK )
        {
            return true;
        }
    }

    return false;
}

void EntityRender_Global( CEntitySAInterface *entity, unsigned int renderAlpha )
{
    bool doRender = true;

    if ( entity->nType == ENTITY_TYPE_VEHICLE )
    {
        if ( !CanVehicleRenderNatively() )
        {
            // Special clump rendering.
            ((CVehicleSAInterface*)entity)->GetRwObject()->Render();
            
            doRender = false;
        }
    }
    else if ( entity->nType == ENTITY_TYPE_PED )
    {
        CPedSAInterface *pedEntity = (CPedSAInterface*)entity;

        bool doAlphaFix = RenderCallbacks::IsAlphaSortingEnabled();

        if ( doAlphaFix && renderAlpha != 255 )
        {
            bool doOpaque, doTranslucent, doDepth;

            RenderCallbacks::GetAlphaSortingParams( doOpaque, doTranslucent, doDepth );

            // Bugfix for Jetpack.
            if ( PedHasJetpack( pedEntity ) || pedEntity->IsDrivingVehicle() )
            {
                doRender = doOpaque;
            }
            else
            {
                doRender = doTranslucent;
            }
        }
    }
    
    if ( doRender )
    {
        RenderEntityNative( entity );
    }
}

void __cdecl _RenderEntity( CEntitySAInterface *entity )
{
    using namespace EntityRender;

    // Do not render peds in the game world if they are inside a vehicle
    // (they need a special render pass to prevent drawing errors)
    if ( entity->nType == ENTITY_TYPE_PED && ((CPedSAInterface*)entity)->IsDrivingVehicle() )
        return;

    unsigned char id = entity->SetupLighting();

    // This way we skip heap allocations
    StaticAllocator <RwRenderStateLock, 1> rsAlloc;
    RwRenderStateLock *alphaRef;

    // Grab the MTA entity
    CEntitySA *mtaEntity = pGame->GetPools()->GetEntity( entity );

    unsigned char alpha = 255;

    if ( mtaEntity )
    {
        // Perform alpha calculations
        if ( entity->nType == ENTITY_TYPE_VEHICLE )
        {
            alpha = ((CVehicleSA*)mtaEntity)->GetAlpha();
        }
        else if ( entity->nType == ENTITY_TYPE_OBJECT )
        {
            alpha = ((CObjectSA*)mtaEntity)->GetAlpha();
        }
        else if ( entity->nType == ENTITY_TYPE_PED )
        {
            alpha = ((CPedSA*)mtaEntity)->GetAlpha();
        }
    }
    else if ( entity->GetRwObject() && entity->GetRwObject()->type == RW_CLUMP )    // the entity may not have a RenderWare object?!
    {
        // This value will be used for internal GTA:SA vehicles.
        // We might aswell use this alpha value of the clump.
        alpha = (unsigned char)RpClumpGetAlpha( (RpClump*)entity->GetRwObject() );
    }

    // Set the rendering entity.
    EntityRender::SetRenderingEntity( mtaEntity );

    if ( alpha != 255 )
    {
        // This has to stay enabled anyway
        HOOK_RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE, true );

        // Ensure the RenderStates necessary for proper alpha blending
        alphaRef = new (rsAlloc.Allocate()) RwRenderStateLock( D3DRS_ALPHAREF, 0x00 );
    }

    bool doRender = true;

    // Prepare entity for rendering/enter frame
    if ( entity->nType == ENTITY_TYPE_VEHICLE )
    {
        // Set up global variables for fast rendering
        CacheVehicleRenderCameraSettings( alpha, entity->GetRwObject() );

        // Clear previous vehicle atomics from render chains
        ClearVehicleRenderChains();

        CVehicleSAInterface *veh = (CVehicleSAInterface*)entity;

        // Render the passengers
        veh->RenderPassengers();

        // Prepare vehicle rendering (i.e. texture replacement)
        veh->SetupRender( (CVehicleSA*)mtaEntity );
    }
    else if ( !( entity->m_entityFlags & ENTITY_BACKFACECULL ) )
    {
        // Change texture stage
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)20, 1 );
    }

    if ( AreScreenEffectsEnabled() )
    {
        if ( *(bool*)0x00C402B8 )
            SetupNightVisionLighting();

        if ( *(bool*)0x00C402B9 )
            SetupInfraRedLighting();
    }

    // Special fix for vehicles, since they are complex entities.
    EntityRender_Global( entity, alpha );

    // Restore values and unset entity rendering status/leave frame
    if ( entity->nType == ENTITY_TYPE_VEHICLE )
    {
        BOOL_FLAG( entity->m_entityFlags, ENTITY_RENDERING, true );

        // Render the delayed atomics
        ExecuteVehicleRenderChains( alpha );

        BOOL_FLAG( entity->m_entityFlags, ENTITY_RENDERING, false );

        CVehicleSAInterface *veh = (CVehicleSAInterface*)entity;

        // Leave rendering stage
        veh->LeaveRender();
    }   
    else if ( !( entity->m_entityFlags & ENTITY_BACKFACECULL ) )
    {
        // Set texture stage back to two
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)20, 2 );
    }

    // Does the entity have alpha enabled?
    if ( alpha != 255 )
    {
        // Remove the RenderState locks
        alphaRef->~RwRenderStateLock();
    }

    // Remove the rendering entity.
    EntityRender::SetRenderingEntity( NULL );

    entity->RemoveLighting( id );
}

// Wiring in some MTA team fixes.
void __cdecl RenderEntity( CEntitySAInterface *entity )
{
#ifdef _MTA_BLUE
    // FIX BEGIN
    OnMY_CEntity_RenderOneNonRoad_Pre( entity );
#endif //_MTA_BLUE

    // Call the actual method.
    _RenderEntity( entity );

#ifdef _MTA_BLUE
    // FIX END
    OnMY_CEntity_RenderOneNonRoad_Post( entity );
#endif //_MTA_BLUE
}

/*=========================================================
    RenderAtomicWithAlpha

    Arguments:
        info - the model info associated with the atomic
        atom - the RenderWare atomic to render with alpha
        alpha - the alpha value to render the atomic with
    Purpose:
        Renders an atomic with a specific alpha value. This
        effect is used to give atomics fading effects.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732610
=========================================================*/
inline void InitModelRendering( CBaseModelInfoSAInterface *info )
{
    if ( info->renderFlags & RENDER_ADDITIVE )
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)11, 2 );
}

inline void ShutdownModelRendering( CBaseModelInfoSAInterface *info )
{
    if ( info->renderFlags & RENDER_ADDITIVE )
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)11, 6 );
}

static void __cdecl RenderAtomicWithAlpha( CBaseModelInfoSAInterface *info, RpAtomic *atom, unsigned int alpha )
{
    InitModelRendering( info );

    RpAtomicRenderAlpha( atom, alpha );

    ShutdownModelRendering( info );
}

/*=========================================================
    RenderClumpWithAlpha

    Arguments:
        info - the model info associated with the atomic
        atom - the RenderWare atomic to render with alpha
        alpha - the alpha value to render the atomic with
    Purpose:
        Renders a clump with a specific alpha value. This
        effect is used to give atomics fading effects.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732680
=========================================================*/
static void __cdecl RenderClumpWithAlpha( CBaseModelInfoSAInterface *info, RpClump *clump, unsigned int alpha )
{
    InitModelRendering( info );

    LIST_FOREACH_BEGIN( RpAtomic, clump->atomics.root, atomics )
        if ( item->IsVisible() )
            RpAtomicRenderAlpha( item, alpha );
    LIST_FOREACH_END

    ShutdownModelRendering( info );
}

/*=========================================================
    EntityRender::DefaultRenderEntityHandler

    Arguments:
        entity - the entity to render right now
        camDistance - camera distance of the given entity
    Purpose:
        Processes a sorted rendering request of an entity. The
        given entity is expected to be sorted from back to front.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732B40
=========================================================*/
void __cdecl EntityRender::DefaultRenderEntityHandler( CEntitySAInterface *entity, float camDistance )
{
    RwObject *rwobj = entity->GetRwObject();

    if ( !rwobj )
        return;

    CBaseModelInfoSAInterface *info = entity->GetModelInfo();

    if ( info->renderFlags & RENDER_NOSHADOW )
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)8, 0 );

    if ( entity->m_entityFlags & ENTITY_FADE )
    {
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)30, 0 );

        // Make sure we keep this entity alive till next render cycle.
        // This is to make sure that we do not crash due to deep render links.
        ReferenceEntityForRendering( entity );

        unsigned int alpha = (unsigned char)CalculateFadingAlpha( info, entity, camDistance, Streamer::GetWorldFarclip() );

        unsigned int flags = entity->m_entityFlags;
        flags |= ENTITY_RENDERING;

        entity->m_entityFlags = flags;

        if ( flags & ENTITY_BACKFACECULL )
            RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)20, 1 );

        unsigned char lightIndex = entity->SetupLighting();

        if ( rwobj->type == RW_ATOMIC )
            RenderAtomicWithAlpha( info, (RpAtomic*)rwobj, alpha );
        else
            RenderClumpWithAlpha( info, (RpClump*)rwobj, alpha );

        entity->RemoveLighting( lightIndex );

        flags &= ~ENTITY_RENDERING;
        entity->m_entityFlags = flags;

        if ( flags & ENTITY_BACKFACECULL )
            RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)20, 2 );

        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)30, 100 );
    }
    else
    {
        if ( !*(unsigned int*)VAR_currArea && !( info->renderFlags & RENDER_NOSHADOW ) )
            RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)30, 100 );
        else
            RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)30, 0 );

        RenderEntity( entity );
    }

    if ( info->renderFlags & RENDER_NOSHADOW )
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)8, 1 );
}

// System callbacks to notify the mods about important progress.
static gameEntityPreRenderCallback_t _preRenderCallback = NULL;
static gameEntityRenderCallback_t _renderCallback = NULL;
static gameEntityRenderUnderwaterCallback_t _renderUnderwaterCallback = NULL;
static gameEntityPostProcessCallback_t _renderPostProcessCallback = NULL;

void Entity::SetPreRenderCallback( gameEntityPreRenderCallback_t callback )                 { _preRenderCallback = callback; }
void Entity::SetRenderCallback( gameEntityRenderCallback_t callback )                       { _renderCallback = callback; }
void Entity::SetRenderUnderwaterCallback( gameEntityRenderUnderwaterCallback_t callback )   { _renderUnderwaterCallback = callback; }
void Entity::SetRenderPostProcessCallback( gameEntityPostProcessCallback_t callback )       { _renderPostProcessCallback = callback; }

/*=========================================================
    PreRender

    Purpose:
        Prepares all entities which are added to the rendering
        queues for rendering by calling their "PreRender"
        method.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00553910
=========================================================*/
struct SetupDefaultEntitiesForRender
{
    bool __forceinline OnEntry( unorderedEntityRenderChainInfo& info )
    {
        info.entity->PreRender();
        return true;
    }
};

struct SetupChainEntitiesForRender
{
    bool __forceinline OnEntry( const entityRenderInfo& info )
    {
        if ( info.callback == EntityRender::DefaultRenderEntityHandler )
        {
            CEntitySAInterface *entity = info.entity;

            entity->m_entityFlags &= ~ENTITY_OFFSCREEN;

            entity->PreRender();
        }

        return true;
    }
};

void __cdecl PreRender( void )
{
    groundAlphaEntities.ExecuteCustom( SetupDefaultEntitiesForRender() );
    staticRenderEntities.ExecuteCustom( SetupDefaultEntitiesForRender() );

    // Left out unused render list.
    // Probably left in for Rockstars internal development tools (like map editor).

    // We notify our system.
    if ( _preRenderCallback )
        _preRenderCallback();

    lowPriorityRenderEntities.ExecuteCustom( SetupDefaultEntitiesForRender() );

    EntityRender::GetDefaultEntityRenderChain().ExecuteCustom( SetupChainEntitiesForRender() );
    EntityRender::GetUnderwaterEntityRenderChain().ExecuteCustom( SetupChainEntitiesForRender() );

    ((void (__cdecl*)( void ))0x00707FA0)();
}

/*=========================================================
    SetupWorldRender

    Purpose:
        Called before any entity rendering has taken place.
        This function sets up properties like rendering
        farclip and entity specific LOD distance. It clears
        all rendering queues so they can be filled with new
        sorted entities.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005556E0
=========================================================*/
void DebugMethod( void )
{
    // Debug an issue I have experienced where MTA entities would lose their RenderWare object
    // although they are used by the game.
    for ( unsigned int n = 0; n < MAX_OBJECTS; n++ )
    {
        CObjectSA *object = mtaObjects[n];

        if ( object )
        {
            CObjectSAInterface *realObj = (CObjectSAInterface*)object->GetInterface();

            if ( !realObj->GetRwObject() )
            {
                realObj->CreateRwObject();

                RwObject *rwobj = realObj->GetRwObject();

                if ( !rwobj )
                    __asm nop
            }
        }
    }
}

void __cdecl SetupWorldRender( void )
{
    using namespace EntityRender;

    // DEBUG
    RwD3D9ValidateDeviceStates();

    DebugMethod();

    CCameraSAInterface& camera = Camera::GetInterface();

    const CVector& camPos = camera.Placeable.GetPosition();

    unsigned int unkResult = ((unsigned int (__cdecl*)( CVector camPos ))0x0072D9F0)( camPos );

    *(bool*)0x00B745C0 = IS_ANY_FLAG( unkResult, 0x880 );

    if ( !IS_ANY_FLAG( unkResult, 0x80 ) )
    {
        *(bool*)0x00B745C1 = !IS_ANY_FLAG( unkResult, 0x8 );
    }
    else
        *(bool*)0x00B745C1 = true;

    *(float*)0x008CD804 = 1.0f;

    // This variable decides whether the runtime may load models that go into vision.
    // It is really weird and was probably made for optimizations.
    EntityRender::blockAutomaticModelLoadingInVision = false;

    // Calculate big entity LOD scale.
    float newBigScale = *(float*)0x008CD804;

    CPlayerPedSAInterface *ped = GetPlayerPed( -1 );

    if ( ped && ped->m_areaCode == 0 )
    {
        float groundLevel1 = camera.GetGroundLevel( 0 );
        float groundLevel2 = camera.GetGroundLevel( 2 );

        if ( camPos.fZ - groundLevel1 > 50.0f && camPos.fZ - groundLevel2 > 10.0f )
        {
            if ( GetPlayerVehicle( -1, false ) )
            {
                EntityRender::blockAutomaticModelLoadingInVision = true;
            }
        }

        if ( camPos.fZ > 80.0f )
        {
            float divResult = ( camPos.fZ - *(float*)0x008CD808 ) / ( *(float*)0x008CD80C / *(float*)0x008CD808 );

            divResult = std::max( 1.0f, divResult );

            newBigScale = ( *(float*)0x008CD810 - 1.0f ) * divResult;
        }
    }

    *(float*)0x008CD804 = *(float*)0x00B7C548 * newBigScale;

    ((void (__cdecl*)( void ))0x00726DF0)();
    ((void (__cdecl*)( void ))0x007201C0)();

    // Clear all render queues.
    ClearFallbackRenderChains();

    // Set up render globals.
    *(CVector*)0x00B76870 = camPos;
    *(float*)0x00B7684C = camera.Placeable.GetHeading();

    Streamer::GetWorldFarclip() = camera.m_pRwCamera->farplane;

    *(void**)0x00B745D0 = (void*)0x00C8E0E0;
    *(void**)0x00B745CC = (void*)0x00C900C8;

    // Prepare stuff for rendering.
    ((void (__cdecl*)( void ))0x00554FE0)();
    ((void (__cdecl*)( void ))0x00553770)();

    // Cache lights to the frustum.
    RpLightPerformFrustumCaching();

    // Initialize the recent GC node system that sorts stuff.
    Streaming::InitRecentGCNode();
}

/*=========================================================
    PostProcessRenderEntitites

    Purpose:
        Called after rendering the main entities. In this routine
        distance objects are rendered that do not require
        alpha blending.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00553A10
=========================================================*/
struct PostProcessEntities
{
    bool __forceinline OnEntry( unorderedEntityRenderChainInfo& info )
    {
        CEntitySAInterface *entity = info.entity;

        if ( entity->nType == ENTITY_TYPE_BUILDING )
        {
            CBaseModelInfoSAInterface *model = entity->GetModelInfo();

            if ( IS_ANY_FLAG( entity->GetModelInfo()->flags, RENDER_STATIC ) )
            {
                RenderEntityNative( entity );
            }
        }

        return true;
    }
};

struct WorldLightingWrap
{
    inline WorldLightingWrap( void )
    {
        EntityRender::DisableSecondLighting();
        EntityRender::SetupPostProcessLighting();
    }

    inline ~WorldLightingWrap( void )
    {
    }

    bool useLightingFix;
};

void __cdecl PostProcessRenderEntities( void )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    rwInterface->m_deviceCommand( (eRwDeviceCmd)14, 1 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)12, 1 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)20, 2 );
    
    {
        // Configure lighting.
        WorldLightingWrap wLighting;

        // Post process the entities!
        staticRenderEntities.ExecuteCustom( PostProcessEntities() );

        // Notify the modifications.
        if ( _renderPostProcessCallback )
            _renderPostProcessCallback();
    }
}

/*=========================================================
    RenderWorldEntities

    Purpose:
        This is the main entity rendering routine.
        Most of the world is rendered here.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005556E0
=========================================================*/
struct RenderStaticWorldEntities
{
    AINLINE RenderStaticWorldEntities( bool isAlphaFix, bool directPurge ) : m_isAlphaFix( isAlphaFix ), m_directPurge( directPurge )
    { }

    AINLINE bool OnEntry( unorderedEntityRenderChainInfo& info )
    {
        CEntitySAInterface *entity = info.entity;

        if ( entity->nType == ENTITY_TYPE_BUILDING && IS_ANY_FLAG( entity->GetModelInfo()->flags, RENDER_STATIC ) )
            return true;

        bool successfullyRendered = false;

        if ( entity->nType == ENTITY_TYPE_VEHICLE || entity->nType == ENTITY_TYPE_PED && RpClumpGetAlpha( (RpClump*)entity->GetRwObject() ) != 255 )
        {
            // MTA extension: make sure that entities are still visible behind alpha textures.
            if ( m_isAlphaFix && !m_directPurge )
                successfullyRendered = true;
            else
            {
                bool isUnderwater = false;

                if ( entity->nType == ENTITY_TYPE_VEHICLE )
                {
                    CVehicleSAInterface *vehicle = (CVehicleSAInterface*)entity;

                    if ( vehicle->m_vehicleType == VEHICLE_BOAT )
                    {
                        CCamSAInterface& currentCam = Camera::GetInterface().GetActiveCam();

                        if ( currentCam.Mode != 14 )
                        {
                            int dirLook = Camera::GetInterface().GetActiveCamLookDirection();

                            if ( dirLook == 3 || dirLook == 0 )
                            {
                                if ( RpClumpGetAlpha( vehicle->GetRwObject() ) == 255 )
                                {
                                    isUnderwater = true;
                                }
                            }
                        }
                    }
                    else
                        isUnderwater = IS_ANY_FLAG( vehicle->physicalFlags, 0x8000000 );
                }

                using namespace EntityRender;

                float camDistance = GetEntityCameraDistance( entity );
                
                if ( !isUnderwater )
                {
                    successfullyRendered = QueueEntityForRendering( entity, camDistance ) == 1;
                }
                else
                {
                    successfullyRendered = PushUnderwaterEntityForRendering( entity, camDistance );
                }
            }
        }

        if ( !successfullyRendered )
            RenderEntity( entity );

        return true;
    }

    bool m_isAlphaFix;
    bool m_directPurge;
};

struct QuickRenderEntities
{
    bool __forceinline OnEntry( unorderedEntityRenderChainInfo& info )
    {
        RenderEntity( info.entity );
        return true;
    }
};

template <typename processor>
inline void ProcessRenderList( entityRenderChain_t& renderChain, processor cb, bool reverse )
{
    // In entityRenderChain_t, entities are ordered from back to front.
    if ( reverse )
        renderChain.ExecuteCustomReverse( cb );
    else
        renderChain.ExecuteCustom( cb );
}

struct StaticRenderStage
{
    AINLINE void OnRenderStage( bool isAlphaFix, bool directPurge, bool alphaFixReverseLoop )
    {
        RenderStaticWorldEntities renderSys( isAlphaFix, directPurge );

        staticRenderEntities.ExecuteCustom( renderSys );
    }
};

struct QuickRenderStage
{
    AINLINE void OnRenderStage( bool isAlphaFix, bool directPurge, bool alphaFixReverseLoop )
    {
        QuickRenderEntities renderSys;

        groundAlphaEntities.ExecuteCustom( renderSys );
    }
};

void __cdecl RenderWorldEntities( void )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    rwInterface->m_deviceCommand( (eRwDeviceCmd)14, 1 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)12, 1 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)20, 2 );

    if ( *(unsigned int*)VAR_currArea == 0 )
        rwInterface->m_deviceCommand( (eRwDeviceCmd)30, 140 );

    {
        StaticRenderStage staticCallback;

        RenderInstances( staticCallback );
    }

    // Notify our system.
    if ( _renderCallback )
        _renderCallback();

    RwCamera *gtaCamera = *(RwCamera**)0x00C1703C;

    gtaCamera->EndUpdate();

    float unk2 = gtaCamera->unknown2;

    gtaCamera->unknown2 += *(float*)0x008CD814;

    gtaCamera->BeginUpdate();

    {
        QuickRenderStage quickCallback;

        RenderInstances( quickCallback );
    }

    gtaCamera->EndUpdate();

    gtaCamera->unknown2 = unk2;

    gtaCamera->BeginUpdate();
}

/*=========================================================
    RenderGrassHouseEntities

    Purpose:
        This function renders special "grasshouse" model
        entities in a seperate render pass.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733800
=========================================================*/
void __cdecl RenderGrassHouseEntities( void )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    rwInterface->m_deviceCommand( (eRwDeviceCmd)1, 0 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)6, 1 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)8, 1 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)12, 1 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)10, 5 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)11, 6 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)14, 1 );
    rwInterface->m_deviceCommand( (eRwDeviceCmd)20, 1 );

    {
        WorldLightingWrap wLighting;

        EntityRender::GetAlphaEntityRenderChain().Execute();
    }

    rwInterface->m_deviceCommand( (eRwDeviceCmd)14, 0 );
}

/*=========================================================
    RenderUnderwaterEntities

    Purpose:
        Renders entities that have the underwater property
        assigned to them.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007337D0
=========================================================*/
struct SimpleStagePass
{
    AINLINE bool OnEntry( entityRenderInfo& info )
    {
        info.Execute();
        return true;
    }
};

struct OrderedRenderStage
{
    AINLINE OrderedRenderStage( entityRenderChain_t& renderChain ) : m_renderChain( renderChain )
    {
    }

    AINLINE void OnRenderStage( bool isAlphaFix, bool directPurge, bool alphaFixReverseLoop )
    {
        // If we render using alpha fix, the entityRenderChain_t, that is ordered back to front,
        // shall be optimized to cull opaque pixels first.
        if ( isAlphaFix )
            alphaFixReverseLoop = !alphaFixReverseLoop;

        ProcessRenderList( m_renderChain, SimpleStagePass(), alphaFixReverseLoop );
    }

    entityRenderChain_t& m_renderChain;
};

void __cdecl RenderUnderwaterEntities( void )
{
    {
        OrderedRenderStage orderedCallback( EntityRender::GetUnderwaterEntityRenderChain() );

        RenderInstances( orderedCallback );
    }

    // Notify the system.
    if ( _renderUnderwaterCallback )
        _renderUnderwaterCallback();
}

/*=========================================================
    RenderBoatAtomics

    Purpose:
        Renders boat atomics that have been picked out by
        the vehicle atomic rendering system.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733EC0
=========================================================*/
void __cdecl RenderBoatAtomics( void )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    rwInterface->m_deviceCommand( (eRwDeviceCmd)20, 1 );

    EntityRender::GetBoatRenderChain().Execute();

    rwInterface->m_deviceCommand( (eRwDeviceCmd)20, 2 );
}

/*=========================================================
    RenderDefaultOrderedWorldEntities

    Purpose:
        Renders entities that require alpha blending. By default,
        these entities are rendered from back to front to
        account for alpha blending.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733F10
=========================================================*/
void __cdecl RenderDefaultOrderedWorldEntities( void )
{
    {
        OrderedRenderStage orderedCallback( EntityRender::GetDefaultEntityRenderChain() );

        RenderInstances( orderedCallback );
    }

    RenderBoatAtomics();

    // This has to stay enabled anyway
    HOOK_RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    HOOK_RwD3D9SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
    HOOK_RwD3D9SetRenderState( D3DRS_ALPHATESTENABLE, true );
    HOOK_RwD3D9SetRenderState( D3DRS_ALPHAREF, 100 );

    // Lets apply the render states for safety.
    // Who knows whether R* produced clean logic? So let us play safe.
    RwD3D9ApplyDeviceStates();
}

void EntityRender_Init( void )
{
    using namespace EntityRender;

    // Do some interesting patches
    HookInstall( 0x00733A20, (DWORD)HOOK_InitRenderChains, 5 );
    HookInstall( 0x00553260, (DWORD)RenderEntity, 5 );
    HookInstall( 0x00734570, (DWORD)QueueEntityForRendering, 5 );
    HookInstall( 0x00554230, (DWORD)SetupEntityVisibility, 5 );
    HookInstall( 0x00553F60, (DWORD)RequestEntityModelInVision, 5 );

    HookInstall( 0x005556E0, (DWORD)SetupWorldRender, 5 );
    HookInstall( 0x005534B0, (DWORD)PushEntityOnRenderQueue, 5 );
    HookInstall( 0x00553910, (DWORD)PreRender, 5 );
    HookInstall( 0x00553AA0, (DWORD)RenderWorldEntities, 5 );
    HookInstall( 0x00553A10, (DWORD)PostProcessRenderEntities, 5 );
    HookInstall( 0x00733800, (DWORD)RenderGrassHouseEntities, 5 );
    HookInstall( 0x007337D0, (DWORD)RenderUnderwaterEntities, 5 );
    HookInstall( 0x00733F10, (DWORD)RenderDefaultOrderedWorldEntities, 5 );
    HookInstall( 0x00734540, (DWORD)ClearEntityRenderChains, 5 );

    // Experimental hooks.
    HookInstall( 0x00553DC0, h_memFunc( &CEntitySAInterface::_SetupLighting ), 5 );
    HookInstall( 0x00553370, h_memFunc( &CEntitySAInterface::_RemoveLighting ), 5 );

    // Do some optimization.
    PatchCall( 0x0053EBE9, (DWORD)ClearAllRenderChains );
}

void EntityRender_Shutdown( void )
{
}

void EntityRender_Reset( void )
{
    // By default, we want to render in original mode.
    Entity::SetWorldRenderMode( WORLD_RENDER_ORIGINAL );
}