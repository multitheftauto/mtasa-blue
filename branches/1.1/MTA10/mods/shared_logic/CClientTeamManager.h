/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTeamManager.h
*  PURPOSE:     Team entity manager class header
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*
*****************************************************************************/

class CClientTeamManager;

#ifndef __CCLIENTTEAMMANAGER_H
#define __CCLIENTTEAMMANAGER_H

#include <list>
#include "CClientTeam.h"

class CClientTeamManager
{
public:
                            CClientTeamManager      ( void );
                            ~CClientTeamManager     ( void );
    
    void                    AddToList               ( CClientTeam* pTeam )      { m_List.push_back ( pTeam ); }
    void                    RemoveFromList          ( CClientTeam* pTeam );
    void                    RemoveAll               ( void );
    bool                    Exists                  ( CClientTeam* pTeam );

    static CClientTeam*     GetTeam                 ( ElementID ID );
    CClientTeam*            GetTeam                 ( const char* szName );

    std::list < CClientTeam* > ::const_iterator IterBegin ( void )                    { return m_List.begin (); }
    std::list < CClientTeam* > ::const_iterator IterEnd   ( void )                    { return m_List.end (); }

private:
    std::list < CClientTeam* >  m_List;
    bool                        m_bRemoveFromList;
};

#endif
