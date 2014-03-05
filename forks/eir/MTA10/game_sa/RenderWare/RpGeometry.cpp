/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpGeometry.cpp
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include <StdInc.h>

/*=========================================================
    RpGeometry::IsAlpha

    Purpose:
        Returns whether the geometry requires alpha blending.
        If it returns false, it may still require alpha blending
        since the texture might have an alpha channel.
=========================================================*/
bool RwMaterialAlphaCheck( RpMaterial *mat, int )
{
    return mat->color.a == 0xFF;
}

bool RpGeometry::IsAlpha( void )
{
    return !ForAllMateria( RwMaterialAlphaCheck, 0 );
}

/*=========================================================
    RpGeometry::UnlinkFX (MTA extension)

    Purpose:
        Scans through all assgined textures and effects of this 
        geometry and un-gracefully removes the references to them.
    Note:
        This function is outdated and hacky. Do not use it.
        It was used to debug the RenderWare texture system.
        We have since then developed much better ways.
=========================================================*/
bool RpMaterialTextureUnlink( RpMaterial *mat, int )
{
    if ( RwTexture *tex = mat->texture )
        mat->texture = NULL;

    return true;
}

void RpGeometry::UnlinkFX( void )
{
    // Clean all texture links
    ForAllMateria( RpMaterialTextureUnlink, 0 );

    if ( _2dfx )
    {
        // Clean the 2dfx structure
        RenderWare::GetInterface()->m_memory.m_free( _2dfx );
        _2dfx = NULL;
    }
}