/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rwstats.cpp
*  PURPOSE:     Record usage of certain Renderware resources
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

namespace
{
    SRwResourceStats ms_Stats;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwTextureCreate
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwTextureCreate_Pre( DWORD calledFrom )
{
}

void OnMY_RwTextureCreate_Post( RwTexture* pTexture, DWORD calledFrom )
{
    ms_Stats.uiTextures++;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwTextureDestroy
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwTextureDestroy( RwTexture* pTexture, DWORD calledFrom )
{
    if ( pTexture->refs == 1 )
        ms_Stats.uiTextures--;
}

static RwTexture* __cdecl _RwTextureStatsConstructor( RwTexture *tex, size_t pluginOffset )
{
    OnMY_RwTextureCreate_Pre( 0 );
    OnMY_RwTextureCreate_Post( tex, 0 );
    return tex;
}

static void __cdecl _RwTextureStatsDestructor( RwTexture *tex, size_t pluginOffset )
{
    OnMY_RwTextureDestroy( tex, 0 );
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwRasterCreate
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwRasterCreate( DWORD calledFrom )
{
    ms_Stats.uiRasters++;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwRasterDestroy
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwRasterDestroy( DWORD calledFrom )
{
    ms_Stats.uiRasters--;
}

static RwRaster* __cdecl _RwRasterStatsConstructor( RwRaster *raster, size_t pluginOffset )
{
    OnMY_RwRasterCreate( 0 );
    return raster;
}

static void __cdecl _RwRasterStatsDestructor( RwRaster *raster, size_t pluginOffset )
{
    OnMY_RwRasterDestroy( 0 );
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// RwGeometryCreate
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwGeometryCreate_Pre( DWORD calledFrom )
{
}

void OnMY_RwGeometryCreate_Post( RwGeometry* pGeometry, DWORD calledFrom )
{
    ms_Stats.uiGeometries++;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// RwGeometryDestroy
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_RwGeometryDestroy( DWORD calledFrom, RwGeometry* pGeometry )
{
    if ( pGeometry->refs == 1 )
        ms_Stats.uiGeometries--;
}

static RwGeometry* __cdecl _RpGeometryStatsConstructor( RpGeometry *geom, size_t pluginOffset )
{
    OnMY_RwGeometryCreate_Pre( 0 );
    OnMY_RwGeometryCreate_Post( geom, 0 );
    return geom;
}

static void __cdecl _RpGeometryStatsDestructor( RpGeometry *geom, size_t pluginOffset )
{
    OnMY_RwGeometryDestroy( 0, geom );
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CGameSA::GetRwResourceStats
//
//
//////////////////////////////////////////////////////////////////////////////////////////
void CGameSA::GetRwResourceStats ( SRwResourceStats& outStats )
{
    outStats = ms_Stats;
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CGameSA::InitHooks_RwResources
//
// Setup hooks for RwResources
//
//////////////////////////////////////////////////////////////////////////////////////////
void CGameSA::InitHooks_RwResources ( void )
{
    memset ( &ms_Stats, 0, sizeof ( ms_Stats ) );

    RwTextureRegisterPlugin( 0, 0xDEAD1001,
        (RwTexturePluginConstructor)_RwTextureStatsConstructor,
        (RwTexturePluginDestructor)_RwTextureStatsDestructor,
        NULL
    );

    RwRasterRegisterPlugin( 0, 0xDEAD1002,
        (RwRasterPluginConstructor)_RwRasterStatsConstructor,
        (RwRasterPluginDestructor)_RwRasterStatsDestructor,
        NULL
    );

    RpGeometryRegisterPlugin( 0, 0xDEAD1000,
        (RpGeometryPluginConstructor)_RpGeometryStatsConstructor,
        (RpGeometryPluginDestructor)_RpGeometryStatsDestructor,
        NULL
    );
}
