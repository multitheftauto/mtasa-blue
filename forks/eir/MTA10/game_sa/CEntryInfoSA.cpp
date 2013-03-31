/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntryInfoSA.cpp
*  PURPOSE:     ???
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void* CEntryInfoSA::operator new( size_t )
{
    return (*ppEntryInfoPool)->Allocate();
}

void CEntryInfoSA::operator delete( void *ptr )
{
    (*ppEntryInfoPool)->Free( (CEntryInfoSA*)ptr );
}