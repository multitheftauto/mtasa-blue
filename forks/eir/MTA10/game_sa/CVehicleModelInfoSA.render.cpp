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

static bool renderLOD = true;
static bool highQualityRender = false;
static bool renderAlpha = false;
// Rendering should be faster on GPU if you enable highQualityRender.
// On the other hand it takes (possibly) longer to calculate the distance of every atomic to the camera.

#define VAR_CameraPositionVector                0x00C88050

/*=========================================================
    GetVectorDistanceToCameraSq

    Arguments:
        vec - world position to get distance to camera from
    Purpose:
        Returns the distance to camera of the world position.
=========================================================*/
inline static float GetVectorDistanceToCameraSq( const CVector& pos )
{
    return ( pos - **(CVector**)0x00C88050 ).LengthSquared();
}

/*=========================================================
    GetAtomicDistanceToCameraSq

    Arguments:
        obj - RenderWare atomic to get distance to camera from
    Purpose:
        Returns the distance to camera of the given atomic.
=========================================================*/
inline static float GetObjectDistanceToCameraSq( RpAtomic *obj )
{
    const RwSphere& sphere = obj->GetWorldBoundingSphere();

    return GetVectorDistanceToCameraSq( sphere.pos );// - sphere.radius * sphere.radius; // dunno about that.
}

/*=========================================================
    GetObjectOffsetRotation

    Arguments:
        obj - RenderWare object to specify special rotation of
    Purpose:
        Returns the special render euler radian rotation value
        between camera position and RenderWare object modelling
        offset [sic].
=========================================================*/
inline static float GetObjectOffsetRotation( RwObject *obj )
{
    CVector dist = **(CVector**)0x00C88050 - obj->parent->GetModelling().vPos;
    return atan2( dist.fZ, sqrt( dist.fX * dist.fX + dist.fY + dist.fY ) );
}

/*=========================================================
    CacheVehicleRenderCameraSettings

    Arguments:
        alpha - global rendering alpha value [MTA extension]
        obj - vehicle rendering object
    Purpose:
        Sets up cached camera details for the rendering of
        the given vehicle clump.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00733160
=========================================================*/
static float cameraRenderDistanceSq = 0;
static float cameraRenderAngleRadians = 0;

void __cdecl CacheVehicleRenderCameraSettings( unsigned char alpha, RwObject *obj )
{
    highQualityRender = g_effectManager->GetEffectQuality() > 1;        // render High Quality if FX Quality better than Medium
    renderLOD = g_effectManager->GetEffectQuality() < 3;                // render LOD vehicles if FX Quality not Very High
    renderAlpha = highQualityRender && alpha != 255;                    // forces sorting of atomics (makes sense with highQualityRender only)

    if ( !highQualityRender )
    {
        if ( obj->type != RW_CLUMP )
            return;
      
        cameraRenderDistanceSq = GetVectorDistanceToCameraSq( obj->parent->GetLTM().vPos );
        cameraRenderAngleRadians = GetObjectOffsetRotation( obj );
    }
}

/*=========================================================
    GetRenderObjectCameraDistanceSq

    Arguments:
        obj - RenderWare object to specify distance to camera to
    Purpose:
        Common routine for the vehicle atomics to retrieve distance
        to camera. Returns either a cached value or the real
        distance depending on configuration.
=========================================================*/
static inline float GetRenderObjectCameraDistanceSq( RpAtomic *obj )
{
    if ( highQualityRender )
        return GetObjectDistanceToCameraSq( obj );
    else
        return cameraRenderDistanceSq;
}

/*=========================================================
    GetRenderObjectOffsetRotation

    Arguments:
        obj - RenderWare object to specify distance to camera to
    Purpose:
        Common routine for the vehicle atomics to retrieve a special
        camera rotation value. Returns either a cached value or the real
        distance depending on configuration.
=========================================================*/
static inline float GetRenderObjectOffsetRotation( RpAtomic *obj )
{
    if ( highQualityRender )
        return GetObjectOffsetRotation( obj );
    else
        return cameraRenderAngleRadians;
}

/*=========================================================
    ClearVehicleRenderChains

    Purpose:
        Called by the render manager to clear previously
        listed-for-rendering vehicle atomics.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00734530
=========================================================*/
// Special vehicle render group
struct vehicleRenderInfo : public atomicRenderInfo
{
    inline void Execute( void )
    {
        atomicRenderInfo::Execute();
    }
};
typedef CRenderChainInterface <vehicleRenderInfo> vehicleRenderChain_t;

// Render chains for vehicles only
static vehicleRenderChain_t vehicleRenderChains( 50 );  // Binary offsets: (1.0 US and 1.0 EU): 0x00C88070
static vehicleRenderChain_t opaqueRenderChain( 128 );   // MTA extension
static vehicleRenderChain_t lastRenderChain( 16 );      // MTA extension

void __cdecl ClearVehicleRenderChains( void )
{
    opaqueRenderChain.Clear();
    vehicleRenderChains.Clear();
    lastRenderChain.Clear();
}

/*=========================================================
    ExecuteVehicleRenderChains

    Purpose:
        Renders all atomics which were listed for delayed
        rendering (transparent/opaque).
=========================================================*/
void __cdecl ExecuteVehicleRenderChains( unsigned char renderAlpha )
{
    // Do special alpha blending if quality is set to high/very high
    if ( g_effectManager->GetEffectQuality() > 1 )
    {
        {
            RwRenderStateLock zfunc( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
            RwRenderStateLock alphaRef( D3DRS_ALPHAREF, renderAlpha );
            RwRenderStateLock alphaBlendEnable( D3DRS_ALPHABLENDENABLE, true );
            RwRenderStateLock alphaTestEnable( D3DRS_ALPHATESTENABLE, true );

            // Make things compatible with our depth fix.
            bool globalDoAlphaFix, globalRenderOpaque, globalRenderTranslucent, globalRenderDepth;

            globalDoAlphaFix = RenderCallbacks::IsAlphaSortingEnabled();
            
            RenderCallbacks::GetAlphaSortingParams( globalRenderOpaque, globalRenderTranslucent, globalRenderDepth );

            if ( !globalDoAlphaFix || globalRenderDepth )
            {
                if ( renderAlpha != 255 )
                {
                    RenderCallbacks::SetVehicleAlphaSortingEnabled( true );
                    RenderCallbacks::SetVehicleAlphaSortingParams( false, false, true );
                    
                    // Make sure we only render depth of opaque pixels.
                    RenderCallbacks::SetVehicleAlphaClamp( renderAlpha );

                    // Render depth components.
                    opaqueRenderChain.Execute();
                    vehicleRenderChains.ExecuteReverse();
                    lastRenderChain.Execute();

                    RenderCallbacks::SetVehicleAlphaSortingEnabled( false );
                }
            }

            // Set opaque rendering flags
            if ( !globalDoAlphaFix || globalRenderOpaque )
            {
                RwRenderStateLock alphaFunc( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );    // for now, instead of D3DCMP_EQUAL
                RwRenderStateLock alphaTestEnable( D3DRS_ALPHATESTENABLE, true );
                RwRenderStateLock zwriteEnable( D3DRS_ZWRITEENABLE, true );

                // First render components which are opaque only
                opaqueRenderChain.Execute();
                vehicleRenderChains.ExecuteReverse();
                lastRenderChain.Execute();
            }

            if ( !globalDoAlphaFix || globalRenderTranslucent )
            {
                // Now render translucent polygons
                RwRenderStateLock alphaFunc( D3DRS_ALPHAFUNC, D3DCMP_LESS );
                RwRenderStateLock zwriteEnable( D3DRS_ZWRITEENABLE, false );
                RwRenderStateLock cullMode( D3DRS_CULLMODE, D3DCULL_NONE );

                // Render alpha polygons
                opaqueRenderChain.ExecuteReverse();
                vehicleRenderChains.Execute();
                lastRenderChain.ExecuteReverse();
            }
        }
    }
    else
        vehicleRenderChains.Execute(); // do what GTA:SA usually does.
}

// LOD distances for models, squared (measured against camera distance from atomic, 0x00733160 is rendering wrapper)
static float trainLODDistance = 45000;          // ~212 units
static float boatLODDistance = 9800;            // ~100 units
static float heliLODDistance = 9800;            // ~100 units, same as boat
static float heliRotorRenderDistance = 45000;   // ~212 units, same as train
static float planeLODDistance = 45000;          // ~212 units, same as train
static float vehicleLODDistance = 9800;         // ~100 units, same as boat
static float highDetailDistance = 4050;         // ~64 units

/*=========================================================
    _renderAtomicCommon

    Arguments:
        atom - the z-layered atomic to render
    Purpose:
        Composite callback to render atomics in the z-layered
        render chains.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732B30
=========================================================*/
static void __cdecl _renderAtomicCommon( RpAtomic *atom )
{
    RpAtomicRender( atom );
}

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
    return !( atomic->componentFlags & 0x0400 ) || GetRenderObjectOffsetRotation( atomic ) < 0.2f || calc > 0.0f;
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
inline static bool RwAtomicIsVisible( RpAtomic *atomic, float calc, float camDistanceSq )
{
    return RwAtomicIsVisibleBasic( atomic, calc ) || !( atomic->componentFlags & 0x80 ) && ( calc * calc ) >= camDistanceSq * 0.1f;
}

/*=========================================================
    RwAtomicHandleHighDetail

    Arguments:
        atomic - rendering object to set flags for
    Purpose:
        If the atomic enters a very close distance (~64 units), a
        special flag is set to it. It does nothing quality-wise.
        Flag 0x2000 can be used to determine very close atomics.
=========================================================*/
inline static void RwAtomicHandleHighDetail( RpAtomic *atomic, float camDistanceSq )
{
    if ( camDistanceSq < highDetailDistance )
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
static __forceinline bool RwAtomicQueueOpaque( RpAtomic *atomic, float camDistanceSq )
{
    // Set up rendering
    vehicleRenderChain_t::depthLevel level;
    level.callback = _renderAtomicCommon;
    level.atomic = atomic;
    //level.distance = camDistanceSq;
    return opaqueRenderChain.PushRenderLast( &level ) != NULL;
}

static __forceinline bool RwAtomicQueueOpaqueLast( RpAtomic *atomic, float camDistanceSq )
{
    // Set up rendering
    vehicleRenderChain_t::depthLevel level;
    level.callback = _renderAtomicCommon;
    level.atomic = atomic;
    //level.distance = camDistanceSq;
    return lastRenderChain.PushRenderLast( &level ) != NULL;
}

static __forceinline bool RwAtomicQueue( RpAtomic *atomic, float camDistanceSq )
{
    // Set up rendering
    vehicleRenderChain_t::depthLevel level;
    level.callback = _renderAtomicCommon;
    level.atomic = atomic;
    level.distance = camDistanceSq;
    return vehicleRenderChains.PushRender( &level ) != NULL;
}

static __forceinline void RwAtomicRenderVehicle( RpAtomic *atomic, float camDistanceSq )
{
    if ( !renderAlpha || !RwAtomicQueueOpaque( atomic, camDistanceSq ) )
        RpAtomicRender( atomic );
}

static __forceinline void RwAtomicRenderVehicleLast( RpAtomic *atomic, float camDistanceSq )
{
    if ( !renderAlpha || !RwAtomicQueueOpaqueLast( atomic, camDistanceSq ) )
        RpAtomicRender( atomic );
}

static RpAtomic* RwAtomicRenderTrainLOD( RpAtomic *atomic )
{
    if ( renderLOD )
    {
        float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );
        
        if ( camDistanceSq <= trainLODDistance )
            return atomic;
        
        RwAtomicRenderVehicle( atomic, camDistanceSq );
    }

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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= trainLODDistance )
        return atomic;

    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    float calc = RwAtomicGetVisibilityCalculation( atomic );

    // Lol, serious checking going on here!
    if ( !( camDistanceSq <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, calc, camDistanceSq ) ) )
        return atomic;

    if ( !highQualityRender && !(atomic->componentFlags & 0x40) )
    {
        camDistanceSq += calc;
    }

    // Queue for default translucency
    if ( !RwAtomicQueue( atomic, camDistanceSq ) )
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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= trainLODDistance )
        return atomic;

    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    // Lol, serious checking going on here!
    if ( camDistanceSq <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, RwAtomicGetVisibilityCalculation( atomic ), camDistanceSq ) )
        RwAtomicRenderVehicle( atomic, camDistanceSq );

    return atomic;
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
    if ( renderLOD )
    {
        float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

        if ( camDistanceSq <= boatLODDistance )
            return atomic;

        RwAtomicRenderVehicle( atomic, camDistanceSq );
    }
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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= boatLODDistance )
        return atomic;

    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    RwAtomicRenderVehicle( atomic, camDistanceSq );
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
static RpAtomic* RwAtomicRenderTranslucentBoat( RpAtomic *atomic )
{
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= boatLODDistance )
        return atomic;

    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    if ( atomic->componentFlags & 0x40 )
    {
        vehicleRenderChain_t::depthLevel level;
        level.callback = _renderAtomicCommon;
        level.atomic = atomic;
        level.distance = camDistanceSq;

        if ( EntityRender::GetBoatRenderChain().PushRender( &level ) )
            return atomic;
    }
    
    RwAtomicRenderVehicle( atomic, camDistanceSq );
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
    if ( renderLOD )
    {
        float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

        if ( camDistanceSq <= heliLODDistance )
            return atomic;

        RwAtomicRenderVehicle( atomic, camDistanceSq );
    }

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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= heliLODDistance )
        return atomic;

    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    // Lol, serious checking going on here!
    if ( camDistanceSq <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, RwAtomicGetVisibilityCalculation( atomic ), camDistanceSq ) )
        RwAtomicRenderVehicle( atomic, camDistanceSq );

    return atomic;
}

/*=========================================================
    RwAtomicRenderTranslucentCommon

    Arguments:
        atomic - object being rendered
    Purpose:
        Common routine for rendering translucent atomics.
=========================================================*/
inline static void RwAtomicRenderTranslucentCommon( RpAtomic *atomic, float camDistanceSq )
{
    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    float calc = RwAtomicGetVisibilityCalculation( atomic );

    // Lol, serious checking going on here!
    if ( !( camDistanceSq <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, calc, camDistanceSq ) ) )
        return;

    // Perform special sorting
    if ( !highQualityRender )
    {
        if ( atomic->componentFlags & 0x40 )
            camDistanceSq -= 0.0001f;
        else
            camDistanceSq += calc;
    }

    // Set up rendering
    if ( !RwAtomicQueue( atomic, camDistanceSq ) )
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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= heliLODDistance )
        return atomic;

    RwAtomicRenderTranslucentCommon( atomic, camDistanceSq );
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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= heliRotorRenderDistance )
        return atomic;

    const RwMatrix& ltm = atomic->parent->GetLTM();
    CVector vecRotor = ltm.vPos - **(CVector**)VAR_CameraPositionVector;

    // Calculate rotor details
    if ( !RwAtomicQueue( atomic, vecRotor.DotProduct( ltm.vUp ) * 20 + camDistanceSq ) )
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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= heliRotorRenderDistance )
        return atomic;

    const RwMatrix& ltm = atomic->parent->GetLTM();
    CVector vecRotor = ltm.vPos - **(CVector**)VAR_CameraPositionVector;

    // Lulz, heavy math, much assembly, small C++ code
    if ( !RwAtomicQueue( atomic, camDistanceSq - vecRotor.DotProduct( ltm.vRight ) - vecRotor.DotProduct( ltm.vFront ) ) )
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
    if ( renderLOD )
    {
        float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

        if ( camDistanceSq <= planeLODDistance )
            return atomic;

        RwAtomicRenderVehicle( atomic, camDistanceSq );
    }

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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= planeLODDistance )
        return atomic;

    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    // Lol, serious checking going on here!
    if ( camDistanceSq <= *(float*)0x00C88028 || RwAtomicIsVisibleBasic( atomic, RwAtomicGetVisibilityCalculation( atomic ) ) )
        RwAtomicRenderVehicle( atomic, camDistanceSq );

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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= planeLODDistance )
        return atomic;

    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    float calc = RwAtomicGetVisibilityCalculation( atomic );

    // Lol, serious checking going on here!
    if ( !( camDistanceSq <= *(float*)0x00C88028 || RwAtomicIsVisible( atomic, calc, camDistanceSq ) ) )
        return atomic;

    if ( !highQualityRender )
    {
        if ( atomic->componentFlags & 0x40 )
            camDistanceSq -= 0.0001f;
        else
            camDistanceSq += calc;
    }

    // Set up rendering
    if ( !RwAtomicQueue( atomic, camDistanceSq ) )
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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= vehicleLODDistance )
        return atomic;

    RwAtomicRenderTranslucentCommon( atomic, camDistanceSq );
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
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= vehicleLODDistance )
        return atomic;

    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    // Lol, serious checking going on here!
    if ( camDistanceSq <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, RwAtomicGetVisibilityCalculation( atomic ), camDistanceSq ) )
        RwAtomicRenderVehicle( atomic, camDistanceSq );

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
    if ( renderLOD )
    {
        float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

        if ( camDistanceSq <= vehicleLODDistance )
            return atomic;

        RwAtomicRenderVehicle( atomic, camDistanceSq );
    }

    return atomic;
}

/*=========================================================
    RwAtomicRenderDefaultVehicleLast (MTA extension)

    Arguments:
        atomic - object being rendered
    Purpose:
        Renders vehicle atomics which have to be rendered last.
        It is applied to vehicle wheels to clip them properly.
=========================================================*/
static RpAtomic* RwAtomicRenderDefaultVehicleLast( RpAtomic *atomic )
{
    float camDistanceSq = GetRenderObjectCameraDistanceSq( atomic );

    if ( renderLOD && camDistanceSq >= vehicleLODDistance )
        return atomic;

    RwAtomicHandleHighDetail( atomic, camDistanceSq );

    // Lol, serious checking going on here!
    if ( camDistanceSq <= *(float*)0x00C8802C || RwAtomicIsVisible( atomic, RwAtomicGetVisibilityCalculation( atomic ), camDistanceSq ) )
        RwAtomicRenderVehicleLast( atomic, camDistanceSq );

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
    if ( strstr( child->parent->szName, "_dam" ) )
    {
        child->flags = 0;

        child->componentFlags = 2;
        return true;
    }

    if ( strstr( child->parent->szName, "_ok" ) )
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
static int RwAtomicRegisterTrain( RpAtomic *child, int )
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
static int RwAtomicRegisterBoat( RpAtomic *child, int )
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
static int RwAtomicRegisterHeli( RpAtomic *child, int )
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
static int RwAtomicRegisterPlane( RpAtomic *child, int )
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
static int RwAtomicRegisterDefaultVehicle( RpAtomic *child, int )
{
    if ( strstr( child->parent->szName, "_vlo" ) )
        child->SetRenderCallback( RwAtomicRenderDefaultVehicleLOD );
    else if ( child->geometry->IsAlpha() || strnicmp( child->parent->szName, "windscreen", 10 ) == 0 )
        child->SetRenderCallback( RwAtomicRenderTranslucentDefaultVehicle );
    else if ( strstr( child->parent->szName, "wheel" ) )  // MTA extension: render wheels last.
        child->SetRenderCallback( RwAtomicRenderDefaultVehicleLast );
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
    switch( GetVehicleType() )
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

/*=========================================================
    CVehicleModelInfoSAInterface::SetRenderColor

    Arguments:
        color[1-4] - vehicle color indexes
    Purpose:
        Sets the vehicle render colors which will be applied
        be applied to materials.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C84B0
=========================================================*/
static unsigned char _vehColor1, _vehColor2, _vehColor3, _vehColor4;

void CVehicleModelInfoSAInterface::SetRenderColor( unsigned char color1, unsigned char color2, unsigned char color3, unsigned char color4 )
{
    // Store the color values
    _vehColor1 = color1;
    _vehColor2 = color2;
    _vehColor3 = color3;
    _vehColor4 = color4;

    // Also store it in the vehicle model info
    this->color1 = color1;
    this->color2 = color2;
    this->color3 = color3;
    this->color4 = color4;
}

/*=========================================================
    SetVehicleLightsFlags

    Arguments:
        vehicle - vehicle entity interface to set bools for
    Purpose:
        Caches color boolean flags for the given vehicle
        interface.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8C90
=========================================================*/
bool vehLightFlags[4];
static CVehicleSA *gameVehicle;

void __cdecl SetVehicleLightsFlags( CVehicleSAInterface *veh )
{
    // Store render flags
    vehLightFlags[1] = ( veh->m_lightFlags ) & 1;
    vehLightFlags[0] = ( veh->m_lightFlags >> 1 ) & 1;
    vehLightFlags[3] = ( veh->m_lightFlags >> 2 ) & 1;
    vehLightFlags[2] = ( veh->m_lightFlags >> 3 ) & 1;
}

/*=========================================================
    RpGeometryMaterialSetupColor

    Arguments:
        mat - material with vehicle color information of
              special component
        storage - pointer to an array which can store
                  material color information
    Purpose:
        Stores color information of the materials of said
        geometry. The model colors may be restored from
        the same array later.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C83B0
    Note:
        Watch out that no vehicle overshoots the limit of the
        _colorTextureStorage array!
=========================================================*/
struct _colorTextureStorage
{
    union
    {
        // Has to match this size!
        struct
        {
            DWORD*      _matUnk;
            DWORD       unk;
        };

        // User defines
        struct
        {
            RwColor*    matColor;
            RwColor     color;
        };
        struct
        {
            RwTexture** matTexture;
            RwTexture*  texture;
        };
        struct
        {
            float*      matAmbient;
            float       ambient;
        };
    };
};

static inline void _StoreColorInfo( _colorTextureStorage **storage, RpMaterial *mat )
{
    // Store the color information
    _colorTextureStorage *entry = (*storage)++;
    entry->matColor = &mat->color;
    entry->color = mat->color;
}

static inline void _StoreTextureInfo( _colorTextureStorage **storage, RpMaterial *mat )
{
    // Store the original texture information
    _colorTextureStorage *entry = (*storage)++;
    entry->matTexture = &mat->texture;
    entry->texture = mat->texture;
}

static inline void _StoreAmbientInfo( _colorTextureStorage **storage, RpMaterial *mat )
{
    // Store the ambient information
    _colorTextureStorage *entry = (*storage)++;
    entry->matAmbient = &mat->lighting.ambient;
    entry->ambient = mat->lighting.ambient;
}

static bool RpGeometryMaterialSetupColor( RpMaterial *mat, _colorTextureStorage **storage )
{
    if ( mat->color.a == 255 )
        return true;

    // Pop an entry from the array
    _StoreColorInfo( storage, mat );

    // Make this invisible and black
    mat->color = 0;
    return true;
}

/*=========================================================
    RpGeometryMaterialApplyVehicleColor

    Arguments:
        mat - material with vehicle color + texture information
        storage - pointer to an array which can store
                  material color and texture information
    Purpose:
        Sets the per-vehicle color to the material of the
        vehicle clump and replaces textures. Returns true
        if color information has been saved, false otherwise.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8220
=========================================================*/
static MaterialContainer vehMats;

static bool RpGeometryMaterialApplyVehicleColor( RpMaterial *mat, _colorTextureStorage **storage )
{
    unsigned int color = mat->color & 0x00FFFFFF;
    RwColor& colorRef = mat->color;
    
    RwTexture *matTex = mat->texture;

    if ( RwTexture *bodyTex = *(RwTexture**)0x00B4E47C )
    {
        // The_GTA: remember the '#' flag at VEHICLE.TXD RwRemapScan we removed?
        // We can replace this logic by checking that this Texture belongs to VEHICLE.TXD!
        // After all, the texture has been picked up by the model loader, or it would not be here.
        if ( matTex->txd == g_vehicleTxd )
        {
            // Store the original texture information
            _StoreTextureInfo( storage, mat );

            // Set it to the paintjob texture
            mat->texture = bodyTex;
        }
    }

    if ( matTex == *(RwTexture**)0x00B4E68C )
    {
        unsigned int id = 0xFFFFFFFF;

        if ( color == 0xAFFF )
            id = 0;
        else if ( color == 0xC8FF00 )
            id = 1;
        else if ( color == 0xFFB9 )
            id = 2;
        else if ( color == 0x3CFF )
            id = 3;

        // Replace the light color
        _StoreColorInfo( storage, mat );

        // Make it white (keep alpha)
        mat->color |= 0xFFFFFF;
        
        if ( id != 0xFFFFFFFF && vehLightFlags[id] )
        {
            // Replace the lights texture
            _StoreTextureInfo( storage, mat );
            _StoreAmbientInfo( storage, mat );

            mat->texture = *(RwTexture**)0x00B4E690;
            mat->lighting.ambient = 16;
            mat->lighting.specular = 0;
            mat->lighting.diffuse = 0;
        }
    }
    else
    {
        // Decide which game logic to use.
        if ( gameVehicle )
        {
            SColor useColor;
            SColor color1, color2, color3, color4;
            gameVehicle->GetColor( &color1, &color2, &color3, &color4, 0 );

            // Each material seems to be individually colored
            if ( color == VEHICLE_COLOR_USECOLOR1 )
                useColor = color1;
            else if ( color == VEHICLE_COLOR_USECOLOR2 )
                useColor = color2;
            else if ( color == VEHICLE_COLOR_USECOLOR3 )
                useColor = color3;
            else if ( color == VEHICLE_COLOR_USECOLOR4 )
                useColor = color4;
            else
                return false;   // The_GTA: I made this return whether it saved color information; previously returned true.

            // We replace the color of this material.
            _StoreColorInfo( storage, mat );

            // MTA extension: apply our custom vehicle color
            mat->color.r = useColor.R;
            mat->color.g = useColor.G;
            mat->color.b = useColor.B;
        }
        else
        {
            unsigned char id;

            // Each material seems to be individually colored
            if ( color == VEHICLE_COLOR_USECOLOR1 )
                id = _vehColor1;
            else if ( color == VEHICLE_COLOR_USECOLOR2 )
                id = _vehColor2;
            else if ( color == VEHICLE_COLOR_USECOLOR3 )
                id = _vehColor3;
            else if ( color == VEHICLE_COLOR_USECOLOR4 )
                id = _vehColor4;
            else
                return false;

            // We replace the color of this material.
            _StoreColorInfo( storage, mat );

            // Default logic; choose a palette color
            RwColor& paletteColor = ((RwColor*)0x00B4E480)[id];
            mat->color.r = paletteColor.r;
            mat->color.g = paletteColor.g;
            mat->color.b = paletteColor.b;
        }
    }

    return true;
}

/*=========================================================
    RpClumpAtomicSetupVehicleMaterials

    Arguments:
        atomic - RenderWare atomic to set up for rendering
    Purpose:
        Prepares the color and texture information of the
        given atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C83E0
=========================================================*/
static unsigned char vehAlpha = 255;

static int RpClumpAtomicSetupVehicleMaterials( RpAtomic *atomic, _colorTextureStorage **storage )
{
    if ( !atomic->IsVisible() )
        return true;

    // The_GTA: the procedure of this function has changed from the GTA:SA version.
    RpMaterials& mats = atomic->geometry->materials;

    // Accelerate things by using only one loop
    bool blankOut = ( atomic->componentFlags & 0x1000 ) != 0;
    unsigned char alpha = vehAlpha;

    for ( unsigned int n = 0; n < mats.entries; n++ )
    {
        RpMaterial *mat = mats.data[n];

        if ( blankOut )
            RpGeometryMaterialSetupColor( mat, storage );
        else if ( alpha != 255 && vehMats.Add( mat ) )  // only perform once per material
        {
            bool savedColors = RpGeometryMaterialApplyVehicleColor( mat, storage );

            // Store the vehicle colors if they have not been saved yet
            if ( !savedColors )
                _StoreColorInfo( storage, mat );

            // Modify the alpha value
            mat->color.a = (unsigned char)( (float)mat->color.a * (float)alpha / 255.0f );
        }
        else    // It does not seem to break things
            RpGeometryMaterialApplyVehicleColor( mat, storage );
    }

    return true;
}

/*=========================================================
    RpClumpSetupVehicleMaterials

    Arguments:
        clump - vehicle clump to setup for rendering
        veh - MTA vehicle pointer to retrieve data from (vehicle color, ...)
    Purpose:
        Prepares the materials of all atomics in this clump
        for rendering by setting their color and replacing
        textures.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8430
=========================================================*/
static _colorTextureStorage _vehColorStorage[2048]; //original 256

void __cdecl RpClumpSetupVehicleMaterials( RpClump *clump, CVehicleSA *veh )
{
    _colorTextureStorage *storage = _vehColorStorage;

    // Set the game vehicle for this setup process
    gameVehicle = veh;
    vehAlpha = veh->GetAlpha();

    clump->ForAllAtomics( RpClumpAtomicSetupVehicleMaterials, &storage );
}

/*=========================================================
    RpClumpRestoreVehicleMaterials

    Arguments:
        clump - vehicle clump to leave rendering stage
    Purpose:
        Restores the clump data to the status prior to rendering
        (prior to RpClumpSetupVehicleMaterials).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8460
=========================================================*/
void __cdecl RpClumpRestoreVehicleMaterials( RpClump *clump )
{
    // We do not have to use the clump, since all pointers are pointing to it's data
    // Rockstar made use of a clever registering system.

    for ( _colorTextureStorage *entry = &_vehColorStorage[0]; entry->_matUnk != NULL; entry++ )
    {
        // Restore the value.
        *entry->_matUnk = entry->unk;

        // Bugfix: we should clean up after usage, so that the engine does not restore data it should keep alone.
        entry->_matUnk = NULL;
    }

    // MTA extension: Clear the list of vehicle materials (used so we process materials only once)
    vehMats.clear();
}

void VehicleModelInfoRender_Init( void )
{
    // Apply our own rendering logic ;)
    HookInstall( 0x004C7B10, h_memFunc( &CVehicleModelInfoSAInterface::RegisterRenderCallbacks ), 5 );

    // Temporary hook. Should be fixed by reversing Vehicle_AddUpgrade.
    HookInstall( 0x00733F80, (DWORD)RwAtomicRenderTranslucentHeli, 5 );
}

void VehicleModelInfoRender_SetupDevice( void )
{
}

void VehicleModelInfoRender_Reset( void )
{
    VehicleModelInfoRender_Shutdown();

    VehicleModelInfoRender_SetupDevice();
}

void VehicleModelInfoRender_Invalidate( void )
{
}

void VehicleModelInfoRender_Shutdown( void )
{
}