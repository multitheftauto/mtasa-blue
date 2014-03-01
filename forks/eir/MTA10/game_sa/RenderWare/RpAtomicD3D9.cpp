/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpAtomicD3D9.cpp
*  PURPOSE:     RenderWare Atomic Direct3D 9 plugin definitions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

void* CEnvMapAtomicSA::operator new ( size_t )
{
    return (*ppEnvMapAtomicPool)->Allocate();
}

void CEnvMapAtomicSA::operator delete ( void *ptr )
{
    (*ppEnvMapAtomicPool)->Free( (CEnvMapAtomicSA*)ptr );
}