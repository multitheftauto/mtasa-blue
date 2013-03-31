/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleModelInfoSA.render.cpp
*  PURPOSE:     Vehicle model info rendering module
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

#define VEHICLE_RENDER_DISABLE_LOD

#define VAR_CameraPositionVector                0x00C88050

// LOD distances for models, squared (measured against camera distance from atomic, 0x00733160 is rendering wrapper)
static float trainLODDistance = 45000;          // ~212 units
static float boatLODDistance = 9800;            // ~100 units
static float heliLODDistance = 9800;            // ~100 units, same as boat
static float heliRotorRenderDistance = 45000;   // ~212 units, same as train
static float planeLODDistance = 45000;          // ~212 units, same as train
static float vehicleLODDistance = 9800;         // ~100 units, same as boat
static float highDetailDistance = 4050;         // ~64 units

/*=========================================================
    RwAtomicGetVisibilityCalculation

    Arguments:
        atomic - rendering object to calculate for
    Purpose:
        Returns a special distance calculated from the actual atomic
        in relation to the model rotation.
=========================================================*/
inline static float RwAtomicGetVisibilityCalculation( RpAtomic *atomic )
{
    return RwMatrixUnknown( atomic->parent->GetLTM(), atomic->clump->parent->GetLTM(), atomic->componentFlags );
}

/*=========================================================
    RwAtomicIsVisibleBasic

    Arguments:
        atomic - rendering object to check visibility of
        calc - special rendering distance
    Purpose:
        Returns whether the atomic is visible. It is directly
        used by non-translucent plane atomics.
=========================================================*/
inline static bool RwAtomicIsVisibleBasic( RpAtomic *atomic, float calc )
{
    return !( atomic->componentFlags & 0x0400 ) || *(float*)0x00C88020 < 0.2f || calc > 0.0f;
}

/*=========================================================
    RwAtomicIsVisible

    Arguments:
        atomic - rendering object to check visibility of
        calc - special rendering distance
    Purpose:
        Returns whether the atomic is visible. This function does
        additional checks to RwAtomicIsVisibleBasic.
=========================================================*/
inline static bool RwAtomicIsVisible( RpAtomic *atomic, float calc )
{
    return RwAtomicIsVisibleBasic( atomic, calc ) || !( atomic->componentFlags & 0x80 ) && ( calc * calc ) >= *(float*)VAR_ATOMIC_RENDER_OFFSET * 0.1f;
}

/*=========================================================
    RwAtomicHandleHighDetail

    Arguments:
        atomic - rendering object to set flags for
    Purpose:
        If the atomic enters a very close distance (~64 units), a
        special flag is set to it. I assume it increases the rendering
        quality.
=========================================================*/
inline static void RwAtomicHandleHighDetail( RpAtomic *atomic )
{
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET < highDetailDistance )
        atomic->componentFlags &= ~0x2000;
    else
        atomic->componentFlags |= 0x2000;
}

/*=========================================================
    RwAtomicRenderTrainLOD

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders train LOD atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732820
=========================================================*/
static RpAtomic* RwAtomicRenderTrainLOD( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if (*(float*)VAR_ATOMIC_RENDER_OFFSET <= trainLODDistance)
        return atomic;

    RpAtomicRender( atomic );
#endif
    return atomic;
}

/*=========================================================
    RwAtomicRenderTranslucentTrain

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders transparent train atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734240
=========================================================*/
static RpAtomic* RwAtomicRenderTranslucentTrain( RpAtomic *atomic )
{
    RwAtomicZBufferEntry level;

#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= trainLODDistance )
        return atomic;
#endif

    RwAtomicHandleHighDetail( atomic );

    float calc = RwAtomicGetVisibilityCalculation( atomic );

    // Lol, serious checking going on here!
    if ( !( *(float*)VAR_ATOMIC_RENDER_OFFSET <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, calc ) ) )
        return atomic;

    // Set up rendering
    level.m_render = RpAtomicRender;
    level.m_atomic = atomic;
    level.m_distance = *(float*)VAR_ATOMIC_RENDER_OFFSET;

    if ( !(atomic->componentFlags & 0x40) )
        level.m_distance += calc;

    if ( !rwRenderChains->PushRender( &level ) )
        RpAtomicRender( atomic );

    return atomic;
}

/*=========================================================
    RwAtomicRenderTrain

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders train atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733330
=========================================================*/
static RpAtomic* RwAtomicRenderTrain( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= trainLODDistance )
        return atomic;
#endif

    RwAtomicHandleHighDetail( atomic );

    // Lol, serious checking going on here!
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, RwAtomicGetVisibilityCalculation( atomic ) ) )
        RpAtomicRender( atomic );

    return atomic;
}

/*=========================================================
    RwAtomicRenderLODAlpha

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders atomics while accounting for clump alpha.
=========================================================*/
inline static void RwAtomicRenderLODAlpha( RpAtomic *atomic )
{
    atomic->componentFlags |= 0x2000;

    if ( atomic->clump->alpha == 0xFF )
        RpAtomicRenderAlpha( atomic, atomic->clump->alpha );
    else
        RpAtomicRender( atomic );
}

/*=========================================================
    RwAtomicRenderBoatLOD

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders LOD boat atomics with clump alpha.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007334F0
=========================================================*/
static RpAtomic* RwAtomicRenderBoatLOD( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if (*(float*)VAR_ATOMIC_RENDER_OFFSET <= boatLODDistance)
        return atomic;

    RwAtomicRenderLODAlpha( atomic );
#endif
    return atomic;
}

/*=========================================================
    RwAtomicRenderBoat

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders boat atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733550
=========================================================*/
static RpAtomic* RwAtomicRenderBoat( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= boatLODDistance )
        return atomic;
#endif

    RwAtomicHandleHighDetail( atomic );

    RpAtomicRender( atomic );
    return atomic;
}

/*=========================================================
    RwAtomicRenderTranslucentBoat

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders transparent boat atomics with a special render chain.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007344A0
=========================================================*/
static RwAtomicRenderChainInterface *const boatRenderChain = (RwAtomicRenderChainInterface*)0x00C880C8;

static RpAtomic* RwAtomicRenderTranslucentBoat( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= boatLODDistance )
        return atomic;
#endif

    RwAtomicHandleHighDetail( atomic );

    if ( atomic->componentFlags & 0x40 )
    {
        RwAtomicZBufferEntry level;

        level.m_atomic = atomic;
        level.m_render = RpAtomicRender;
        level.m_distance = *(float*)VAR_ATOMIC_RENDER_OFFSET;

        if ( boatRenderChain->PushRender( &level ) )
            return atomic;
    }
    
    RpAtomicRender( atomic );
    return atomic;
}

/*=========================================================
    RwAtomicRenderHeliLOD

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders LOD helicopter atomics with clump alpha.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007331E0
=========================================================*/
static RpAtomic* RwAtomicRenderHeliLOD( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if (*(float*)VAR_ATOMIC_RENDER_OFFSET <= heliLODDistance)
        return atomic;

    RwAtomicRenderLODAlpha( atomic );
#endif
    return atomic;
}

/*=========================================================
    RwAtomicRenderHeli

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders helicopter atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733240
=========================================================*/
static RpAtomic* RwAtomicRenderHeli( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= heliLODDistance )
        return atomic;
#endif

    RwAtomicHandleHighDetail( atomic );

    // Lol, serious checking going on here!
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, RwAtomicGetVisibilityCalculation( atomic ) ) )
        RpAtomicRender( atomic );

    return atomic;
}

/*=========================================================
    RwAtomicRenderTranslucentCommon

    Arguments:
        atomic - object being rendered
    Purpose:
        Common routine for rendering translucent atomics.
=========================================================*/
inline static void RwAtomicRenderTranslucentCommon( RpAtomic *atomic )
{
    RwAtomicHandleHighDetail( atomic );

    float calc = RwAtomicGetVisibilityCalculation( atomic );

    // Lol, serious checking going on here!
    if ( !( *(float*)VAR_ATOMIC_RENDER_OFFSET <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, calc ) ) )
        return;

    // Set up rendering
    RwAtomicZBufferEntry level;
    level.m_render = RpAtomicRender;
    level.m_atomic = atomic;
    level.m_distance = *(float*)VAR_ATOMIC_RENDER_OFFSET;

    if ( atomic->componentFlags & 0x40 )
        level.m_distance -= 0.0001f;
    else
        level.m_distance += calc;

    if ( !rwRenderChains->PushRender( &level ) )
        RpAtomicRender( atomic );
}

/*=========================================================
    RwAtomicRenderTranslucentHeli

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders transparent helicopter atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733F80
=========================================================*/
static RpAtomic* RwAtomicRenderTranslucentHeli( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= heliLODDistance )
        return atomic;
#endif

    RwAtomicRenderTranslucentCommon( atomic );
    return atomic;
}

/*=========================================================
    RwAtomicRenderHeliMovingRotor

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders the 'moving_rotor' atomic of helicopter models.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007340B0
=========================================================*/
static RpAtomic* RwAtomicRenderHeliMovingRotor( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= heliRotorRenderDistance )
        return atomic;
#endif

    CVector vecRotor = atomic->parent->GetLTM().vPos - *(CVector*)VAR_CameraPositionVector;
    RwAtomicZBufferEntry level;

    // Calculate rotor details
    level.m_distance = vecRotor.DotProduct( atomic->clump->parent->GetLTM().vUp ) * 20 + *(float*)VAR_ATOMIC_RENDER_OFFSET;
    level.m_render = RpAtomicRender;
    level.m_atomic = atomic;

    if ( !rwRenderChains->PushRender( &level ) )
        RpAtomicRender( atomic );

    return atomic;
}

/*=========================================================
    RwAtomicRenderHeliMovingRotor2

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders the 'moving_rotor2' atomic of helicopter models.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734170
=========================================================*/
static RpAtomic* RwAtomicRenderHeliMovingRotor2( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= heliRotorRenderDistance )
        return atomic;
#endif

    CVector vecRotor = atomic->parent->GetLTM().vPos - *(CVector*)VAR_CameraPositionVector;
    RwAtomicZBufferEntry level;

    // Lulz, heavy math, much assembly, small C++ code
    level.m_distance = *(float*)VAR_ATOMIC_RENDER_OFFSET - vecRotor.DotProduct( atomic->clump->parent->GetLTM().vRight )
                                                         - vecRotor.DotProduct( atomic->clump->parent->GetLTM().vFront );
    level.m_render = RpAtomicRender;
    level.m_atomic = atomic;

    if ( !rwRenderChains->PushRender( &level ) )
        RpAtomicRender( atomic );

    return atomic;
}

/*=========================================================
    RwAtomicRenderPlaneLOD

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders LOD plane atomics.
    Note:
        The GTA:SA function equals RwAtomicRenderTrainLOD.
=========================================================*/
static RpAtomic* RwAtomicRenderPlaneLOD( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if (*(float*)VAR_ATOMIC_RENDER_OFFSET <= planeLODDistance)
        return atomic;

    RpAtomicRender( atomic );
#endif
    return atomic;
}

/*=========================================================
    RwAtomicRenderPlane

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders plane atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733420
=========================================================*/
static RpAtomic* RwAtomicRenderPlane( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= planeLODDistance )
        return atomic;
#endif

    RwAtomicHandleHighDetail( atomic );

    // Lol, serious checking going on here!
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET <= *(float*)0x00C88028 || RwAtomicIsVisibleBasic( atomic, RwAtomicGetVisibilityCalculation( atomic ) ) )
        RpAtomicRender( atomic );

    return atomic;
}

/*=========================================================
    RwAtomicRenderTranslucentPlane

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders translucent plane atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734370
=========================================================*/
static RpAtomic* RwAtomicRenderTranslucentPlane( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= heliLODDistance )
        return atomic;
#endif

    RwAtomicHandleHighDetail( atomic );

    float calc = RwAtomicGetVisibilityCalculation( atomic );

    // Lol, serious checking going on here!
    if ( !( *(float*)VAR_ATOMIC_RENDER_OFFSET <= *(float*)0x00C88028 || RwAtomicIsVisible( atomic, calc ) ) )
        return atomic;

    // Set up rendering
    RwAtomicZBufferEntry level;
    level.m_render = RpAtomicRender;
    level.m_atomic = atomic;
    level.m_distance = *(float*)VAR_ATOMIC_RENDER_OFFSET;

    if ( atomic->componentFlags & 0x40 )
        level.m_distance -= 0.0001f;
    else
        level.m_distance += calc;

    if ( !rwRenderChains->PushRender( &level ) )
        RpAtomicRender( atomic );
    
    return atomic;
}

/*=========================================================
    RwAtomicRenderTranslucentDefaultVehicle

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders translucent any vehicle atomics.
    Note:
        The GTA:SA function equals RwAtomicRenderTranslucentHeli.
=========================================================*/
static RpAtomic* RwAtomicRenderTranslucentDefaultVehicle( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= vehicleLODDistance )
        return atomic;
#endif

    RwAtomicRenderTranslucentCommon( atomic );
    return atomic;
}

/*=========================================================
    RwAtomicRenderDefaultVehicle

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders any vehicle atomics.
    Note:
        The GTA:SA function equals RwAtomicRenderHeli.
=========================================================*/
static RpAtomic* RwAtomicRenderDefaultVehicle( RpAtomic *atomic )    // actually equals heli render
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET >= vehicleLODDistance )
        return atomic;
#endif

    RwAtomicHandleHighDetail( atomic );

    // Lol, serious checking going on here!
    if ( *(float*)VAR_ATOMIC_RENDER_OFFSET <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, RwAtomicGetVisibilityCalculation( atomic ) ) )
        RpAtomicRender( atomic );

    return atomic;
}

/*=========================================================
    RwAtomicRenderDefaultVehicleLOD

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders any vehicle LOD atomics.
    Note:
        The GTA:SA function equals RwAtomicRenderHeliLOD.
=========================================================*/
static RpAtomic* RwAtomicRenderDefaultVehicleLOD( RpAtomic *atomic )
{
#ifndef VEHICLE_RENDER_DISABLE_LOD
    if (*(float*)VAR_ATOMIC_RENDER_OFFSET <= vehicleLODDistance)
        return atomic;

    RwAtomicRenderLODAlpha( atomic );
#endif
    return atomic;
}

/*=========================================================
    RwAtomicSetupVehicleDamaged

    Arguments:
        child - atomic which shall be checked for damage
                component status
    Purpose:
        Sets the atomic up for the GTA:SA damage system.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7720
=========================================================*/
static bool RwAtomicSetupVehicleDamaged( RpAtomic *child )
{
    if ( strstr(child->parent->szName, "_dam") )
    {
        child->flags = 0;

        child->componentFlags = 2;
        return true;
    }

    if ( strstr(child->parent->szName, "_ok") )
    {
        child->componentFlags = 1;
        return true;
    }

    return true;
}

/*=========================================================
    RwAtomicRegisterTrain

    Arguments:
        child - atomic to set up rendering for
    Purpose:
        Registers the appropriate rendering callback for the
        atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7AA0
=========================================================*/
static bool RwAtomicRegisterTrain( RpAtomic *child, int )
{
    if ( strstr( child->parent->szName, "_vlo" ) )
    {
        child->SetRenderCallback( RwAtomicRenderTrainLOD );
        return true;
    }
    else if ( child->geometry->IsAlpha() )
        child->SetRenderCallback( RwAtomicRenderTranslucentTrain ); // translucent polys need second render pass
    else
        child->SetRenderCallback( RwAtomicRenderTrain );

    RwAtomicSetupVehicleDamaged( child );
    return true;
}

/*=========================================================
    RwAtomicRegisterBoat

    Arguments:
        child - atomic to set up rendering for
    Purpose:
        Registers the appropriate rendering callback for the
        atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C79A0
=========================================================*/
static bool RwAtomicRegisterBoat( RpAtomic *child, int )
{
    if ( strcmp( child->parent->szName, "boat_hi" ) == 0 )
        child->SetRenderCallback( RwAtomicRenderBoat );         // boat_hi does not support alpha?
    else if ( strstr( child->parent->szName, "_vlo" ) )
        child->SetRenderCallback( RwAtomicRenderBoatLOD );
    else if ( child->geometry->IsAlpha() )
        child->SetRenderCallback( RwAtomicRenderTranslucentBoat );
    else
        child->SetRenderCallback( RwAtomicRenderBoat );

    RwAtomicSetupVehicleDamaged( child );
    return true;
}

/*=========================================================
    RwAtomicRegisterHeli

    Arguments:
        child - atomic to set up rendering for
    Purpose:
        Registers the appropriate rendering callback for the
        atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7870
=========================================================*/
static bool RwAtomicRegisterHeli( RpAtomic *child, int )
{
    if ( strcmp( child->parent->szName, "moving_rotor" ) == 0 )
        child->SetRenderCallback( RwAtomicRenderHeliMovingRotor );
    else if ( strcmp( child->parent->szName, "moving_rotor2" ) == 0 )
        child->SetRenderCallback( RwAtomicRenderHeliMovingRotor2 );
    else if ( strstr( child->parent->szName, "_vlo" ) )
        child->SetRenderCallback( RwAtomicRenderHeliLOD );
    else if ( child->geometry->IsAlpha() || strncmp( child->parent->szName, "windscreen", 10) == 0 )
        child->SetRenderCallback( RwAtomicRenderTranslucentHeli );
    else
        child->SetRenderCallback( RwAtomicRenderHeli );

    RwAtomicSetupVehicleDamaged( child );
    return true;
}

/*=========================================================
    RwAtomicRegisterPlane

    Arguments:
        child - atomic to set up rendering for
    Purpose:
        Registers the appropriate rendering callback for the
        atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7930
=========================================================*/
static bool RwAtomicRegisterPlane( RpAtomic *child, int )
{
    if ( strstr( child->parent->szName, "_vlo" ) )
    {
        child->SetRenderCallback( RwAtomicRenderPlaneLOD );
        return true;
    }
    else if ( child->geometry->IsAlpha() )
        child->SetRenderCallback( RwAtomicRenderTranslucentPlane );
    else
        child->SetRenderCallback( RwAtomicRenderPlane );

    RwAtomicSetupVehicleDamaged( child );
    return true;
}

/*=========================================================
    RwAtomicRegisterDefaultVehicle

    Arguments:
        child - atomic to set up rendering for
    Purpose:
        Registers the appropriate rendering callback for the
        atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C77E0
=========================================================*/
static bool RwAtomicRegisterDefaultVehicle( RpAtomic *child, int )
{
    if ( strstr( child->parent->szName, "_vlo" ) )
        child->SetRenderCallback( RwAtomicRenderDefaultVehicleLOD );
    else if ( child->geometry->IsAlpha() || strnicmp( child->parent->szName, "windscreen", 10 ) == 0 )
        child->SetRenderCallback( RwAtomicRenderTranslucentDefaultVehicle );
    else
        child->SetRenderCallback( RwAtomicRenderDefaultVehicle );

    RwAtomicSetupVehicleDamaged( child );
    return true;
}

/*=========================================================
    CVehicleModelInfoSAInterface::RegisterRenderCallbacks

    Purpose:
        Scans the clump according to vehicle type and applies
        attributes such as rendering callback and damage
        component flags.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C7B10
=========================================================*/
void CVehicleModelInfoSAInterface::RegisterRenderCallbacks( void )
{
    switch( m_vehicleType )
    {
    case VEHICLE_TRAIN:
        GetRwObject()->ForAllAtomics( RwAtomicRegisterTrain, 0 );
        return;
    case VEHICLE_PLANE:
    case VEHICLE_FAKEPLANE:
        GetRwObject()->ForAllAtomics( RwAtomicRegisterPlane, 0 );
        return;
    case VEHICLE_BOAT:
        GetRwObject()->ForAllAtomics( RwAtomicRegisterBoat, 0 );
        return;
    case VEHICLE_HELI:
        GetRwObject()->ForAllAtomics( RwAtomicRegisterHeli, 0 );
        return;
    }

    GetRwObject()->ForAllAtomics( RwAtomicRegisterDefaultVehicle, 0 );
}

void VehicleModelInfoRender_Init( void )
{
    // Apply our own rendering logic ;)
    HookInstall( 0x004C7B10, h_memFunc( &CVehicleModelInfoSAInterface::RegisterRenderCallbacks ), 5 );
}

void VehicleModelInfoRender_Shutdown( void )
{
}