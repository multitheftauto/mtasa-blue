/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplayFadeEffect.h
*  PURPOSE:     Client text display fade effect class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCLIENTTEXTDISPLAYFADEEFFECT_H
#define __CCLIENTTEXTDISPLAYFADEEFFECT_H

#include "CClientTextDisplayEffect.h"
#include <ctime>

using namespace std;

enum
{
    TEXTDISPLAY_FADE_IN = 1,
    TEXTDISPLAY_FADE_OUT = 2,
};

class CClientTextDisplayFadeEffect : public CClientTextDisplayEffect
{
public:
                    CClientTextDisplayFadeEffect        ( unsigned long ulTime, unsigned long ulFlags );

protected:
    inline          ~CClientTextDisplayFadeEffect       ( void )    {};

    void            DoPulse                             ( void );

    unsigned long   m_ulTime;
    bool            m_bFadeIn;
    bool            m_bFadeOut;

    unsigned long   m_ulStart;
};

#endif
