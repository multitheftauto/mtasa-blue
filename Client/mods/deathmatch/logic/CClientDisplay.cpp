/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientDisplay.cpp
 *  PURPOSE:     Text display class
 *
 *****************************************************************************/

#include <StdInc.h>

CClientDisplay::CClientDisplay(unsigned long ulID)
{
    m_ulID = ulID;
    m_bVisible = true;
    m_Color = SColorRGBA(255, 255, 255, 255);
}

void CClientDisplay::SetColorAlpha(unsigned char ucAlpha)
{
    m_Color.A = ucAlpha;
}
