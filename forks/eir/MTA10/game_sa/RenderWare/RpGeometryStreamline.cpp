/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/renderware/RpGeometryStreamline.cpp
*  PURPOSE:     RenderWare geometry streamline threaded management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include <gamesa_renderware.h>

// MTA streamline extensions for multi-threaded calculations
static RpGeometry* RpGeometryStreamlineConstructor( RpGeometry *geom, size_t )
{
    geom->streamline.m_tasks = 0;
    LIST_INITNODE( geom->streamline.m_managerNode );
    return geom;
}

static void RpGeometryStreamlineDestructor( RpGeometry *geom, size_t )
{
    if ( geom->streamline.m_tasks )
        LIST_REMOVE( geom->streamline.m_managerNode );
}

static RpGeometry *RpGeometryStreamlineCopyConstructor( RpGeometry *dst, const RpGeometry *src, size_t, unsigned int )
{
    RpGeometryStreamlineConstructor( dst, sizeof( RpGeomStreamline ) );
    return dst;
}

void RpGeometryStreamlineInit( void )
{
    // Register ourselves to the geometry structures
    RpGeometryRegisterPlugin( sizeof( RpGeomStreamline ), RW_PLUGIN_STREAMLINE,
        RpGeometryStreamlineConstructor, RpGeometryStreamlineDestructor, RpGeometryStreamlineCopyConstructor );
}

void RpGeometryStreamlineShutdown( void )
{
}