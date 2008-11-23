/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CRemoteDataSA.h
*  PURPOSE:     Remote data storage class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CREMOTEDATASA
#define __CREMOTEDATASA

#include "multiplayersa_init.h"
#include <multiplayer/CMultiplayer.h>

#include <game/CPlayerPed.h>
#include <game/CPad.h>

// These includes have to be fixed!
#include "../game_sa/CPadSA.h"
#include "../game_sa/common.h"

class CRemoteDataStorageSA : public CRemoteDataStorage
{
public:
    inline CRemoteDataStorageSA ( CPlayerPed * player ) 
    { 
        m_pPlayer = player; 
        memset ( &m_pad, 0, sizeof ( CPadSAInterface ) );
        m_fCameraRotation = 0.0f;
        memset ( &m_stats, 0, sizeof ( CStatsData ) );
        m_fGravity = 0.008f;
        m_bAkimboTargetUp = false;
        m_bProcessPlayerWeapon = false;
    }

    inline CControllerState *   CurrentControllerState ( ) { return &m_pad.NewState; }
    inline CControllerState *   LastControllerState ( ) { return &m_pad.OldState; }
    inline CShotSyncData *      ShotSyncData ( ) { return &m_shotSyncData; }
    inline CStatsData *         Stats ( void ) { return &m_stats; }
    inline void                 SetCameraRotation ( float fCameraRotation ) { m_fCameraRotation = fCameraRotation; }
    inline float                GetCameraRotation ( void ) { return m_fCameraRotation; }
    inline void                 SetPlayer ( CPlayerPed * player ) { m_pPlayer = player; }
    inline void                 SetGravity ( float fGravity ) { m_fGravity = fGravity; }
    inline bool                 ProcessPlayerWeapon ( void ) { return m_bProcessPlayerWeapon; }
    inline void                 SetProcessPlayerWeapon ( bool bProcess )    { m_bProcessPlayerWeapon = bProcess; }

    CVector&                    GetAkimboTarget     ( void ) { return m_vecAkimboTarget; };
    bool                        GetAkimboTargetUp   ( void ) { return m_bAkimboTargetUp; };

    void                        SetAkimboTarget     ( const CVector& vecTarget ) { m_vecAkimboTarget = vecTarget; };
    void                        SetAkimboTargetUp   ( bool bUp ) { m_bAkimboTargetUp = bUp; };
    
    
    // The player that owns this class
    CPlayerPed *                m_pPlayer;

    // The player's pad
	CPadSAInterface		        m_pad;	
    float                       m_fCameraRotation;
    CShotSyncData               m_shotSyncData;
    CVector                     m_vecAkimboTarget;
    bool                        m_bAkimboTargetUp;
    CStatsData                  m_stats;
    float                       m_fGravity;
    bool                        m_bProcessPlayerWeapon;
};


class CRemoteDataSA
{
public:
    static                          Init ( );

    // Static accessors
    static CRemoteDataStorageSA *   GetRemoteDataStorage ( CPlayerPed * player );
    static CRemoteDataStorageSA *   GetRemoteDataStorage ( CPedSAInterface * ped );
    static bool                     AddRemoteDataStorage ( CRemoteDataStorage * data );
    static void                     RemoveRemoteDataStorage ( CRemoteDataStorage * data );

private:

};


#endif