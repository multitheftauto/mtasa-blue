/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPlayer.h
*  PURPOSE:     Player entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Oliver Brown <>
*               Kent Simon <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*****************************************************************************/

class CClientPlayer;

#ifndef __CCLIENTPLAYER_H
#define __CCLIENTPLAYER_H

#include <game/CPlayerPed.h>

#include "CClientCommon.h"
#include "CClientPed.h"

#define NUM_PLAYER_STATS 343

class CClientPlayerManager;
class CClientTeam;

class CClientPlayer : public CClientPed
{
    friend class CClientPlayerManager;

public:
                                    CClientPlayer           ( CClientManager* pManager, ElementID ID, bool bIsLocalPlayer = false );
    virtual                         ~CClientPlayer          ( void );

    void                            Unlink                  ( void );

    eClientEntityType               GetType                 ( void ) const                          { return CCLIENTPLAYER; }

    const char*                     GetNick                 ( void ) const                          { return m_szNick; }
    void                            SetNick                 ( const char* szNick );

    inline unsigned int             GetPing                 ( void )                                { return ( m_bIsLocalPlayer ) ? g_pNet->GetPing () : m_uiPing; }
    inline void                     SetPing                 ( unsigned int uiPing )                 { m_uiPing = uiPing; }

    void                            GetNametagColor         ( unsigned char& ucR, unsigned char& ucG, unsigned char& ucB );
    void                            SetNametagOverrideColor ( unsigned char ucR, unsigned char ucG, unsigned char ucB );
    void                            RemoveNametagOverrideColor  ( void );
    inline bool                     IsNametagColorOverridden( void )                                { return m_bNametagColorOverridden; }

    inline const char*              GetNametagText          ( void )                                { return m_strNametag.c_str (); }
    inline void                     SetNametagText          ( const char* szText );
    inline bool                     IsNametagShowing        ( void )                                { return m_bNametagShowing; }
    inline void                     SetNametagShowing       ( bool bShowing )                       { m_bNametagShowing = bShowing; }
    inline CGUIStaticImage*         GetStatusIcon           ( void )                                { return m_pStatusIcon; }
    inline unsigned long            GetLastNametagShow      ( void )                                { return m_ulLastNametagShow; }
    inline void                     SetLastNametagShow      ( unsigned long ulTime )                { m_ulLastNametagShow = ulTime; }

    inline void                     SetIsExtrapolatingAim   ( bool m_bExtrap )                      { m_bDoExtrapolatingAim = m_bExtrap; }
    inline bool                     IsExtrapolatingAim      ( void )                                { return m_bDoExtrapolatingAim; }
    void                            UpdateAimPosition       ( const CVector &vecAim );

    inline unsigned short           GetLatency              ( void )                                { return m_usLatency; }
    inline void                     SetLatency              ( unsigned short usLatency )            { m_usLatency = (m_usLatency + usLatency) / 2; }

    inline unsigned long            GetLastPuresyncTime     ( void )                                { return m_ulLastPuresyncTime; }
    inline void                     SetLastPuresyncTime     ( unsigned long ulLastPuresyncTime )    { m_ulLastPuresyncTime = ulLastPuresyncTime; }
    inline const CVector&           GetLastPuresyncPosition ( void )                                { return m_vecLastPuresyncPosition; }
    inline void                     SetLastPuresyncPosition ( const CVector& vecPosition )          { m_vecLastPuresyncPosition = vecPosition; }
    inline bool                     HasConnectionTrouble    ( void )                                { return m_bHasConnectionTrouble; }
    inline void                     SetHasConnectionTrouble ( bool bHasTrouble )                    { m_bHasConnectionTrouble = bHasTrouble; }

    inline void                     IncrementPlayerSync     ( void )                                { ++m_uiPlayerSyncCount; }
    inline void                     IncrementKeySync        ( void )                                { ++m_uiKeySyncCount; }
    inline void                     IncrementVehicleSync    ( void )                                { ++m_uiVehicleSyncCount; }

    inline unsigned int             GetPlayerSyncCount      ( void )                                { return m_uiPlayerSyncCount; }
    inline unsigned int             GetKeySyncCount         ( void )                                { return m_uiKeySyncCount; }
    inline unsigned int             GetVehicleSyncCount     ( void )                                { return m_uiVehicleSyncCount; }
    
    inline CClientTeam*             GetTeam                 ( void )                                { return m_pTeam; }
    void                            SetTeam                 ( CClientTeam* pTeam, bool bChangeTeam = false);
    bool                            IsOnMyTeam              ( CClientPlayer* pPlayer );

    inline float                    GetNametagDistance      ( void )                                { return m_fNametagDistance; }
    inline void                     SetNametagDistance      ( float fDistance )                     { m_fNametagDistance = fDistance; }

    inline bool                     IsDeadOnNetwork         ( void )                                { return m_bNetworkDead; }
    inline void                     SetDeadOnNetwork        ( bool bDead )                          { m_bNetworkDead = bDead; }

    void                            Reset                   ( void );

    inline CClientManager*          GetManager              ( void )                                { return m_pManager; }

private:
    bool                            m_bIsLocalPlayer;
    char                            m_szNick [ MAX_PLAYER_NICK_LENGTH + 1 ];

    unsigned int                    m_uiPing;

    CVector                         m_vecTargetPosition;
    CVector                         m_vecTargetMoveSpeed;
    CVector                         m_vecTargetTurnSpeed;
    CVector                         m_vecTargetIncrements;
    unsigned int                    m_uiFramesSincePositionUpdate;

    unsigned short                  m_usLatency;
    CVector                         m_vecAimSpeed;
    CVector                         m_vecOldAim;
    CVector                         m_vecCurrentAim;
    unsigned long                   m_ulOldAimTime;
    unsigned long                   m_ulCurrentAimTime;
    unsigned long                   m_ulTick;
    bool                            m_bDoExtrapolatingAim;

    bool                            m_bForce;
    CVector                         m_vecForcedMoveSpeed;
    CVector                         m_vecForcedTurnSpeed;    

    unsigned long                   m_ulLastPuresyncTime;
    CVector                         m_vecLastPuresyncPosition;
    bool                            m_bHasConnectionTrouble;

    CClientTeam*                    m_pTeam;

    CGUIStaticImage*                m_pStatusIcon;
    bool                            m_bNametagShowing;
    unsigned long                   m_ulLastNametagShow;
    unsigned char                   m_ucNametagColorR, m_ucNametagColorG, m_ucNametagColorB;
    bool                            m_bNametagColorOverridden;
    std::string                     m_strNametag;

    unsigned int                    m_uiPlayerSyncCount;
    unsigned int                    m_uiKeySyncCount;
    unsigned int                    m_uiVehicleSyncCount;

    float                           m_fNametagDistance;

    bool                            m_bNetworkDead;

#ifdef MTA_DEBUG
private:
    bool                            m_bShowingWepdata;
public:
    inline void                     SetShowingWepdata       ( bool bState ) { m_bShowingWepdata = bState; }
    inline bool                     IsShowingWepdata        ( ) const       { return m_bShowingWepdata; }
#endif

};

#endif
