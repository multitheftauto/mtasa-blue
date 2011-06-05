/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CElementGroup.h
*  PURPOSE:     Static element array management class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CELEMENTIDS_H
#define __CELEMENTIDS_H

#include "CElement.h"

class CElementIDs
{
public:
    static void                 Initialize          ( void );

    static ElementID            PopUniqueID         ( CElement* pElement );
    static ElementID            PopPlayerID         ( CPlayer* pPlayer );
    static void                 PushUniqueID        ( ElementID ID );
    static void                 PushUniqueID        ( CElement* pElement );

    static CElement*            GetElement          ( ElementID ID );

private:
    static CStack < ElementID, MAX_SERVER_ELEMENTS - MAX_PLAYER_COUNT, MAX_PLAYER_COUNT > m_UniqueIDs;
    static CElement*            m_Elements [MAX_SERVER_ELEMENTS];
};

#endif
