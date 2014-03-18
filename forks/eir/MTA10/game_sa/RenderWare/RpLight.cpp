/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpLight.cpp
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include <StdInc.h>
#include "../gamesa_renderware.h"

/*=========================================================
    RpLight::AddToClump

    Arguments:
        _clump - model registry to put this light into
    Purpose:
        Registers this light at another clump. It unregisters
        it from the previous clump.
    Binary offsets:
        (1.0 US): 0x0074A4F0
        (1.0 EU): 0x0074A540
    Note:
        At the binary offset location actually is
        RpClumpAddLight.
=========================================================*/
void RpLight::AddToClump( RpClump *clump )
{
    // Bugfix: remove from previous clump
    RemoveFromClump();

    LIST_INSERT( clump->lights.root, clumpLights );

    this->clump = clump;
}

RpClump* __cdecl RpClumpAddLight( RpClump *clump, RpLight *light )      { light->AddToClump( clump ); return clump; }
/*=========================================================
    RpLight::RemoveFromClump

    Purpose:
        Removes this light from any clump it might be registered at.
    Binary offsets:
        (1.0 US): 0x0074A520
        (1.0 EU): 0x0074A570
    Note:
        At the binary offset location actually is
        RpClumpRemoveLight.
=========================================================*/
void RpLight::RemoveFromClump( void )
{
    if ( !clump )
        return;

    LIST_REMOVE( clumpLights );

    clump = NULL;
}

/*=========================================================
    RpLight::AddToScene

    Arguments:
        scene - scene to register this light at
    Purpose:
        Puts this light into a scene. It will interact with
        all atomics inside of the scene. Global lights are applied
        to all atomics without position preference. Local lights
        are tailored along sectors, applying to in-range atomics.
    Binary offsets:
        (1.0 US): 0x00751910
        (1.0 EU): 0x00751960
    Note:
        At the binary offset location actually is
        RwSceneAddLight.
=========================================================*/
void RpLight::AddToScene_Global( RwScene *scene )
{
    RemoveFromScene();

    this->scene = scene;

    LIST_INSERT( scene->globalLights.root, sceneLights );
}

void RpLight::AddToScene_Local( RwScene *scene )
{
    RemoveFromScene();

    this->scene = scene;

    if ( scene->parent )
        scene->parent->Update();

    LIST_INSERT( scene->localLights.root, sceneLights );
}

void RpLight::AddToScene( RwScene *scene )
{
    if ( subtype < 0x80 )
        AddToScene_Global( scene );
    else
        AddToScene_Local( scene );
}

/*=========================================================
    RpLight::RemoveFromScene

    Purpose:
        Unregisters this light from a scene it might be inside.
    Binary offsets:
        (1.0 US): 0x00751960
        (1.0 EU): 0x007519B0
    Note:
        At the binary offset location actually is
        RwSceneRemoveLight.
=========================================================*/
void RpLight::RemoveFromScene( void )
{
    if ( !scene )
        return;

    RwSceneRemoveLight( scene, this );
}

/*=========================================================
    RpLight::SetColor

    Arguments:
        color - new color to set the light to
    Purpose:
        Changes the light's color to another. If the color is
        brightness only, it sets privateFlags to 1; otherwise 0.
    Binary offsets:
        (1.0 US): 0x00751A90
        (1.0 EU): 0x00751AE0
=========================================================*/
void RpLight::SetColor( const RwColorFloat& color )
{
    this->color = color;
    
    // Check whether we are brightness only
    privateFlags = ( color.r == color.g && color.r == color.b );
}

void __cdecl RpLightSetColor( RpLight *light, const RwColorFloat& color )   { light->SetColor( color ); }
void __cdecl RpLightGetColor( RpLight *light, RwColorFloat& colorOut )      { colorOut = light->color; }
/*=========================================================
    RpLightCreate

    Arguments:
        type - type identifier of the new light
               see RpLightType enum (LIGHT_TYPE_* )
    Purpose:
        Creates a new RpLight plugin instance and registers
        it into the system. It assigns a light-type to it.
        This light-type may not be changed during the light's
        lifetime.
    Binary offsets:
        (1.0 US): 0x00752110
        (1.0 EU): 0x00752160
=========================================================*/
static void* _lightCallback( void *ptr )
{
    // Nothing to synchronize for lights!
    return ptr;
}

RpLight* RpLightCreate( unsigned char type )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    RpLight *light = (RpLight*)rwInterface->m_allocStruct( rwInterface->m_lightInfo, 0x30012 );

    if ( !light )
        return NULL;

    light->type = RW_LIGHT;
    light->subtype = type;
    light->color.a = 0;
    light->color.r = 0;
    light->color.g = 0;
    light->color.b = 0;

    light->callback = (RwObjectFrame::syncCallback_t)_lightCallback;
    light->flags = 0;
    light->parent = NULL;

    light->radius = 0;
    light->coneAngle = 0;
    light->privateFlags = 0;

    // Clear the list awareness
    LIST_CLEAR( light->sectors.root );
    LIST_INITNODE( light->clumpLights );

    light->flags = 3; // why write it again? R* hack?
    light->frame = rwInterface->m_frame;

    RwObjectRegister( (void*)0x008D62F8, light );
    return light;
}

// NEW RenderWare function for MTA
RpLight* RpLightClone( const RpLight *src )
{
    RpLight *obj = RpLightCreate( src->subtype );

    if ( !obj )
        return NULL;

    // Copy values over
    obj->flags = src->flags;
    obj->privateFlags = src->privateFlags;
    obj->RemoveFromFrame();

    // Copy important data
    obj->radius = src->radius;
    obj->color = src->color;
    obj->coneAngle = src->coneAngle;
    obj->unk = src->unk;

    // Clone plugin details.
    ((RwPluginRegistry <RpLight>*)0x008D62F8)->CloneObject( obj, src );
    return obj;
}

/*===================================================================
    RpLight Frustum Caching Plugin
===================================================================*/

static int _lightFrustumCachePluginOffset = -1;

struct _lightFrustumCache
{
    bool isInsideFrustum;
};

static RpLight* __cdecl _RpLightFrustumCacheConstructor( RpLight *light, size_t pluginOffset, unsigned int pluginId )
{
    _lightFrustumCache *cache = RW_PLUGINSTRUCT <_lightFrustumCache> ( light, pluginOffset );

    cache->isInsideFrustum = true;
    return light;
}

static void __cdecl _RpLightFrustumCacheDestructor( RpLight *light, size_t pluginOffset )
{
    return;
}

bool __cdecl RpLightIsFrustumCachable( RpLight *light )
{
    unsigned char subType = light->subtype;

    if ( !light->parent )
        return false;

    return subType == LIGHT_TYPE_POINT ||
           subType == LIGHT_TYPE_SPOT_1 ||
           subType == LIGHT_TYPE_SPOT_2;
}

bool __cdecl RpLightIsInsideFrustum( RpLight *light )
{
    return RW_PLUGINSTRUCT <_lightFrustumCache> ( light, _lightFrustumCachePluginOffset )->isInsideFrustum;
}

void __cdecl RpLightPerformFrustumCaching( void )
{
    RwInterface *rwInterface = RenderWare::GetInterface();

    RwScene *scene = *p_gtaScene;
    CCameraSAInterface& camera = Camera::GetInterface();

    if ( !scene )
        return;

    LIST_FOREACH_BEGIN( RpLight, scene->localLights.root, sceneLights )
        if ( RpLightIsFrustumCachable( item ) )
        {
            _lightFrustumCache *cache = RW_PLUGINSTRUCT <_lightFrustumCache> ( item, _lightFrustumCachePluginOffset );

            cache->isInsideFrustum = camera.IsSphereVisible( item->parent->GetLTM().vPos, item->radius, (void*)0x00B6FA74 );
        }
    LIST_FOREACH_END
}

// General light initialization
void __cdecl RpLightInit( void )
{
    _lightFrustumCachePluginOffset = RpLightRegisterPlugin( sizeof( _lightFrustumCache ), 0xDEAD1005,
        (RpLightPluginConstructor)_RpLightFrustumCacheConstructor,
        (RpLightPluginDestructor)_RpLightFrustumCacheDestructor,
        NULL
    );
}

void __cdecl RpLightShutdown( void )
{
}