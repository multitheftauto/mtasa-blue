/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPlayerManager.h
*  PURPOSE:     Player entity manager class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

// ## TODO ## : Make it possible to delete the local player without crashin ( m_pLocalPlayer not NULL-ed )

#ifndef __CCLIENTPLAYERMANAGER_H
#define __CCLIENTPLAYERMANAGER_H

#include <list>

class CClientManager;
class CClientPlayer;
class CPlayerPed;

class CClientPlayerManager
{
    friend CClientManager;
    friend CClientPlayer;

public:
    inline CClientPlayer*       GetLocalPlayer                  ( void )                                                { return m_pLocalPlayer; };
    inline void                 SetLocalPlayer                  ( CClientPlayer* pPlayer )                              { m_pLocalPlayer = pPlayer; };

    void                        DoPulse                         ( void );
    void                        DeleteAll                       ( void );

    inline unsigned int         Count                           ( void )                                                { return static_cast < unsigned int > ( m_Players.size () ); };
    static CClientPlayer*       Get                             ( ElementID ID );
    CClientPlayer*              Get                             ( const char* szNick, bool bCaseSensitive = false );
    CClientPlayer*              Get                             ( CPlayerPed* pPlayer, bool bValidatePointer );

    bool                        Exists                          ( CClientPlayer* pPlayer );

    static bool                 IsPlayerLimitReached            ( void );

    std::list < CClientPlayer* > ::const_iterator                IterGet             ( CClientPlayer* pPlayer );
    std::list < CClientPlayer* > ::const_reverse_iterator        IterGetReverse      ( CClientPlayer* pPlayer );
    inline std::list < CClientPlayer* > ::const_iterator         IterBegin           ( void )                                    { return m_Players.begin (); };
    inline std::list < CClientPlayer* > ::const_iterator         IterEnd             ( void )                                    { return m_Players.end (); };
    inline std::list < CClientPlayer* > ::const_reverse_iterator IterReverseBegin    ( void )                                    { return m_Players.rbegin (); };
    inline std::list < CClientPlayer* > ::const_reverse_iterator IterReverseEnd      ( void )                                    { return m_Players.rend (); };

    static bool                 IsValidModel                    ( unsigned long ulModel );

    void                        ResetAll                        ( void );

private:
                                    CClientPlayerManager            ( CClientManager* pManager );
                                    ~CClientPlayerManager           ( void );

    inline void                     AddToList                       ( CClientPlayer* pPlayer )                              { m_Players.push_back ( pPlayer ); };
    void                            RemoveFromList                  ( CClientPlayer* pPlayer );

    CClientManager*                 m_pManager;
    bool                            m_bCanRemoveFromList;
    std::list < CClientPlayer* >    m_Players;
    CClientPlayer*                  m_pLocalPlayer;
};

#endif
