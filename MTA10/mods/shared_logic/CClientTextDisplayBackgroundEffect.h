/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplayBackgroundEffect.h
*  PURPOSE:     Client text display background effect class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CCLIENTTEXTDISPLAYBACKGROUNDEFFECT_H
#define __CCLIENTTEXTDISPLAYBACKGROUNDEFFECT_H

#include "CClientTextDisplayEffect.h"

class CClientTextDisplayBackgroundEffect : public CClientTextDisplayEffect
{
public:
                    CClientTextDisplayBackgroundEffect      ( void );
                    CClientTextDisplayBackgroundEffect      ( unsigned long ulColor );

protected:
    inline          ~CClientTextDisplayBackgroundEffect     ( void )    {};

    void            DoPulse                                 ( void );

    unsigned long   m_ulColor;
};

#endif
