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
    static void                 PushUniqueID        ( ElementID ID );
    static void                 PushUniqueID        ( CElement* pElement );

    static CElement*            GetElement          ( ElementID ID );

private:
    static CStack < ElementID, MAX_SERVER_ELEMENTS - 2 > m_UniqueIDs;
    static SFixedArray < CElement*, MAX_SERVER_ELEMENTS >  m_Elements;
};

#endif
