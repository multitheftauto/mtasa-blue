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
    friend class CClientManager;
    friend class CClientPed;

public:
    void                            DeleteAll                       ( void );

    void                            DoPulse                         ( bool bDoStandardPulses );
    CClientPed*                     Get                             ( ElementID ID, bool bCheckPlayers = false );    
    CClientPed*                     Get                             ( CPlayerPed* pPlayer, bool bValidatePointer, bool bCheckPlayers );
    CClientPed*                     GetSafe                         ( CEntity * pEntity, bool bCheckPlayers );

    bool                            Exists                          ( CClientPed* pPed );

    const std::vector < CClientPed* >& GetPeds                      ( void ) { return m_List;  }
    const std::vector < CClientPed* >& GetStreamedPeds              ( void ) { return m_StreamedIn; }

    static unsigned short           GetWeaponSlotFromModel          ( DWORD dwModelID );
    static bool                     IsValidWeaponModel              ( DWORD dwModelID );

    void                            RestreamPeds                    ( unsigned short usModel );
    void                            RestreamWeapon                  ( unsigned short usModel );


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
