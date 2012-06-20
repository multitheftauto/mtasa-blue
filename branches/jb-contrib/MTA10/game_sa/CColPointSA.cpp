/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColPointSA.cpp
*  PURPOSE:     Collision point
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CColPointSA::CColPointSA()
{ 
    this->internalInterface = new CColPointSAInterface(); 
}

VOID CColPointSA::SetPosition(CVector * vecPosition)  
{ 
    MemCpyFast (&this->GetInterface()->Position, vecPosition, sizeof(CVector)); 
}

VOID CColPointSA::SetNormal(CVector * vecNormal) 
{ 
    MemCpyFast (&this->GetInterface()->Normal, vecNormal, sizeof(CVector)); 
}
