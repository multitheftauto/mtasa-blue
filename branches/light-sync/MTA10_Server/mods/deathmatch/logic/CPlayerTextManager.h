/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerTextManager.h
*  PURPOSE:     Player text display manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayerTextManager;

#ifndef __CPLAYERTEXTMANAGER_H
#define __CPLAYERTEXTMANAGER_H

//#include <queue>
#include <list>
#include "CTextItem.h"
#include "CPlayer.h"
#include "CTextDisplay.h"

class CPlayerTextManager
{
    friend class CTextDisplay;

private:
    CPlayer*                    m_pPlayer;
    std::list < CTextItem* >    m_highQueue;
    std::list < CTextItem* >    m_mediumQueue;
    std::list < CTextItem* >    m_lowQueue;

    std::list < CTextDisplay* > m_displays;

    CTextItem *                 GetTextItemOnQueue ( CTextItem * textItem );

public:
                                CPlayerTextManager          ( CPlayer * player );
                                ~CPlayerTextManager         ( void );

    void                        Update                      ( CTextItem * textItem, bool bRemovedFromDisplay = false );
    void                        Process                     ( void );

    CPlayer*                    GetPlayer                   ( void );
};

#endif
