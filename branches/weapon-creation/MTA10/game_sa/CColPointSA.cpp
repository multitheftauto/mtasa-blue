/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColPointSA.cpp
*  PURPOSE:     Collision point
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CColPointSA::CColPointSA ( void )
{ 
    m_pInternalInterface = new CColPointSAInterface(); 
};

// Get lighting taking into account time of day
// Returns between 0 and 1
float CColPointSA::GetLightingForTimeOfDay ( void )
{
    CColLighting lighting = GetLightingB ();

    // Find correct position between night and day value
    float m_fDNBalanceParam = *(float*)(0x08D12C0);
    float fResult = Lerp < float > ( lighting.day, m_fDNBalanceParam, lighting.night );

    // Scale to range 0-1
    return fResult * ( 1 / 15.f );
}
