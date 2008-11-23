/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplayEffect.h
*  PURPOSE:     Client text display effect class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

class CClientTextDisplayEffect;

#ifndef __CCLIENTTEXTDISPLAYEFFECT_H
#define __CCLIENTTEXTDISPLAYEFFECT_H

#include "CClientTextDisplay.h"

class CClientTextDisplayEffect
{
    friend CClientTextDisplay;

public:
                            CClientTextDisplayEffect        ( void );

    inline bool             GetNeverDelete                  ( void )                { return m_bNeverDelete; };
    inline void             SetNeverDelete                  ( bool bNeverDelete )   { m_bNeverDelete = bNeverDelete; };

protected:
    virtual                 ~CClientTextDisplayEffect       ( void ) {};

    virtual void            DoPulse                         ( void ) = 0;

    CClientTextDisplay*     m_pTextDisplay;
    bool                    m_bDeleteMe;
    bool                    m_bNeverDelete;

private:
    inline void             SetTextDisplay                  ( CClientTextDisplay* pTextDisplay )    { m_pTextDisplay = pTextDisplay; };
    inline bool             CanDeleteMe                     ( void )                                { return m_bDeleteMe; };
};

#endif
