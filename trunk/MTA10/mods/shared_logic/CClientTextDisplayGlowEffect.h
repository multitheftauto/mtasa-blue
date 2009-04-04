/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplayGlowEffect.h
*  PURPOSE:     Client text display glow effect class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCLIENTTEXTDISPLAYGLOWEFFECT_H
#define __CCLIENTTEXTDISPLAYGLOWEFFECT_H

#include "CClientTextDisplayEffect.h"

class CClientTextDisplayGlowEffect : public CClientTextDisplayEffect
{
public:
                        CClientTextDisplayGlowEffect        ( unsigned long ulPulseTime, unsigned char ucLowestAlpha = 128, unsigned char ucHighestAlpha = 255 );

protected:
    inline              ~CClientTextDisplayGlowEffect       ( void )    {};

    void                DoPulse                             ( void );

    unsigned long       m_ulPulseTime;
    unsigned char       m_ucLowestAlpha;
    unsigned char       m_ucHighestAlpha;

    unsigned long       m_ulLastPulse;
    bool                m_bPulsingOut;
};

#endif
