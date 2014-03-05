/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.render.cpp
*  PURPOSE:     Entity render utilities
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CBaseModelInfoSAInterface **ppModelInfo;

void __cdecl HOOK_InitRenderChains( void )
{
    new ((void*)0x00C88070) atomicRenderChain_t( 50 );      // vehicleRenderChains, orig 20
    new ((void*)0x00C880C8) atomicRenderChain_t( 50 );      // boatRenderChains, orig 20
    new ((void*)0x00C88120) entityRenderChain_t( 8000 );    // ???, orig 200
    new ((void*)0x00C88178) entityRenderChain_t( 4000 );    // ???, orig 100, used to render underwater entities
    new ((void*)0x00C881D0) entityRenderChain_t( 50 );      // ???, orig 50, used for "grasshouse" model
    new ((void*)0x00C88224) pedRenderChain_t( 8000 );       // ???, orig 100
}

static bool __cdecl AreScreenEffectsEnabled( void )
{
    return !*(bool*)0x00C402B7 && ( *(bool*)0x00C402B8 || *(bool*)0x00C402B9 );
}

static void __cdecl SetupNightVisionLighting( void )
{
    // Check whether effects and night vision are enabled
    if ( !*(bool*)0x00C402B8 || *(bool*)0x00C402B7 )
        return;

    RwColorFloat color( 0, 1.0f, 0, 1.0f );

    (*(RpLight**)0x00C886E8)->SetColor( color );    // first light
    (*(RpLight**)0x00C886EC)->SetColor( color );    // second light
}

static void __cdecl SetupInfraRedLighting( void )
{
    // Check if effects and infra red are enabled
    if ( !*(bool*)0x00C402B9 || *(bool*)0x00C402B7 )
        return;

    RwColorFloat color( 0, 0, 1.0f, 1.0f );

    (*(RpLight**)0x00C886E8)->SetColor( color );    // first light
    (*(RpLight**)0x00C886EC)->SetColor( color );    // second light
}

float _tempDayNightBalance = 0;

static void __cdecl NormalizeDayNight( void )
{
    if ( *(bool*)0x00C402B9 )
        return;

    _tempDayNightBalance = *(float*)0x008D12C0;
    *(float*)0x008D12C0 = 1.0f;
}

static void __cdecl RestoreDayNight( void )
{
    if ( *(bool*)0x00C402B9 )
        return;

    *(float*)0x008D12C0 = _tempDayNightBalance;
}

static void __cdecl _RenderEntity( CEntitySAInterface *entity )
{
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
            alpha = ((CVehicleSA*)mtaEntity)->GetAlpha();
        else if ( entity->nType == ENTITY_TYPE_OBJECT )
            alpha = ((CObjectSA*)mtaEntity)->GetAlpha();
#if 0
        else if ( entity->nType == ENTITY_TYPE_PED )
            alpha = ((CPedSA*)mtaEntity)->GetAlpha();
#endif
    }
    else if ( entity->GetRwObject() && entity->GetRwObject()->type == RW_CLUMP )    // the entity may not have a RenderWare object?!
    {
        // This value will be used for internal GTA:SA vehicles.
        // We might aswell use this alpha value of the clump.
        alpha = (unsigned char)((RpClump*)entity->GetRwObject())->alpha;
    }

    if ( alpha != 255 )
    {
        // This has to stay enabled anyway
        RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE, true );
        RwD3D9SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
        RwD3D9SetRenderState( D3DRS_ALPHATESTENABLE, true );
        RwD3D9SetRenderState( D3DRS_ALPHAREF, 100 );

        // Ensure the RenderStates necessary for proper alpha blending
        alphaRef = new (rsAlloc.Allocate()) RwRenderStateLock( D3DRS_ALPHAREF, 0x00 );
        RwD3D9ApplyDeviceStates();
    }

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
    else if ( !entity->bBackfaceCulled )
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

        NormalizeDayNight();

        entity->Render();

        RestoreDayNight();
    }
    else
        entity->Render();

    // Restore values and unset entity rendering status/leave frame
    if ( entity->nType == ENTITY_TYPE_VEHICLE )
    {
        entity->bImBeingRendered = true;

        // Render the delayed atomics
        ExecuteVehicleRenderChains( alpha );

        entity->bImBeingRendered = false;

        CVehicleSAInterface *veh = (CVehicleSAInterface*)entity;

        // Leave rendering stage
        veh->LeaveRender();
    }   
    else if ( !entity->bBackfaceCulled )
    {
        // Set texture stage back to two (?)
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)20, 2 );
    }

    // Does the entity have alpha enabled?
    if ( alpha != 255 )
    {
        // Remove the RenderState locks
        alphaRef->~RwRenderStateLock();
        RwD3D9ApplyDeviceStates();
    }

    entity->RemoveLighting( id );
}

// Wiring in some MTA team fixes.
void __cdecl RenderEntity( CEntitySAInterface *entity )
{
    // FIX BEGIN
    OnMY_CEntity_RenderOneNonRoad_Pre( entity );

    _RenderEntity( entity );

    // FIX END
    OnMY_CEntity_RenderOneNonRoad_Post( entity );
}

// based on 0x0055458A (1.0 US and 1.0 EU)
__forceinline float GetComplexCameraEntityDistance( const CEntitySAInterface *entity )
{
    CCameraSAInterface *camera = pGame->GetCamera()->GetInterface();

    const CVector& camPos = camera->Placeable.GetPosition();
    const CVector& pos = entity->Placeable.GetPosition();
    
    float camDistance = ( camPos - pos ).Length();

    if ( camDistance > 300.0f )
    {
        CBaseModelInfoSAInterface *info = entity->GetModelInfo();
        float scaledLODDistance = info->fLodDistanceUnscaled * camera->LODDistMultiplier;

        if ( 300.0f < scaledLODDistance && ( scaledLODDistance + 20.0f ) > camDistance )
        {
            return scaledLODDistance - 300.0f + camDistance;
        }
    }

    return camDistance;
}

static float CalculateFadingAlpha( CBaseModelInfoSAInterface *info, const CEntitySAInterface *entity, float camDistance, float camFarClip )
{
    // Wire in a MTA team fix.
    int iCustomRet = OnMY_CVisibilityPlugins_CalculateFadingAtomicAlpha_Pre( info, entity, camDistance );

    if ( iCustomRet != -1 )
        return (float)iCustomRet / 255.0f;

    float sectorDivide = 20.0f;
    float lodScale = pGame->GetCamera()->GetInterface()->LODDistMultiplier;
    float distAway = info->pColModel->m_bounds.fRadius + camFarClip;
    float unscaledLODDistance = info->fLodDistanceUnscaled;
    float scaledLODDistance = unscaledLODDistance * lodScale;

    float useDist = distAway;

    if ( scaledLODDistance < distAway )
        useDist = scaledLODDistance;

    if ( !entity->m_pLod )
    {
        float useDist2 = unscaledLODDistance;

        if ( unscaledLODDistance > useDist )
            useDist2 = useDist;

        if ( useDist2 > 150.0f )
            sectorDivide = useDist2 * 0.06666667f + 10.0f;

        if ( entity->bIsBIGBuilding )
        {
            useDist *= *(float*)0x008CD804;
        }
    }

    useDist += 20.0f;
    useDist -= camDistance;
    useDist /= sectorDivide;

    if ( useDist < 0.0f )
        return 0;

    if ( useDist > 1.0f )
        useDist = 1;

    return useDist * info->ucAlpha;
}

float CEntitySAInterface::GetFadingAlpha( void ) const
{
    float camDistance = GetComplexCameraEntityDistance( this );

    return (unsigned char)CalculateFadingAlpha( GetModelInfo(), this, camDistance, *(float*)0x00B7C4F0 );
}

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

static void __cdecl RenderClumpWithAlpha( CBaseModelInfoSAInterface *info, RpClump *clump, unsigned int alpha )
{
    InitModelRendering( info );

    LIST_FOREACH_BEGIN( RpAtomic, clump->atomics.root, atomics )
        if ( item->IsVisible() )
            RpAtomicRenderAlpha( item, alpha );
    LIST_FOREACH_END

    ShutdownModelRendering( info );
}

static void __cdecl DefaultRenderEntityHandler( CEntitySAInterface *entity, float camDistance )
{
    RwObject *rwobj = entity->GetRwObject();

    if ( !rwobj )
        return;

    CBaseModelInfoSAInterface *info = entity->GetModelInfo();

    if ( info->renderFlags & RENDER_NOSHADOW )
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)8, 0 );

    if ( entity->bDistanceFade )
    {
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)30, 0 );

        unsigned int alpha = (unsigned char)CalculateFadingAlpha( info, entity, camDistance, *(float*)0x00B76848 );

        entity->bImBeingRendered = true;

        if ( entity->bBackfaceCulled )
            RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)20, 1 );

        unsigned char lightIndex = entity->SetupLighting();

        if ( rwobj->type == RW_ATOMIC )
            RenderAtomicWithAlpha( info, (RpAtomic*)rwobj, alpha );
        else
            RenderClumpWithAlpha( info, (RpClump*)rwobj, alpha );

        entity->RemoveLighting( lightIndex );

        entity->bImBeingRendered = false;

        if ( entity->bBackfaceCulled )
            RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)20, 2 );

        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)30, 100 );
    }
    else
    {
        if ( !*(unsigned int*)VAR_currArea && info->renderFlags & RENDER_NOSHADOW )
            RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)30, 100 );
        else
            RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)30, 0 );

        RenderEntity( entity );
    }

    if ( info->renderFlags & RENDER_NOSHADOW )
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)8, 1 );
}

int __cdecl QueueEntityForRendering( CEntitySAInterface *entity, float camDistance )
{
    // This function has been optimized a little.
    // Rockstar created two depthLevel instances, we only do once.
    entityRenderInfo level;
    level.callback = (entityRenderInfo::callback_t)0x00732B40;   // this special callback is mandatory
    level.entity = entity;
    level.distance = camDistance;

    // Try to display the "grasshouse" model
    if ( entity->GetModelIndex() == *(unsigned short*)0x008CD6F4 && ((entityRenderChain_t*)0x00C881D0)->PushRender( &level ) )
        return true;

    // If the entity is underwater, it needs a different rendering order
    if ( entity->bUnderwater )
        return ((entityRenderChain_t*)0x00C88178)->PushRender( &level ) != NULL;

    // Do default render
    return ((entityRenderChain_t*)0x00C88120)->PushRender( &level ) != NULL;
}

void EntityRender_Init( void )
{
    // Do some interesting patches
    HookInstall( 0x00733A20, (DWORD)HOOK_InitRenderChains, 5 );
    HookInstall( 0x00732B40, (DWORD)DefaultRenderEntityHandler, 5 );
    HookInstall( 0x00553260, (DWORD)RenderEntity, 5 );
    HookInstall( 0x00734570, (DWORD)QueueEntityForRendering, 5 );
}

void EntityRender_Shutdown( void )
{
}