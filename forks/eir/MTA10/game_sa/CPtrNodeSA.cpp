/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPtrNodeSA.cpp
*  PURPOSE:     ???
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void* CPtrNodeSingleSA::operator new( size_t )
{
    return (*ppPtrNodeSinglePool)->Allocate();
}

void CPtrNodeSingleSA::operator delete( void *ptr )
{
    (*ppPtrNodeSinglePool)->Free( (CPtrNodeSingleSA*)ptr );
}

void* CPtrNodeDoubleSA::operator new( size_t )
{
    return (*ppPtrNodeDoublePool)->Allocate();
}

void CPtrNodeDoubleSA::operator delete( void *ptr )
{
    (*ppPtrNodeDoublePool)->Free( (CPtrNodeDoubleSA*)ptr );
}