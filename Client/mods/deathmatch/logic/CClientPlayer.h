/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPlayer.h
 *  PURPOSE:     Player entity class header
 *
 *****************************************************************************/

class CClientPlayer;

#ifndef __CCLIENTPLAYER_H
#define __CCLIENTPLAYER_H

#include <game/CPlayerPed.h>

#include "CClientCommon.h"
#include "CClientPed.h"
#include "CClientPlayerVoice.h"

#define NUM_PLAYER_STATS 343

class CClientPlayerManager;
class CClientTeam;
enum ePuresyncType
{
    PURESYNC_TYPE_NONE,
    PURESYNC_TYPE_LIGHTSYNC,
    PURESYNC_TYPE_PURESYNC,
};
class CClientPlayer : public CClientPed
{
    DECLARE_CLASS(CClientPlayer, CClientPed)
    friend class CClientPlayerManager;

public:
    CClientPlayer(CClientManager* pManager, ElementID ID, bool bIsLocalPlayer = false);
    virtual ~CClientPlayer(void);

    void Unlink(void);

    eClientEntityType GetType(void) const { return CCLIENTPLAYER; }

    const char* GetNick(void) const { return m_strNick; }
    void        SetNick(const char* szNick);

    unsigned int GetPing(void) { return (m_bIsLocalPlayer) ? g_pNet->GetPing() : m_uiPing; }
    void         SetPing(unsigned int uiPing) { m_uiPing = uiPing; }

    void GetNametagColor(unsigned char& ucR, unsigned char& ucG, unsigned char& ucB);
    void SetNametagOverrideColor(unsigned char ucR, unsigned char ucG, unsigned char ucB);
    void RemoveNametagOverrideColor(void);
    bool IsNametagColorOverridden(void) { return m_bNametagColorOverridden; }

    const char*   GetNametagText(void) { return m_strNametag.c_str(); }
    void          SetNametagText(const char* szText);
    bool          IsNametagShowing(void) { return m_bNametagShowing; }
    void          SetNametagShowing(bool bShowing) { m_bNametagShowing = bShowing; }
    unsigned long GetLastNametagShow(void) { return m_ulLastNametagShow; }
    void          SetLastNametagShow(unsigned long ulTime) { m_ulLastNametagShow = ulTime; }

    void SetIsExtrapolatingAim(bool m_bExtrap) { m_bDoExtrapolatingAim = m_bExtrap; }
    bool IsExtrapolatingAim(void) { return m_bDoExtrapolatingAim; }
    void UpdateAimPosition(const CVector& vecAim);

    unsigned short GetLatency(void) { return m_usLatency; }
    void           SetLatency(unsigned short usLatency) { m_usLatency = (m_usLatency + usLatency) / 2; }

    unsigned long  GetLastPuresyncTime(void) { return m_ulLastPuresyncTime; }
    void           SetLastPuresyncTime(unsigned long ulLastPuresyncTime) { m_ulLastPuresyncTime = ulLastPuresyncTime; }
    const CVector& GetLastPuresyncPosition(void) { return m_vecLastPuresyncPosition; }
    void           SetLastPuresyncPosition(const CVector& vecPosition) { m_vecLastPuresyncPosition = vecPosition; }
    bool           HasConnectionTrouble(void) { return m_bHasConnectionTrouble; }
    void           SetHasConnectionTrouble(bool bHasTrouble) { m_bHasConnectionTrouble = bHasTrouble; }
    ePuresyncType  GetLastPuresyncType(void) { return m_LastPuresyncType; }
    void           SetLastPuresyncType(ePuresyncType LastPuresyncType) { m_LastPuresyncType = LastPuresyncType; }
    void           SetLightsyncCalcedVelocity(const CVector& vecVelocity) { m_vecLightsyncCalcedVelocity = vecVelocity; }
    const CVector& GetLightsyncCalcedVelocity(void) { return m_vecLightsyncCalcedVelocity; }
    void           IncrementPlayerSync(void) { ++m_uiPlayerSyncCount; }
    void           IncrementKeySync(void) { ++m_uiKeySyncCount; }
    void           IncrementVehicleSync(void) { ++m_uiVehicleSyncCount; }

    unsigned int GetPlayerSyncCount(void) { return m_uiPlayerSyncCount; }
    unsigned int GetKeySyncCount(void) { return m_uiKeySyncCount; }
    unsigned int GetVehicleSyncCount(void) { return m_uiVehicleSyncCount; }

    CClientTeam* GetTeam(void) { return m_pTeam; }
    void         SetTeam(CClientTeam* pTeam, bool bChangeTeam);
    bool         IsOnMyTeam(CClientPlayer* pPlayer);

    CClientPlayerVoice* GetVoice(void) { return m_voice; }
    void                SetPlayerVoice(CClientPlayerVoice* voice) { m_voice = voice; }

    float GetNametagDistance(void) { return m_fNametagDistance; }
    void  SetNametagDistance(float fDistance) { m_fNametagDistance = fDistance; }

    bool IsDeadOnNetwork(void) { return m_bNetworkDead; }
    void SetDeadOnNetwork(bool bDead) { m_bNetworkDead = bDead; }

    void Reset(void);

    CClientManager* GetManager(void) { return m_pManager; }

    void DischargeWeapon(eWeaponType weaponType, const CVector& vecStart, const CVector& vecEnd, float fBackupDamage, uchar ucBackupHitZone,
                         CClientPlayer* pBackupDamagedPlayer);

    void   SetRemoteVersionInfo(ushort usBitstreamVersion, uint uiBuildNumber);
    ushort GetRemoteBitstreamVersion(void);
    uint   GetRemoteBuildNumber(void);
    bool   GetWasRecentlyInNetworkInterruption(uint uiMaxTicksAgo);
    void   SetIsInNetworkInterruption(bool bInNetworkInterruption);

    CVector m_vecPrevBulletSyncStart;
    CVector m_vecPrevBulletSyncEnd;
    uchar   m_ucPrevBulletSyncOrderCounter;

private:
    bool    m_bIsLocalPlayer;
    SString m_strNick;

    unsigned int m_uiPing;

    CVector      m_vecTargetPosition;
    CVector      m_vecTargetMoveSpeed;
    CVector      m_vecTargetTurnSpeed;
    CVector      m_vecTargetIncrements;
    unsigned int m_uiFramesSincePositionUpdate;

    unsigned short m_usLatency;
    CVector        m_vecAimSpeed;
    CVector        m_vecOldAim;
    CVector        m_vecCurrentAim;
    unsigned long  m_ulOldAimTime;
    unsigned long  m_ulCurrentAimTime;
    unsigned long  m_ulTick;
    bool           m_bDoExtrapolatingAim;

    bool    m_bForce;
    CVector m_vecForcedMoveSpeed;
    CVector m_vecForcedTurnSpeed;

    unsigned long m_ulLastPuresyncTime;
    CVector       m_vecLastPuresyncPosition;
    bool          m_bHasConnectionTrouble;

    CClientTeam* m_pTeam;

    bool          m_bNametagShowing;
    unsigned long m_ulLastNametagShow;
    unsigned char m_ucNametagColorR, m_ucNametagColorG, m_ucNametagColorB;
    bool          m_bNametagColorOverridden;
    std::string   m_strNametag;

    unsigned int m_uiPlayerSyncCount;
    unsigned int m_uiKeySyncCount;
    unsigned int m_uiVehicleSyncCount;

    float m_fNametagDistance;

    bool m_bNetworkDead;

    CClientPlayerVoice* m_voice;
    ePuresyncType       m_LastPuresyncType;
    CVector             m_vecLightsyncCalcedVelocity;
    ushort              m_usRemoteBitstreamVersion;
    uint                m_uiRemoteBuildNumber;
    bool                m_bInNetworkInterruption;
    CElapsedTime        m_TimeSinceNetworkInterruptionEnded;

#ifdef MTA_DEBUG
private:
    bool m_bShowingWepdata;

public:
    void SetShowingWepdata(bool bState) { m_bShowingWepdata = bState; }
    bool IsShowingWepdata() const { return m_bShowingWepdata; }
#endif
};

#endif
