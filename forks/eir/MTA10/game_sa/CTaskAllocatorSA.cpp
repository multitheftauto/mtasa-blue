/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTaskAllocatorSA.cpp
*  PURPOSE:     ???
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void* CTaskAllocatorSA::operator new( size_t )
{
    return (*ppTaskAllocatorPool)->Allocate();
}

void CTaskAllocatorSA::operator delete( void *ptr )
{
    (*ppTaskAllocatorPool)->Free( (CTaskAllocatorSA*)ptr );
}