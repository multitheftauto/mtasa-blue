/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRouteSA.cpp
*  PURPOSE:     Route for navigation
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void* CPointRouteSA::operator new( size_t )
{
    return (*ppPointRoutePool)->Allocate();
}

void CPointRouteSA::operator delete( void *ptr )
{
    (*ppPointRoutePool)->Free( (CPointRouteSA*)ptr );
}

void* CPatrolRouteSA::operator new( size_t )
{
    return (*ppPatrolRoutePool)->Allocate();
}

void CPatrolRouteSA::operator delete( void *ptr )
{
    (*ppPatrolRoutePool)->Free( (CPatrolRouteSA*)ptr );
}

void* CNodeRouteSA::operator new( size_t )
{
    return (*ppNodeRoutePool)->Allocate();
}

void CNodeRouteSA::operator delete( void *ptr )
{
    (*ppNodeRoutePool)->Free( (CNodeRouteSA*)ptr );
}