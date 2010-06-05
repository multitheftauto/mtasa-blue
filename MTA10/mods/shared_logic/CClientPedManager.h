/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPedManager.h
*  PURPOSE:     Ped entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#ifndef __CClientPedManager_H
#define __CClientPedManager_H

#include <list>

class CClientManager;
class CClientPed;
class CPlayerPed;
class CEntity;

class CClientPedManager
{
    friend CClientManager;
    friend CClientPed;

public:
    void                            DeleteAll                       ( void );

    void                            DoPulse                         ( void );
    CClientPed*                     Get                             ( ElementID ID, bool bCheckPlayers = false );    
    CClientPed*                     Get                             ( CPlayerPed* pPlayer, bool bValidatePointer, bool bCheckPlayers );
    CClientPed*                     Get                             ( RpClump* pClump, bool bCheckPlayers );
    CClientPed*                     GetSafe                         ( CEntity * pEntity, bool bCheckPlayers );

    bool                            Exists                          ( CClientPed* pPed );

    std::vector < CClientPed* > ::const_iterator  IterBegin         ( void ) { return m_List.begin (); };
    std::vector < CClientPed* > ::const_iterator  IterEnd           ( void ) { return m_List.end (); };
    std::vector < CClientPed* > ::const_iterator  StreamedBegin     ( void ) { return m_StreamedIn.begin (); };
    std::vector < CClientPed* > ::const_iterator  StreamedEnd       ( void ) { return m_StreamedIn.end (); };

protected:
                                    CClientPedManager               ( class CClientManager* pManager );
                                    ~CClientPedManager              ( void );

    inline void                     AddToList                       ( CClientPed* pPed )        { m_List.push_back ( pPed ); }
    void                            RemoveFromList                  ( CClientPed* pPed );

    void                            OnCreation                      ( CClientPed * pPed );
    void                            OnDestruction                   ( CClientPed * pPed );

    CClientManager*                 m_pManager;
    std::vector < CClientPed* >     m_List;    
    std::vector < CClientPed* >     m_StreamedIn;
    bool                            m_bRemoveFromList;
};

#endif