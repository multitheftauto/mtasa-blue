/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CDummySA.cpp
*  PURPOSE:     ???
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CDummySAInterface::CDummySAInterface( void )
{
    nType = ENTITY_TYPE_DUMMY;

    // Overwrite vtbl
    *(DWORD**)this = (DWORD*)0x008638C0;
}

void* CDummySAInterface::operator new( size_t )
{
    return Pools::GetDummyPool()->Allocate();
}

void CDummySAInterface::operator delete( void *ptr )
{
    Pools::GetDummyPool()->Free( (CDummySAInterface*)ptr );
}