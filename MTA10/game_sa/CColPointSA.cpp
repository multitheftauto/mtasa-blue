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

CColPointSA::CColPointSA()
{ 
    this->internalInterface = new CColPointSAInterface(); 
};

// Get lighting taking into account time of day
// Returns between 0 and 1
float CColPointSA::GetLightingForTimeOfDay ( void )
{
    BYTE ucCombo = GetLightingB ();
    float fDayValue = ucCombo & 0x0f;
    float fNightValue = ucCombo >> 4;

    float m_fDNBalanceParam = *(float*)(0x08D12C0);
    float fResult = Lerp ( fDayValue, m_fDNBalanceParam, fNightValue );
    return fResult * ( 1 / 15.f );
}
