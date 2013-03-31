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

void* CDummySAInterface::operator new( size_t )
{
    return (*ppDummyPool)->Allocate();
}

void CDummySAInterface::operator delete( void *ptr )
{
    (*ppDummyPool)->Free( (CDummySAInterface*)ptr );
}