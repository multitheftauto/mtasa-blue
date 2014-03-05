/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CQuadTreeSA.cpp
*  PURPOSE:     Quad Tree world engine
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// Pool of all quad tree nodes.
CQuadTreeNodePool **ppQuadTreeNodePool = (CQuadTreeNodePool**)0x00B745BC;

/*=========================================================
    InitQuadTreeNodes

    Purpose:
        Initializes the quad tree node system by allocating
        the pool.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00552C00
=========================================================*/
void __cdecl InitQuadTreeNodes( void )
{
    *ppQuadTreeNodePool = new CQuadTreeNodePool;
}

void QuadTree_Init( void )
{
    // Hook the usual routines.
    HookInstall( 0x00552C00, (DWORD)InitQuadTreeNodes, 5 );
}

void QuadTree_Shutdown( void )
{
}