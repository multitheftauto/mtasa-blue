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
enum class eVehicleAimDirection : unsigned char;

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
    long long llLastUpdateTime;

    bool bInPureSyncSimSendList;
};

typedef CFastHashMap<CPlayer*, SViewerInfo> SViewerMapType;

struct SScreenShotInfo
{
    bool      bInProgress;
    ushort    usNextPartNumber;
    ushort    usScreenShotId;
    long long llTimeStamp;
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
    unsigned long long GetTimeSinceConnected() { return m_ConnectedTimer.Get(); }

    const char* GetNick() { return m_strNick; };
    void        SetNick(const char* szNick);

    int                GetGameVersion() { return m_iGameVersion; };
    void               SetGameVersion(int iGameVersion) { m_iGameVersion = iGameVersion; };
    unsigned short     GetMTAVersion() { return m_usMTAVersion; };
    void               SetMTAVersion(unsigned short usMTAVersion) { m_usMTAVersion = usMTAVersion; };
    unsigned short     GetBitStreamVersion() { return m_usBitStreamVersion; };
    void               SetBitStreamVersion(unsigned short usBitStreamVersion) { m_usBitStreamVersion = usBitStreamVersion; };
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
    unsigned short     GetSourcePort() { return m_PlayerSocket.GetPort(); };

    void         SetPing(uint uiPing) { m_uiPing = uiPing; }
    unsigned int GetPing() { return m_uiPing; }

    time_t GetNickChangeTime() { return m_tNickChange; };
    void   SetNickChangeTime(time_t tNickChange) { m_tNickChange = tNickChange; };

    uint Send(const CPacket& Packet);
    void SendEcho(const char* szEcho);
    void SendConsole(const char* szEcho);

    CPlayerTextManager* GetPlayerTextManager() { return m_pPlayerTextManager; };

    void AddSyncingVehicle(CVehicle* pVehicle);
    void RemoveSyncingVehicle(CVehicle* pVehicle);
    void RemoveAllSyncingVehicles();

    unsigned int                         CountSyncingVehicles() { return static_cast<unsigned int>(m_SyncingVehicles.size()); };
    std::list<CVehicle*>::const_iterator IterSyncingVehicleBegin() { return m_SyncingVehicles.begin(); };
    std::list<CVehicle*>::const_iterator IterSyncingVehicleEnd() { return m_SyncingVehicles.end(); };

    void AddSyncingPed(CPed* pPed);
    void RemoveSyncingPed(CPed* pPed);
    void RemoveAllSyncingPeds();

    unsigned int                     CountSyncingPeds() { return static_cast<unsigned int>(m_SyncingPeds.size()); };
    std::list<CPed*>::const_iterator IterSyncingPedBegin() { return m_SyncingPeds.begin(); };
    std::list<CPed*>::const_iterator IterSyncingPedEnd() { return m_SyncingPeds.end(); };

    void AddSyncingObject(CObject* pObject);
    void RemoveSyncingObject(CObject* pObject);
    void RemoveAllSyncingObjects();

    unsigned int                        CountSyncingObjects() { return static_cast<unsigned int>(m_SyncingObjects.size()); };
    std::list<CObject*>::const_iterator IterSyncingObjectBegin() { return m_SyncingObjects.begin(); };
    std::list<CObject*>::const_iterator IterSyncingObjectEnd() { return m_SyncingObjects.end(); };

    unsigned int GetScriptDebugLevel() { return m_uiScriptDebugLevel; };
    bool         SetScriptDebugLevel(std::uint8_t level);

    void          SetDamageInfo(ElementID ElementID, unsigned char ucWeapon, unsigned char ucBodyPart);
    void          ValidateDamageInfo();
    ElementID     GetPlayerAttacker();
    unsigned char GetAttackWeapon();
    unsigned char GetAttackBodyPart();

    CTeam* GetTeam() { return m_pTeam; }
    void   SetTeam(CTeam* pTeam, bool bChangeTeam = false);

    CPad* GetPad() { return m_pPad; }

    bool IsDebuggerVisible() { return m_bDebuggerVisible; }
    void SetDebuggerVisible(bool bVisible) { m_bDebuggerVisible = bVisible; }

    unsigned int GetWantedLevel() { return m_uiWantedLevel; }
    void         SetWantedLevel(unsigned int uiWantedLevel) { m_uiWantedLevel = uiWantedLevel; }

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
    void  GetNametagColor(unsigned char& ucR, unsigned char& ucG, unsigned char& ucB);
    void  SetNametagOverrideColor(unsigned char ucR, unsigned char ucG, unsigned char ucB);
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

    unsigned char GetBlurLevel() { return m_ucBlurLevel; }
    void          SetBlurLevel(unsigned char ucBlurLevel) { m_ucBlurLevel = ucBlurLevel; }

    bool IsTimeForPuresyncFar();

    // Sync stuff
    void         SetSyncingVelocity(bool bSyncing) { m_bSyncingVelocity = bSyncing; }
    bool         IsSyncingVelocity() const { return m_bSyncingVelocity; }
    void         IncrementPuresync() { m_uiPuresyncPackets++; }
    unsigned int GetPuresyncCount() const { return m_uiPuresyncPackets; }

    void               NotifyReceivedSync() { m_LastReceivedSyncTimer.Reset(); }
    unsigned long long GetTimeSinceReceivedSync() { return m_LastReceivedSyncTimer.Get(); }
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
    void                SetPlayerStat(unsigned short usID, float fValue);
    float               GetWeaponRangeFromSlot(uint uiSlot = 0xFF);

    void SetLeavingServer(bool bLeaving) noexcept { m_bIsLeavingServer = bLeaving; }
    bool IsLeavingServer() const noexcept { return m_bIsLeavingServer; }

    void SetRedirecting(bool bRedirecting) noexcept { m_bIsRedirecting = bRedirecting; }
    bool IsRedirecting() const noexcept { return m_bIsRedirecting; }

    bool GetTeleported() const noexcept { return m_teleported; }
    void SetTeleported(bool state) noexcept { m_teleported = state; }

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
            unsigned int uiContext;
        } health;

        struct
        {
            CVehicle*    lastVehicle;
            float        fLastHealth;
            bool         bSync;
            unsigned int uiContext;
        } vehicleHealth;

        bool m_bSyncPosition;
    };
    SLightweightSyncData& GetLightweightSyncData() { return m_lightweightSyncData; }

    void      SetPosition(const CVector& vecPosition);
    long long GetPositionLastChanged() { return m_llLastPositionHasChanged; }
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
    unsigned short m_usMTAVersion;
    unsigned short m_usBitStreamVersion;
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

    unsigned int m_uiScriptDebugLevel;

    ElementID     m_PlayerAttackerID;
    unsigned char m_ucAttackWeapon;
    unsigned char m_ucAttackBodyPart;
    long long     m_llSetDamageInfoTime;

    CTeam* m_pTeam;
    CPad*  m_pPad;

    bool m_bDebuggerVisible;

    unsigned int m_uiWantedLevel;

    bool m_bForcedScoreboard;
    bool m_bForcedMap;

    CPlayerCamera* m_pCamera;

    CKeyBinds* m_pKeyBinds;

    bool m_bCursorShowing;

    char*         m_szNametagText;
    unsigned char m_ucNametagR;
    unsigned char m_ucNametagG;
    unsigned char m_ucNametagB;
    bool          m_bNametagShowing;

    std::string m_strSerials[2];
    std::string m_strSerialUser;
    std::string m_strCommunityID;

    unsigned char m_ucBlurLevel;

    long long m_llNextFarPuresyncTime;

    // Voice
    eVoiceState          m_VoiceState;
    std::list<CElement*> m_lstBroadcastList;
    std::list<CElement*> m_lstIgnoredList;

    // Sync stuff
    bool         m_bSyncingVelocity;
    unsigned int m_uiPuresyncPackets;

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

    long long m_llLastPositionHasChanged;
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

    bool m_teleported = false;
};
