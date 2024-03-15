/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayer.h
 *  PURPOSE:     Player ped entity class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPlayer;

#pragma once

#include "CPed.h"
#include "CCommon.h"
#include "CClient.h"
#include "CElement.h"
#include "CPlayerTextManager.h"
#include "CTeam.h"
#include "CPad.h"
#include "CObject.h"
#include "packets/CPacket.h"
#include "packets/CPlayerStatsPacket.h"
class CKeyBinds;
class CPlayerCamera;
enum class eVehicleAimDirection : std::uint8_t;

enum eVoiceState
{
    VOICESTATE_IDLE = 0,
    VOICESTATE_TRANSMITTING,
    VOICESTATE_TRANSMITTING_IGNORED,
};

#define MOVEMENT_UPDATE_THRESH (5)
#define DISTANCE_FOR_NEAR_VIEWER (310)

struct SViewerInfo
{
    SViewerInfo() : iMoveToFarCountDown(0), iZone(0), llLastUpdateTime(0), bInPureSyncSimSendList(false) {}

    int iMoveToFarCountDown;

    // Used in puresync
    int       iZone;
    std::int64_t llLastUpdateTime;

    bool bInPureSyncSimSendList;
};

typedef CFastHashMap<CPlayer*, SViewerInfo> SViewerMapType;

struct SScreenShotInfo
{
    bool      bInProgress;
    ushort    usNextPartNumber;
    ushort    usScreenShotId;
    std::int64_t llTimeStamp;
    uint      uiTotalBytes;
    ushort    usTotalParts;
    ushort    usResourceNetId;
    SString   strTag;
    CBuffer   buffer;
};

class CPlayer final : public CPed, public CClient
{
    friend class CElement;
    friend class CScriptDebugging;

public:
    ZERO_ON_NEW
    CPlayer(class CPlayerManager* pPlayerManager, class CScriptDebugging* pScriptDebugging, const NetServerPlayerID& PlayerSocket);
    ~CPlayer();

    void DoPulse();

    void Unlink();

    bool DoNotSendEntities() { return m_bDoNotSendEntities; };
    void SetDoNotSendEntities(bool bDont) { m_bDoNotSendEntities = bDont; };

    int                GetClientType() { return CClient::CLIENT_PLAYER; };
    std::uint64_t GetTimeSinceConnected() { return m_ConnectedTimer.Get(); }

    const char* GetNick() { return m_strNick; };
    void        SetNick(const char* szNick);

    int                GetGameVersion() { return m_iGameVersion; };
    void               SetGameVersion(int iGameVersion) { m_iGameVersion = iGameVersion; };
    std::uint16_t     GetMTAVersion() { return m_usMTAVersion; };
    void               SetMTAVersion(std::uint16_t usMTAVersion) { m_usMTAVersion = usMTAVersion; };
    std::uint16_t     GetBitStreamVersion() { return m_usBitStreamVersion; };
    void               SetBitStreamVersion(std::uint16_t usBitStreamVersion) { m_usBitStreamVersion = usBitStreamVersion; };
    bool               CanBitStream(eBitStreamVersion query) { return static_cast<eBitStreamVersion>(m_usBitStreamVersion) >= query; }
    void               SetPlayerVersion(const CMtaVersion& strPlayerVersion);
    const CMtaVersion& GetPlayerVersion() { return m_strPlayerVersion; };
    bool               ShouldIgnoreMinClientVersionChecks();

    bool IsMuted() { return m_bIsMuted; };
    void SetMuted(bool bSetMuted) { m_bIsMuted = bSetMuted; };
    bool IsJoined() { return m_bIsJoined; }
    void SetJoined() { m_bIsJoined = true; }

    bool SubscribeElementData(CElement* pElement, const std::string& strName);
    bool UnsubscribeElementData(CElement* pElement, const std::string& strName);
    bool UnsubscribeElementData(CElement* pElement);
    bool IsSubscribed(CElement* pElement, const std::string& strName) const;

    float GetCameraRotation() { return m_fCameraRotation; };
    void  SetCameraRotation(float fRotation) { m_fCameraRotation = fRotation; };

    long GetMoney() { return m_lMoney; };
    void SetMoney(long lMoney) { m_lMoney = lMoney; };

    const CVector&       GetSniperSourceVector() { return m_vecSniperSource; };
    void                 SetSniperSourceVector(const CVector& vecSource) { m_vecSniperSource = vecSource; };
    void                 GetTargettingVector(CVector& vecTarget) { vecTarget = m_vecTargetting; };
    void                 SetTargettingVector(const CVector& vecTarget) { m_vecTargetting = vecTarget; };
    float                GetAimDirection() { return m_fAimDirection; };
    void                 SetAimDirection(float fDirection) { m_fAimDirection = fDirection; };
    eVehicleAimDirection GetDriveByDirection() { return m_ucDriveByDirection; };
    void                 SetDriveByDirection(eVehicleAimDirection ucDirection) { m_ucDriveByDirection = ucDirection; };
    bool                 IsAkimboArmUp() { return m_bAkimboArmUp; };
    void                 SetAkimboArmUp(bool bUp) { m_bAkimboArmUp = bUp; };

    NetServerPlayerID& GetSocket() { return m_PlayerSocket; };
    const char*        GetSourceIP();
    std::uint16_t     GetSourcePort() { return m_PlayerSocket.GetPort(); };

    void         SetPing(uint uiPing) { m_uiPing = uiPing; }
    std::uint32_t GetPing() { return m_uiPing; }

    time_t GetNickChangeTime() { return m_tNickChange; };
    void   SetNickChangeTime(time_t tNickChange) { m_tNickChange = tNickChange; };

    uint Send(const CPacket& Packet);
    void SendEcho(const char* szEcho);
    void SendConsole(const char* szEcho);

    CPlayerTextManager* GetPlayerTextManager() { return m_pPlayerTextManager; };

    void AddSyncingVehicle(CVehicle* pVehicle);
    void RemoveSyncingVehicle(CVehicle* pVehicle);
    void RemoveAllSyncingVehicles();

    std::uint32_t                         CountSyncingVehicles() { return static_cast<std::uint32_t>(m_SyncingVehicles.size()); };
    std::list<CVehicle*>::const_iterator IterSyncingVehicleBegin() { return m_SyncingVehicles.begin(); };
    std::list<CVehicle*>::const_iterator IterSyncingVehicleEnd() { return m_SyncingVehicles.end(); };

    void AddSyncingPed(CPed* pPed);
    void RemoveSyncingPed(CPed* pPed);
    void RemoveAllSyncingPeds();

    std::uint32_t                     CountSyncingPeds() { return static_cast<std::uint32_t>(m_SyncingPeds.size()); };
    std::list<CPed*>::const_iterator IterSyncingPedBegin() { return m_SyncingPeds.begin(); };
    std::list<CPed*>::const_iterator IterSyncingPedEnd() { return m_SyncingPeds.end(); };

    void AddSyncingObject(CObject* pObject);
    void RemoveSyncingObject(CObject* pObject);
    void RemoveAllSyncingObjects();

    std::uint32_t                        CountSyncingObjects() { return static_cast<std::uint32_t>(m_SyncingObjects.size()); };
    std::list<CObject*>::const_iterator IterSyncingObjectBegin() { return m_SyncingObjects.begin(); };
    std::list<CObject*>::const_iterator IterSyncingObjectEnd() { return m_SyncingObjects.end(); };

    std::uint32_t GetScriptDebugLevel() { return m_uiScriptDebugLevel; };
    bool         SetScriptDebugLevel(std::uint32_t uiLevel);

    void          SetDamageInfo(ElementID ElementID, std::uint8_t ucWeapon, std::uint8_t ucBodyPart);
    void          ValidateDamageInfo();
    ElementID     GetPlayerAttacker();
    std::uint8_t GetAttackWeapon();
    std::uint8_t GetAttackBodyPart();

    CTeam* GetTeam() { return m_pTeam; }
    void   SetTeam(CTeam* pTeam, bool bChangeTeam = false);

    CPad* GetPad() { return m_pPad; }

    bool IsDebuggerVisible() { return m_bDebuggerVisible; }
    void SetDebuggerVisible(bool bVisible) { m_bDebuggerVisible = bVisible; }

    std::uint32_t GetWantedLevel() { return m_uiWantedLevel; }
    void         SetWantedLevel(std::uint32_t uiWantedLevel) { m_uiWantedLevel = uiWantedLevel; }

    bool GetForcedScoreboard() { return m_bForcedScoreboard; }
    void SetForcedScoreboard(bool bVisible) { m_bForcedScoreboard = bVisible; }

    bool GetForcedMap() { return m_bForcedMap; }
    void SetForcedMap(bool bVisible) { m_bForcedMap = bVisible; }

    void Reset();

    CPlayerCamera* GetCamera() { return m_pCamera; }

    CKeyBinds* GetKeyBinds() { return m_pKeyBinds; }

    bool IsCursorShowing() { return m_bCursorShowing; }
    void SetCursorShowing(bool bCursorShowing) { m_bCursorShowing = bCursorShowing; }

    char* GetNametagText() { return m_szNametagText; }
    void  SetNametagText(const char* szText);
    void  GetNametagColor(std::uint8_t& ucR, std::uint8_t& ucG, std::uint8_t& ucB);
    void  SetNametagOverrideColor(std::uint8_t ucR, std::uint8_t ucG, std::uint8_t ucB);
    void  RemoveNametagOverrideColor();
    bool  IsNametagColorOverridden() { return m_bNametagColorOverridden; }

    bool IsNametagShowing() { return m_bNametagShowing; }
    void SetNametagShowing(bool bShowing) { m_bNametagShowing = bShowing; }

    const std::string& GetSerial(uint uiIndex = 0) { return m_strSerials[uiIndex % NUMELMS(m_strSerials)]; }
    void               SetSerial(const std::string& strSerial, uint uiIndex) { m_strSerials[uiIndex % NUMELMS(m_strSerials)] = strSerial; }

    const std::string& GetSerialUser() { return m_strSerialUser; };
    void               SetSerialUser(const std::string& strUser) { m_strSerialUser = strUser; };

    const std::string& GetCommunityID() { return m_strCommunityID; };
    void               SetCommunityID(const std::string& strID) { m_strCommunityID = strID; };

    std::uint8_t GetBlurLevel() { return m_ucBlurLevel; }
    void          SetBlurLevel(std::uint8_t ucBlurLevel) { m_ucBlurLevel = ucBlurLevel; }

    bool IsTimeForPuresyncFar();

    // Sync stuff
    void         SetSyncingVelocity(bool bSyncing) { m_bSyncingVelocity = bSyncing; }
    bool         IsSyncingVelocity() const { return m_bSyncingVelocity; }
    void         IncrementPuresync() { m_uiPuresyncPackets++; }
    std::uint32_t GetPuresyncCount() const { return m_uiPuresyncPackets; }

    void               NotifyReceivedSync() { m_LastReceivedSyncTimer.Reset(); }
    std::uint64_t GetTimeSinceReceivedSync() { return m_LastReceivedSyncTimer.Get(); }
    bool               UhOhNetworkTrouble() { return GetTimeSinceReceivedSync() > 5000; }

    const std::string& GetAnnounceValue(const std::string& strKey) const;
    void               SetAnnounceValue(const std::string& strKey, const std::string& strValue);

    // Checks
    void SetWeaponCorrect(bool bWeaponCorrect);
    bool GetWeaponCorrect();

    void            MaybeUpdateOthersNearList();
    void            UpdateOthersNearList();
    void            RefreshNearPlayer(CPlayer* pOther);
    SViewerMapType& GetNearPlayerList() { return m_NearPlayerList; }
    SViewerMapType& GetFarPlayerList() { return m_FarPlayerList; }
    void            AddPlayerToDistLists(CPlayer* pOther);
    void            RemovePlayerFromDistLists(CPlayer* pOther);
    void            MovePlayerToNearList(CPlayer* pOther);
    void            MovePlayerToFarList(CPlayer* pOther);
    bool            ShouldPlayerBeInNearList(CPlayer* pOther);

    SScreenShotInfo& GetScreenShotInfo() { return m_ScreenShotInfo; }

    CPlayerStatsPacket* GetPlayerStatsPacket() { return m_pPlayerStatsPacket; }
    void                SetPlayerStat(std::uint16_t usID, float fValue);
    float               GetWeaponRangeFromSlot(uint uiSlot = 0xFF);

    void SetLeavingServer(bool bLeaving) noexcept { m_bIsLeavingServer = bLeaving; }
    bool IsLeavingServer() const noexcept { return m_bIsLeavingServer; }

    void SetRedirecting(bool bRedirecting) noexcept { m_bIsRedirecting = bRedirecting; }
    bool IsRedirecting() const noexcept { return m_bIsRedirecting; }

protected:
    bool ReadSpecialData(const int iLine) override { return true; }

public:
    //
    // Light Sync
    //
    struct SLightweightSyncData
    {
        SLightweightSyncData()
        {
            health.uiContext = 0;
            health.bSync = false;
            vehicleHealth.uiContext = 0;
            vehicleHealth.bSync = false;
            m_bSyncPosition = false;
        }

        struct
        {
            float        fLastHealth;
            float        fLastArmor;
            bool         bSync;
            std::uint32_t uiContext;
        } health;

        struct
        {
            CVehicle*    lastVehicle;
            float        fLastHealth;
            bool         bSync;
            std::uint32_t uiContext;
        } vehicleHealth;

        bool m_bSyncPosition;
    };
    SLightweightSyncData& GetLightweightSyncData() { return m_lightweightSyncData; }

    void      SetPosition(const CVector& vecPosition);
    std::int64_t GetPositionLastChanged() { return m_llLastPositionHasChanged; }
    void      MarkPositionAsChanged() { m_llLastPositionHasChanged = GetTickCount64_(); }

    //
    // End Light Sync
    //

    eVoiceState GetVoiceState() { return m_VoiceState; }
    void        SetVoiceState(eVoiceState State) { m_VoiceState = State; }

    std::list<CElement*>::const_iterator IterBroadcastListBegin() { return m_lstBroadcastList.begin(); };
    std::list<CElement*>::const_iterator IterBroadcastListEnd() { return m_lstBroadcastList.end(); };
    bool                                 IsVoiceMuted() { return m_lstBroadcastList.empty(); }
    void                                 SetVoiceBroadcastTo(CElement* pElement);
    void                                 SetVoiceBroadcastTo(const std::list<CElement*>& lstElements);

    void                                 SetVoiceIgnoredElement(CElement* pElement);
    void                                 SetVoiceIgnoredList(const std::list<CElement*>& lstElements);
    std::list<CElement*>::const_iterator IterIgnoredListBegin() { return m_lstIgnoredList.begin(); };
    std::list<CElement*>::const_iterator IterIgnoredListEnd() { return m_lstIgnoredList.end(); };
    bool                                 IsPlayerIgnoringElement(CElement* pElement);

    void           SetCameraOrientation(const CVector& vecPosition, const CVector& vecFwd);
    bool           IsTimeToReceivePuresyncNearFrom(CPlayer* pOther, SViewerInfo& nearInfo);
    int            GetPuresyncZone(CPlayer* pOther);
    int            GetApproxPuresyncPacketSize();
    const CVector& GetCamPosition() { return m_vecCamPosition; };
    const CVector& GetCamFwd() { return m_vecCamFwd; };
    const SString& GetQuitReasonForLog() { return m_strQuitReasonForLog; }
    void           SetQuitReasonForLog(const SString& strReason) { m_strQuitReasonForLog = strReason; }

    CFastHashSet<CPlayer*> m_PureSyncSimSendList;
    bool                   m_bPureSyncSimSendListDirty;
    class CSimPlayer*      m_pSimPlayer;
    SString                m_strDetectedAC;
    uint                   m_uiD3d9Size;
    SString                m_strD3d9Md5;
    SString                m_strD3d9Sha256;

private:
    SLightweightSyncData m_lightweightSyncData;

    void WriteCameraModePacket();
    void WriteCameraPositionPacket();

    class CPlayerManager*   m_pPlayerManager;
    class CScriptDebugging* m_pScriptDebugging;

    CPlayerTextManager* m_pPlayerTextManager;

    SString        m_strNick;
    bool           m_bDoNotSendEntities;
    int            m_iGameVersion;
    std::uint16_t m_usMTAVersion;
    std::uint16_t m_usBitStreamVersion;
    CMtaVersion    m_strPlayerVersion;
    bool           m_bIsMuted;
    bool           m_bIsLeavingServer;
    bool           m_bIsRedirecting;
    bool           m_bIsJoined;

    bool m_bNametagColorOverridden;

    float                m_fCameraRotation;
    long                 m_lMoney;
    CVector              m_vecSniperSource;
    CVector              m_vecTargetting;
    float                m_fAimDirection;
    eVehicleAimDirection m_ucDriveByDirection;

    bool m_bAkimboArmUp;

    CElapsedTime m_ConnectedTimer;

    NetServerPlayerID m_PlayerSocket;
    uint              m_uiPing;

    time_t m_tNickChange;

    std::list<CVehicle*> m_SyncingVehicles;
    std::list<CPed*>     m_SyncingPeds;
    std::list<CObject*>  m_SyncingObjects;

    std::uint32_t m_uiScriptDebugLevel;

    ElementID     m_PlayerAttackerID;
    std::uint8_t m_ucAttackWeapon;
    std::uint8_t m_ucAttackBodyPart;
    std::int64_t     m_llSetDamageInfoTime;

    CTeam* m_pTeam;
    CPad*  m_pPad;

    bool m_bDebuggerVisible;

    std::uint32_t m_uiWantedLevel;

    bool m_bForcedScoreboard;
    bool m_bForcedMap;

    CPlayerCamera* m_pCamera;

    CKeyBinds* m_pKeyBinds;

    bool m_bCursorShowing;

    char*         m_szNametagText;
    std::uint8_t m_ucNametagR;
    std::uint8_t m_ucNametagG;
    std::uint8_t m_ucNametagB;
    bool          m_bNametagShowing;

    std::string m_strSerials[2];
    std::string m_strSerialUser;
    std::string m_strCommunityID;

    std::uint8_t m_ucBlurLevel;

    std::int64_t m_llNextFarPuresyncTime;

    // Voice
    eVoiceState          m_VoiceState;
    std::list<CElement*> m_lstBroadcastList;
    std::list<CElement*> m_lstIgnoredList;

    // Sync stuff
    bool         m_bSyncingVelocity;
    std::uint32_t m_uiPuresyncPackets;

    CElapsedTime m_LastReceivedSyncTimer;

    std::map<std::string, std::string> m_AnnounceValues;

    std::set<std::pair<CElement*, std::string>> m_DataSubscriptions;

    uint m_uiWeaponIncorrectCount;

    SViewerMapType m_NearPlayerList;
    SViewerMapType m_FarPlayerList;
    CElapsedTime   m_UpdateNearListTimer;
    CVector        m_vecUpdateNearLastPosition;

    CVector m_vecCamPosition;
    CVector m_vecCamFwd;
    int     m_iLastPuresyncZoneDebug;

    std::int64_t m_llLastPositionHasChanged;
    SString   m_strIP;

    SScreenShotInfo m_ScreenShotInfo;

    CPlayerStatsPacket* m_pPlayerStatsPacket;

    // Used to reduce calls when calculating weapon range
    float       m_fWeaponRangeLast;
    float       m_fWeaponRangeLastSkill;
    eWeaponType m_eWeaponRangeLastWeapon;
    uint        m_uiWeaponRangeLastStatsRevision;

    ushort  m_usPrevDimension;
    SString m_strQuitReasonForLog;
};
