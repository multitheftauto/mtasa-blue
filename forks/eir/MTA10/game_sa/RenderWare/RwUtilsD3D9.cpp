/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwUtilsD3D9.cpp
*  PURPOSE:     RenderWare utilities for Direct3D 9 interfacing
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "RwRenderTools.hxx"

bool __cdecl RwD3D9MatrixEqual( const D3DMATRIX& left, const D3DMATRIX& right )
{
    for ( unsigned int i = 0; i < 4; i++ )
    {
        for ( unsigned int j = 0; j < 4; j++ )
        {
            if ( left.m[i][j] != right.m[i][j] )
                return false;
        }
    }

    return true;
}

static D3DMATRIX** GetTransformationCache( void )
{
    return (D3DMATRIX**)0x00C97C70;
}

static const float identityMatrix[] =
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

// Binary offsets: (1.0 US): 0x007FA390
int __cdecl RwD3D9SetTransform( D3DTRANSFORMSTATETYPE state, const D3DMATRIX *matrix )
{
    D3DMATRIX **transCache = GetTransformationCache();

    const D3DMATRIX *setToMatrix = ( matrix ) ? matrix : (const D3DMATRIX*)identityMatrix;

    D3DMATRIX*& prevMatrix = transCache[state];

    if ( prevMatrix && RwD3D9MatrixEqual( *prevMatrix, *setToMatrix ) )
        return true;

    if ( !prevMatrix )
    {
        prevMatrix = (D3DMATRIX*)pRwInterface->m_allocStruct( *(RwStructInfo**)0x00C98080, 0x30411 );
    }

    *prevMatrix = *setToMatrix;

    bool result = GetRenderDevice_Native()->SetTransform( state, setToMatrix ) >= 0;

    // Is this an optimization? What is this?
    if ( state == 0x100 )
        *(int*)0x00C97C68 = ( matrix == NULL );

    return result;
}

void RenderWareUtilsD3D9_Init( void )
{
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        HookInstall( 0x007FA3D0, (DWORD)RwD3D9SetTransform, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x007FA390, (DWORD)RwD3D9SetTransform, 5 );
        break;
    }
}

void RenderWareUtilsD3D9_Shutdown( void )
{
}