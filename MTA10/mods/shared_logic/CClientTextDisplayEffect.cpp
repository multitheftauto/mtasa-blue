/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplayEffect.cpp
*  PURPOSE:     Client text display effect class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

CClientTextDisplayEffect::CClientTextDisplayEffect ( void )
{
    // Init
    m_pTextDisplay = NULL;
    m_bDeleteMe = false;
    m_bNeverDelete = false;
}
