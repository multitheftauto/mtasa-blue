/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CGame.h
 *  PURPOSE:     Server game class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CGame;

#pragma once

#include "CCommon.h"
#include <net/CNetServer.h>
#include "CClient.h"
#include "CEvents.h"
#include "CCommandLineParser.h"
#include "CConnectHistory.h"
#include "CElementDeleter.h"

#include "packets/CCommandPacket.h"
#include "packets/CExplosionSyncPacket.h"
#include "packets/CProjectileSyncPacket.h"
#include "packets/CPedWastedPacket.h"
#include "packets/CPlayerJoinDataPacket.h"
#include "packets/CPlayerQuitPacket.h"
#include "packets/CPlayerTimeoutPacket.h"
#include "packets/CPlayerPuresyncPacket.h"
#include "packets/CVehiclePuresyncPacket.h"
#include "packets/CVehicleResyncPacket.h"
#include "packets/CKeysyncPacket.h"
#include "packets/CBulletsyncPacket.h"
#include "packets/CPedTaskPacket.h"
#include "packets/CCustomWeaponBulletSyncPacket.h"
#include "packets/CVehicleInOutPacket.h"
#include "packets/CVehicleDamageSyncPacket.h"
#include "packets/CVehicleTrailerPacket.h"
#include "packets/CVoiceDataPacket.h"
#include "packets/CLuaEventPacket.h"
#include "packets/CDestroySatchelsPacket.h"
#include "packets/CDetonateSatchelsPacket.h"
#include "packets/CCustomDataPacket.h"
#include "packets/CCameraSyncPacket.h"
#include "packets/CPlayerTransgressionPacket.h"
#include "packets/CPlayerDiagnosticPacket.h"
#include "packets/CPlayerModInfoPacket.h"
#include "packets/CPlayerACInfoPacket.h"
#include "packets/CPlayerScreenShotPacket.h"

#include "CRPCFunctions.h"

#include "lua/CLuaManager.h"

#include "CLightsyncManager.h"
#include "CBanManager.h"

// Forward declarations
class ASE;
class CAccessControlListManager;
class CAccountManager;
class CBlipManager;
class CClock;
class CColManager;
class CConsole;
class CConsoleClient;
class CElementDeleter;
class CGroups;
class CHandlingManager;
class CHTTPD;
class CMainConfig;
class CMapManager;
class CMarkerManager;
class CObjectManager;
class CPacket;
class CPacketTranslator;
class CLatentTransferManager;
class CDebugHookManager;
class CPedManager;
class CPickupManager;
class CPlayer;
class CPlayerManager;
class CRadarAreaManager;
class CRegisteredCommands;
class CDatabaseManager;
class CLuaCallbackManager;
class CRegistryManager;
class CRegistry;
class CRemoteCalls;
class CResourceManager;
class CScriptDebugging;
class CSettings;
class CTeamManager;
class CUnoccupiedVehicleSync;
class CPedSync;
class CObjectSync;
class CVehicleManager;
class CZoneNames;
class CLanBroadcast;
class CWaterManager;
class CTrainTrackManager;
class CWeaponStatManager;
class CBuildingRemovalManager;

class CCustomWeaponManager;
class COpenPortsTester;
class CMasterServerAnnouncer;
class CHqComms;
class CFunctionUseLogger;

// Packet forward declarations
class CCommandPacket;
class CCustomDataPacket;
class CDetonateSatchelsPacket;
class CExplosionSyncPacket;
class CKeysyncPacket;
class CLuaEventPacket;
class CPlayerJoinDataPacket;
class CPlayerPuresyncPacket;
class CPlayerQuitPacket;
class CPlayerTimeoutPacket;
class CPlayerWastedPacket;
class CProjectileSyncPacket;
class CVehicleDamageSyncPacket;
class CVehicleInOutPacket;
class CVehiclePuresyncPacket;
class CVehicleTrailerPacket;
class CVoiceDataPacket;
class CWeaponDamageCheckPacket;

using SGarageStates = SFixedArray<bool, MAX_GARAGES>;

// CSendList - Can be used like a std::list of players for sending packets.
//             Used to construct an optimized list of players for CGame::Broadcast
class CSendList : public std::multimap<ushort, CPlayer*>
{
public:
    void push_back(CPlayer* pPlayer) { MapInsert(*this, pPlayer->GetBitStreamVersion(), pPlayer); }
};

class CGame
{
public:
    ZERO_ON_NEW            // To be sure everything is cleared
    enum
    {
        VEHICLE_REQUEST_IN,
        VEHICLE_NOTIFY_IN,
        VEHICLE_NOTIFY_IN_ABORT,
        VEHICLE_REQUEST_OUT,
        VEHICLE_NOTIFY_OUT,
        VEHICLE_NOTIFY_OUT_ABORT,
        VEHICLE_NOTIFY_JACK,
        VEHICLE_NOTIFY_JACK_ABORT,
        VEHICLE_NOTIFY_FELL_OFF,
    };

    enum
    {
        VEHICLE_REQUEST_IN_CONFIRMED,
        VEHICLE_NOTIFY_IN_RETURN,
        VEHICLE_NOTIFY_IN_ABORT_RETURN,
        VEHICLE_REQUEST_OUT_CONFIRMED,
        VEHICLE_NOTIFY_OUT_RETURN,
        VEHICLE_NOTIFY_OUT_ABORT_RETURN,
        VEHICLE_NOTIFY_FELL_OFF_RETURN,
        VEHICLE_REQUEST_JACK_CONFIRMED,
        VEHICLE_NOTIFY_JACK_RETURN,
        VEHICLE_ATTEMPT_FAILED,
    };
    enum eGlitchType
    {
        GLITCH_QUICKRELOAD,
        GLITCH_FASTFIRE,
        GLITCH_FASTMOVE,
        GLITCH_CROUCHBUG,
        GLITCH_CLOSEDAMAGE,
        GLITCH_HITANIM,
        GLITCH_FASTSPRINT,
        GLITCH_BADDRIVEBYHITBOX,
        GLITCH_QUICKSTAND,
        GLITCH_KICKOUTOFVEHICLE_ONMODELREPLACE,
        NUM_GLITCHES
    };

public:
    CGame() noexcept;
    ~CGame() noexcept;

    void GetTag(char* szInfoTag, int iInfoTag) const noexcept;
    void HandleInput(const char* szCommand) noexcept;

    void DoPulse() noexcept;

    bool Start(int iArgumentCount, char* szArguments[]) noexcept;
    void Stop() noexcept;

    static bool StaticProcessPacket(std::uint8_t ucPacketID,
        const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream,
        SNetExtraInfo* pNetExtraInfo
    );
    bool        ProcessPacket(CPacket& Packet) noexcept;

    constexpr bool IsFinished() const noexcept { return m_bIsFinished; };
    constexpr void SetIsFinished(bool bFinished) noexcept { m_bIsFinished = bFinished; };

    
    constexpr CAccountManager*           GetAccountManager() const noexcept { return m_pAccountManager; }
    constexpr CAccessControlListManager* GetACLManager() const noexcept { return m_pACLManager; }
    constexpr ASE*                       GetASE() const noexcept { return m_pASE; }
    constexpr SharedUtil::CAsyncTaskScheduler* GetAsyncTaskScheduler() const noexcept { return m_pAsyncTaskScheduler; }
    constexpr CBanManager*               GetBanManager() const noexcept { return m_pBanManager; }
    constexpr CBlipManager*              GetBlipManager() const noexcept { return m_pBlipManager; }
    constexpr CBuildingRemovalManager*   GetBuildingRemovalManager() const noexcept { return m_pBuildingRemovalManager; }
    constexpr CClock*                    GetClock() const noexcept { return m_pClock; }
    constexpr CColManager*               GetColManager() const noexcept { return m_pColManager; }
    constexpr CMainConfig*               GetConfig() const noexcept { return m_pMainConfig; }
    constexpr CConsole*                  GetConsole() const noexcept { return m_pConsole; }
    constexpr CConsoleClient*            GetConsoleClient() const noexcept { return m_pConsoleClient; }
    constexpr CCustomWeaponManager*      GetCustomWeaponManager() const noexcept { return m_pCustomWeaponManager; }
    constexpr CDatabaseManager*          GetDatabaseManager() const noexcept { return m_pDatabaseManager; }
    constexpr CDebugHookManager*         GetDebugHookManager() const noexcept { return m_pDebugHookManager; }
    constexpr CFunctionUseLogger*        GetFunctionUseLogger() const noexcept { return m_pFunctionUseLogger; }
    constexpr CGroups*                   GetGroups() const noexcept { return m_pGroups; }
    constexpr CHTTPD*                    GetHTTPD() const noexcept { return m_pHTTPD; }
    constexpr CHandlingManager*          GetHandlingManager() const noexcept { return m_pHandlingManager; }
    constexpr CLanBroadcast*             GetLanBroadcast() const noexcept { return m_pLanBroadcast; }
    constexpr CLatentTransferManager*    GetLatentTransferManager() const noexcept { return m_pLatentTransferManager; }
    constexpr CLuaCallbackManager*       GetLuaCallbackManager() const noexcept { return m_pLuaCallbackManager; }
    constexpr CLuaManager*               GetLuaManager() const noexcept { return m_pLuaManager; }
    constexpr CMapManager*               GetMapManager() const noexcept { return m_pMapManager; }
    constexpr CMarkerManager*            GetMarkerManager() const noexcept { return m_pMarkerManager; }
    constexpr CMasterServerAnnouncer*    GetMasterServerAnnouncer() { return m_pMasterServerAnnouncer; }
    constexpr CObjectManager*            GetObjectManager() const noexcept { return m_pObjectManager; }
#ifdef WITH_OBJECT_SYNC
    constexpr CObjectSync* GetObjectSync() const noexcept { return m_pObjectSync; }
#endif
    constexpr CPacketTranslator*      GetPacketTranslator() const noexcept { return m_pPacketTranslator; }
    constexpr CPedManager*            GetPedManager() const noexcept { return m_pPedManager; }
    constexpr CPedSync*               GetPedSync() const noexcept { return m_pPedSync; }
    constexpr CPickupManager*         GetPickupManager() const noexcept { return m_pPickupManager; }
    constexpr CPlayerManager*         GetPlayerManager() const noexcept { return m_pPlayerManager; }
    constexpr CRPCFunctions*          GetRPCFunctions() const noexcept { return m_pRPCFunctions; }
    constexpr CRadarAreaManager*      GetRadarAreaManager() const noexcept { return m_pRadarAreaManager; }
    constexpr CRegisteredCommands*    GetRegisteredCommands() const noexcept { return m_pRegisteredCommands; }
    constexpr CRegistry*              GetRegistry() const noexcept { return m_pRegistry; }
    constexpr CRegistryManager*       GetRegistryManager() const noexcept { return m_pRegistryManager; }
    constexpr CRemoteCalls*           GetRemoteCalls() const noexcept { return m_pRemoteCalls; }
    constexpr CResourceManager*       GetResourceManager() const noexcept { return m_pResourceManager; }
    constexpr CScriptDebugging*       GetScriptDebugging() const noexcept { return m_pScriptDebugging; }
    constexpr CSettings*              GetSettings() const noexcept { return m_pSettings; }
    constexpr CTeamManager*           GetTeamManager() const noexcept { return m_pTeamManager; }
    constexpr CUnoccupiedVehicleSync* GetUnoccupiedVehicleSync() const noexcept { return m_pUnoccupiedVehicleSync; }
    constexpr CVehicleManager*        GetVehicleManager() const noexcept { return m_pVehicleManager; }
    constexpr CWaterManager*          GetWaterManager() const noexcept { return m_pWaterManager; }
    constexpr CWeaponStatManager*     GetWeaponStatManager() const noexcept { return m_pWeaponStatsManager; }
    constexpr CZoneNames*             GetZoneNames() const noexcept { return m_pZoneNames; }

    constexpr CEvents*           GetEvents() noexcept { return &m_Events; }
    constexpr CElementDeleter*   GetElementDeleter() noexcept { return &m_ElementDeleter; }
    constexpr CConnectHistory*   GetJoinFloodProtector() noexcept { return &m_FloodProtect; }
    constexpr CLightsyncManager* GetLightSyncManager() noexcept { return &m_lightsyncManager; }

    std::shared_ptr<CTrainTrackManager> GetTrainTrackManager() const noexcept { return m_pTrainTrackManager; }

    void JoinPlayer(CPlayer& Player);
    void InitialDataStream(CPlayer& Player);
    void QuitPlayer(CPlayer& Player,
        CClient::eQuitReasons Reason = CClient::QUIT_QUIT,
        bool bSayInConsole = true,
        const char* szKickReason = "None",
        const char* szResponsiblePlayer = "None");

    constexpr float GetGravity() const noexcept { return m_fGravity; }
    constexpr void  SetGravity(float fGravity) noexcept { m_fGravity = fGravity; }

    constexpr std::uint8_t GetTrafficLightState() const noexcept {
        return m_ucTrafficLightState;
    }
    constexpr void         SetTrafficLightState(const std::uint8_t ucState) noexcept {
        m_ucTrafficLightState = ucState;
    }

    constexpr bool GetTrafficLightsLocked() const noexcept { return m_bTrafficLightsLocked; }
    constexpr void SetTrafficLightsLocked(bool bLocked) noexcept {
        m_bTrafficLightsLocked = bLocked;
    }

    constexpr float GetJetpackMaxHeight() const noexcept { return m_fJetpackMaxHeight; }
    constexpr void  SetJetpackMaxHeight(float fMaxHeight) noexcept {
        m_fJetpackMaxHeight = fMaxHeight;
    }

    constexpr float GetGameSpeed() const noexcept { return m_fGameSpeed; }
    constexpr void  SetGameSpeed(float fGameSpeed) noexcept {
        m_fGameSpeed = fGameSpeed;
    }

    constexpr bool HasSkyGradient() const noexcept { return m_bHasSkyGradient; }
    constexpr void SetHasSkyGradient(bool bHasSkyGradient) noexcept {
        m_bHasSkyGradient = bHasSkyGradient;
    }

    constexpr void GetSkyGradient(
        std::uint8_t& ucTR, std::uint8_t& ucTG, std::uint8_t& ucTB,
        std::uint8_t& ucBR, std::uint8_t& ucBG, std::uint8_t& ucBB
    ) const noexcept {
        ucTR = m_ucSkyGradientTR;
        ucTG = m_ucSkyGradientTG;
        ucTB = m_ucSkyGradientTB;
        ucBR = m_ucSkyGradientBR;
        ucBG = m_ucSkyGradientBG;
        ucBB = m_ucSkyGradientBB;
    }
    constexpr void SetSkyGradient(
        std::uint8_t& ucTR, std::uint8_t& ucTG, std::uint8_t& ucTB,
        std::uint8_t& ucBR, std::uint8_t& ucBG, std::uint8_t& ucBB
    ) noexcept {
        m_ucSkyGradientTR = ucTR;
        m_ucSkyGradientTG = ucTG;
        m_ucSkyGradientTB = ucTB;
        m_ucSkyGradientBR = ucBR;
        m_ucSkyGradientBG = ucBG;
        m_ucSkyGradientBB = ucBB;
    }

    constexpr bool HasHeatHaze() const noexcept { return m_bHasHeatHaze; }
    constexpr void SetHasHeatHaze(bool bHasHeatHaze) noexcept {
        m_bHasHeatHaze = bHasHeatHaze;
    }

    constexpr void GetHeatHaze(SHeatHazeSettings& heatHazeSettings) const noexcept {
        // wouldn't "return m_HeatHazeSettings" be better here?
        heatHazeSettings = m_HeatHazeSettings;
    }
    constexpr void SetHeatHaze(const SHeatHazeSettings& heatHazeSettings) noexcept {
        m_HeatHazeSettings = heatHazeSettings;
    }

    constexpr bool GetInteriorSoundsEnabled() const noexcept { return m_bInteriorSoundsEnabled; }
    constexpr void SetInteriorSoundsEnabled(bool bEnable) noexcept {
        m_bInteriorSoundsEnabled = bEnable;
    }

    constexpr bool HasWaterColor() const noexcept { return m_bOverrideWaterColor; }
    constexpr void SetHasWaterColor(bool bOverrideWaterColor) noexcept {
        m_bOverrideWaterColor = bOverrideWaterColor;
    }

    constexpr void GetWaterColor(std::uint8_t& ucRed, std::uint8_t& ucGreen,
        std::uint8_t& ucBlue, std::uint8_t& ucAlpha
    ) noexcept {
        ucRed = m_ucWaterRed;
        ucGreen = m_ucWaterGreen;
        ucBlue = m_ucWaterBlue;
        ucAlpha = m_ucWaterAlpha;
    }
    constexpr void SetWaterColor(std::uint8_t& ucRed, std::uint8_t& ucGreen,
        std::uint8_t& ucBlue, std::uint8_t& ucAlpha
    ) noexcept {
        m_ucWaterRed = ucRed;
        m_ucWaterGreen = ucGreen;
        m_ucWaterBlue = ucBlue;
        m_ucWaterAlpha = ucAlpha;
    }

    constexpr bool HasRainLevel() const noexcept { return m_bOverrideRainLevel; }
    constexpr void SetHasRainLevel(bool bOverrideRainLevel) noexcept {
        m_bOverrideRainLevel = bOverrideRainLevel;
    }

    constexpr float GetRainLevel() const noexcept { return m_fRainLevel; }
    constexpr void  SetRainLevel(float& fRainLevel) noexcept {
        m_fRainLevel = fRainLevel;
    }

    constexpr bool HasSunSize() const noexcept { return m_bOverrideSunSize; }
    constexpr void SetHasSunSize(bool bOverrideSunSize) noexcept {
        m_bOverrideSunSize = bOverrideSunSize;
    }

    constexpr float GetSunSize() const noexcept { return m_fSunSize; }
    constexpr void  SetSunSize(float& fSunSize) noexcept {
        m_fSunSize = fSunSize;
    }

    constexpr bool HasSunColor() const noexcept { return m_bOverrideSunColor; }
    constexpr void SetHasSunColor(bool bOverrideSunColor) noexcept {
        m_bOverrideSunColor = bOverrideSunColor;
    }

    constexpr void GetSunColor(
        std::uint8_t& ucCoreR, std::uint8_t& ucCoreG, std::uint8_t& ucCoreB,
        std::uint8_t& ucCoronaR, std::uint8_t& ucCoronaG, std::uint8_t& ucCoronaB
    ) const noexcept {
        ucCoreR = m_ucSunCoreR;
        ucCoreG = m_ucSunCoreG;
        ucCoreB = m_ucSunCoreB;
        ucCoronaR = m_ucSunCoronaR;
        ucCoronaG = m_ucSunCoronaG;
        ucCoronaB = m_ucSunCoronaB;
    }
    constexpr void SetSunColor(
        std::uint8_t& ucCoreR, std::uint8_t& ucCoreG, std::uint8_t& ucCoreB,
        std::uint8_t& ucCoronaR, std::uint8_t& ucCoronaG, std::uint8_t& ucCoronaB
    ) noexcept {
        m_ucSunCoreR = ucCoreR;
        m_ucSunCoreG = ucCoreG;
        m_ucSunCoreB = ucCoreB;
        m_ucSunCoronaR = ucCoronaR;
        m_ucSunCoronaG = ucCoronaG;
        m_ucSunCoronaB = ucCoronaB;
    }

    constexpr bool HasWindVelocity() const noexcept { return m_bOverrideWindVelocity; }
    constexpr void SetHasWindVelocity(bool bOverrideWindVelocity) noexcept {
        m_bOverrideWindVelocity = bOverrideWindVelocity;
    }

    constexpr void GetWindVelocity(float& fVelX, float& fVelY, float& fVelZ) noexcept
    {
        fVelX = m_fWindVelX;
        fVelY = m_fWindVelY;
        fVelZ = m_fWindVelZ;
    }
    constexpr void SetWindVelocity(float& fVelX, float& fVelY, float& fVelZ) noexcept
    {
        m_fWindVelX = fVelX;
        m_fWindVelY = fVelY;
        m_fWindVelZ = fVelZ;
    }

    constexpr bool HasFarClipDistance() const noexcept { return m_bOverrideFarClip; }
    constexpr void SetHasFarClipDistance(bool bOverrideFarClip) noexcept {
        m_bOverrideFarClip = bOverrideFarClip;
    }

    constexpr float GetFarClipDistance() const noexcept { return m_fFarClipDistance; }
    constexpr void  SetFarClipDistance(float& fFarClipDistance) noexcept {
        m_fFarClipDistance = fFarClipDistance;
    }

    constexpr bool HasFogDistance() const noexcept { return m_bOverrideFogDistance; }
    constexpr void SetHasFogDistance(bool bOverrideFogDistance) noexcept {
        m_bOverrideFogDistance = bOverrideFogDistance;
    }

    constexpr float GetFogDistance() const noexcept { return m_fFogDistance; }
    constexpr void  SetFogDistance(float& fFogDistance) noexcept {
        m_fFogDistance = fFogDistance;
    }

    constexpr float GetAircraftMaxHeight() const noexcept { return m_fAircraftMaxHeight; }
    constexpr void  SetAircraftMaxHeight(float fMaxHeight) noexcept {
        m_fAircraftMaxHeight = fMaxHeight;
    }

    constexpr float GetAircraftMaxVelocity() const noexcept {
        return m_fAircraftMaxVelocity;
    }
    constexpr void  SetAircraftMaxVelocity(float fVelocity) noexcept
    {
        m_fAircraftMaxVelocity = fVelocity;
        m_fAircraftMaxVelocity_Sq = fVelocity * fVelocity;
    }

    constexpr bool GetOcclusionsEnabled() const noexcept { return m_bOcclusionsEnabled; }
    constexpr void SetOcclusionsEnabled(bool bOcclusionsEnabled) noexcept {
        m_bOcclusionsEnabled = bOcclusionsEnabled;
    }

    constexpr SGarageStates& GetGarageStates() noexcept { return m_bGarageStates; }

    void Lock() noexcept;
    void Unlock() noexcept;

    constexpr bool IsBeingDeleted() const noexcept { return m_bBeingDeleted; }
    void ResetMapInfo() noexcept;

    void        SetGlitchEnabled(const std::string& strGlitch, bool bEnabled) noexcept;
    bool        IsGlitchEnabled(const std::string& strGlitch) const noexcept;
    bool        IsGlitchEnabled(eGlitchType cGlitch) const noexcept;
    eGlitchType GetGlitchIndex(const std::string& strGlitch) noexcept { return m_GlitchNames[strGlitch]; }
    bool        IsGlitch(const std::string& strGlitch) const noexcept { return m_GlitchNames.count(strGlitch) > 0; }

    bool IsWorldSpecialPropertyEnabled(WorldSpecialProperty property) const noexcept {
        return m_WorldSpecialProps[property];
    }
    void SetWorldSpecialPropertyEnabled(WorldSpecialProperty property, bool isEnabled) noexcept {
        m_WorldSpecialProps[property] = isEnabled;
    }

    void SetCloudsEnabled(bool bEnabled) noexcept;
    bool GetCloudsEnabled() const noexcept;

    void SetJetpackWeaponEnabled(eWeaponType weaponType, bool bEnabled) noexcept;
    bool GetJetpackWeaponEnabled(eWeaponType weaponType) const noexcept;

    constexpr bool HasMoonSize() const noexcept { return m_bOverrideMoonSize; }
    constexpr void SetHasMoonSize(bool bOverrideMoonSize) noexcept {
        m_bOverrideMoonSize = bOverrideMoonSize;
    }

    constexpr int  GetMoonSize() const noexcept { return m_iMoonSize; }
    constexpr void SetMoonSize(int iMoonSize) noexcept { m_iMoonSize = iMoonSize; }

    void PrintLogOutputFromNetModule() noexcept;
    void StartOpenPortsTest() noexcept;

    constexpr bool IsServerFullyUp() const noexcept { return m_bServerFullyUp; }

    constexpr ushort GetServerFPS() const noexcept { return m_usFPS; }
    constexpr int    GetSyncFPS() const noexcept { return m_iSyncFPS; }
    constexpr void   SetSyncFPS(int iSyncFPS) noexcept { m_iSyncFPS = iSyncFPS; }

    void HandleBackup();
    void HandleCrashDumpEncryption();
    void EnableLatentSends(bool bEnabled, int iBandwidth = 0, CLuaMain* pLuaMain = NULL, ushort usResourceNetId = 0xFFFF);
    void SendPacketBatchBegin(std::uint8_t ucPacketId, NetBitStreamInterface* pBitStream);
    bool SendPacket(std::uint8_t ucPacketID, const NetServerPlayerID& playerID, NetBitStreamInterface* pBitStream, bool bBroadcast,
                    NetServerPacketPriority packetPriority, NetServerPacketReliability packetReliability,
                    ePacketOrdering packetOrdering = PACKET_ORDERING_DEFAULT);
    void SendPacketBatchEnd();

    bool IsBulletSyncActive();
    void SendSyncSettings(CPlayer* pPlayer = NULL);

    CMtaVersion CalculateMinClientRequirement();
    bool        IsBelowMinimumClient(const CMtaVersion& strVersion);
    bool        IsBelowRecommendedClient(const CMtaVersion& strVersion);
    void        ApplyAseSetting();
    void        ApplyPlayerTriggeredEventIntervalChange();
    bool        IsUsingMtaServerConf() { return m_bUsingMtaServerConf; }

    void SetDevelopmentMode(bool enabled) { m_DevelopmentModeEnabled = enabled; }
    bool GetDevelopmentMode() { return m_DevelopmentModeEnabled; }

    bool IsClientTransferBoxVisible() const { return m_showClientTransferBox; }
    void SetClientTransferBoxVisible(bool visible) { m_showClientTransferBox = visible; }

private:
    void AddBuiltInEvents();
    void RelayPlayerPuresync(class CPacket& Packet);
    void RelayNearbyPacket(class CPacket& Packet);

    void ProcessTrafficLights(long long llCurrentTime);

    void Packet_PlayerJoin(const NetServerPlayerID& Source);
    void Packet_PlayerJoinData(class CPlayerJoinDataPacket& Packet);
    void Packet_PedWasted(class CPedWastedPacket& Packet);
    void Packet_PlayerWasted(class CPlayerWastedPacket& Packet);
    void Packet_PlayerQuit(class CPlayerQuitPacket& Packet);
    void Packet_PlayerTimeout(class CPlayerTimeoutPacket& Packet);
    void Packet_PlayerPuresync(class CPlayerPuresyncPacket& Packet);
    void Packet_DetonateSatchels(class CDetonateSatchelsPacket& Packet);
    void Packet_DestroySatchels(class CDestroySatchelsPacket& Packet);
    void Packet_ExplosionSync(class CExplosionSyncPacket& Packet);
    void Packet_ProjectileSync(class CProjectileSyncPacket& Packet);
    void Packet_Command(class CCommandPacket& Packet);
    void Packet_VehicleDamageSync(class CVehicleDamageSyncPacket& Packet);
    void Packet_VehiclePuresync(class CVehiclePuresyncPacket& Packet);
    void Packet_Keysync(class CKeysyncPacket& Packet);
    void Packet_Bulletsync(class CBulletsyncPacket& Packet);
    void Packet_PedTask(class CPedTaskPacket& Packet);
    void Packet_WeaponBulletsync(class CCustomWeaponBulletSyncPacket& Packet);
    void Packet_Vehicle_InOut(class CVehicleInOutPacket& Packet);
    void Packet_VehicleTrailer(class CVehicleTrailerPacket& Packet);
    void Packet_LuaEvent(class CLuaEventPacket& Packet);
    void Packet_CustomData(class CCustomDataPacket& Packet);
    void Packet_Voice_Data(class CVoiceDataPacket& Packet);
    void Packet_Voice_End(class CVoiceEndPacket& Packet);
    void Packet_CameraSync(class CCameraSyncPacket& Packet);
    void Packet_PlayerTransgression(class CPlayerTransgressionPacket& Packet);
    void Packet_PlayerDiagnostic(class CPlayerDiagnosticPacket& Packet);
    void Packet_PlayerModInfo(class CPlayerModInfoPacket& Packet);
    void Packet_PlayerACInfo(class CPlayerACInfoPacket& Packet);
    void Packet_PlayerScreenShot(class CPlayerScreenShotPacket& Packet);
    void Packet_PlayerNoSocket(class CPlayerNoSocketPacket& Packet);
    void Packet_PlayerNetworkStatus(class CPlayerNetworkStatusPacket& Packet);
    void Packet_PlayerResourceStart(class CPlayerResourceStartPacket& Packet);

    static void PlayerCompleteConnect(CPlayer* pPlayer);

    void ProcessClientTriggeredEventSpam();
    void RegisterClientTriggeredEventUsage(CPlayer* pPlayer);

    // Technically, this could be put somewhere else.  It's a callback function
    // which the voice server library will call to send out data.

    CEvents                 m_Events;
    CElementDeleter         m_ElementDeleter;
    CConnectHistory         m_FloodProtect;
    CCommandLineParser      m_CommandLineParser;

    ASE*                       m_pASE{nullptr};
    CAccessControlListManager* m_pACLManager{nullptr};
    CAccountManager*           m_pAccountManager{nullptr};
    CBanManager*               m_pBanManager{nullptr};
    CBlipManager*              m_pBlipManager{nullptr};
    CBuildingRemovalManager*   m_pBuildingRemovalManager{nullptr};
    CClock*                    m_pClock{nullptr};
    CColManager*               m_pColManager{nullptr};
    CConsole*                  m_pConsole{nullptr};
    CConsoleClient*            m_pConsoleClient{nullptr};
    CCustomWeaponManager*      m_pCustomWeaponManager{nullptr};
    CDatabaseManager*          m_pDatabaseManager{nullptr};
    CDebugHookManager*         m_pDebugHookManager{nullptr};
    CFunctionUseLogger*        m_pFunctionUseLogger{nullptr};
    CGroups*                   m_pGroups{nullptr};
    CHTTPD*                    m_pHTTPD{nullptr};
    CHandlingManager*          m_pHandlingManager{nullptr};
    CLanBroadcast*             m_pLanBroadcast{nullptr};
    CLatentTransferManager*    m_pLatentTransferManager{nullptr};
    CLuaCallbackManager*       m_pLuaCallbackManager{nullptr};
    CLuaManager*               m_pLuaManager{nullptr};
    CMainConfig*               m_pMainConfig{nullptr};
    CMapManager*               m_pMapManager{nullptr};
    CMarkerManager*            m_pMarkerManager{nullptr};
    CObjectManager*            m_pObjectManager{nullptr};
#ifdef WITH_OBJECT_SYNC
    CObjectSync* m_pObjectSync{nullptr};
#endif
    CPacketTranslator*         m_pPacketTranslator{nullptr};
    CPedManager*               m_pPedManager{nullptr};
    CPedSync*                  m_pPedSync{nullptr};
    CPickupManager*            m_pPickupManager{nullptr};
    CPlayerManager*            m_pPlayerManager{nullptr};
    CRPCFunctions*             m_pRPCFunctions{nullptr};
    CRadarAreaManager*         m_pRadarAreaManager{nullptr};
    CRegisteredCommands*       m_pRegisteredCommands{nullptr};
    CRegistry*                 m_pRegistry{nullptr};
    CRegistryManager*          m_pRegistryManager{nullptr};
    CRemoteCalls*              m_pRemoteCalls{nullptr};
    CResourceManager*          m_pResourceManager{nullptr};
    CScriptDebugging*          m_pScriptDebugging{nullptr};
    CSettings*                 m_pSettings{nullptr};
    CTeamManager*              m_pTeamManager{nullptr};
    CUnoccupiedVehicleSync*    m_pUnoccupiedVehicleSync{nullptr};
    CVehicleManager*           m_pVehicleManager{nullptr};
    CWaterManager*             m_pWaterManager{nullptr};
    CWeaponStatManager*        m_pWeaponStatsManager{nullptr};
    CZoneNames*                m_pZoneNames{nullptr};

    std::shared_ptr<CTrainTrackManager> m_pTrainTrackManager;

    char* m_szCurrentFileName;

    float m_fGravity;
    float m_fGameSpeed;
    float m_fJetpackMaxHeight;
    float m_fAircraftMaxHeight;
    float m_fAircraftMaxVelocity;
    float m_fAircraftMaxVelocity_Sq;
    bool  m_bOcclusionsEnabled;
    bool  m_bUsingMtaServerConf;

    std::uint8_t m_ucTrafficLightState;
    bool          m_bTrafficLightsLocked;
    long long     m_llLastTrafficUpdate;

    std::uint8_t m_ucSkyGradientTR, m_ucSkyGradientTG, m_ucSkyGradientTB;
    std::uint8_t m_ucSkyGradientBR, m_ucSkyGradientBG, m_ucSkyGradientBB;
    bool          m_bHasSkyGradient;

    SHeatHazeSettings m_HeatHazeSettings;
    bool              m_bHasHeatHaze;

    bool          m_bOverrideWaterColor;
    std::uint8_t m_ucWaterRed, m_ucWaterGreen, m_ucWaterBlue, m_ucWaterAlpha;

    bool m_bInteriorSoundsEnabled;

    bool  m_bOverrideRainLevel;
    float m_fRainLevel;

    bool  m_bOverrideSunSize;
    float m_fSunSize;

    bool m_bOverrideMoonSize;
    int  m_iMoonSize;

    bool          m_bOverrideSunColor;
    std::uint8_t m_ucSunCoreR, m_ucSunCoreG, m_ucSunCoreB, m_ucSunCoronaR, m_ucSunCoronaG, m_ucSunCoronaB;

    bool  m_bOverrideWindVelocity;
    float m_fWindVelX, m_fWindVelY, m_fWindVelZ;

    bool  m_bOverrideFarClip;
    float m_fFarClipDistance;

    bool  m_bOverrideFogDistance;
    float m_fFogDistance;

    SGarageStates m_bGarageStates;

    // FPS statistics
    long long                                     m_llLastFPSTime;
    unsigned short                                m_usFrames;
    unsigned short                                m_usFPS;
    int                                           m_iSyncFPS;
    std::map<std::string, eGlitchType>            m_GlitchNames;
    SFixedArray<bool, NUM_GLITCHES>               m_Glitches;
    SFixedArray<bool, WEAPONTYPE_LAST_WEAPONTYPE> m_JetpackWeapons;
    std::map<WorldSpecialProperty, bool>          m_WorldSpecialProps;

    // This is ticked to true when the app should end
    bool m_bIsFinished;
    bool m_bBeingDeleted;

    // Clouds Enabled
    bool m_bCloudsEnabled;

    COpenPortsTester*       m_pOpenPortsTester;
    CMasterServerAnnouncer* m_pMasterServerAnnouncer;
    CHqComms*               m_pHqComms;

    CLightsyncManager m_lightsyncManager;

    bool m_bServerFullyUp;            // No http operations should be allowed unless this is true

    bool      m_bLatentSendsEnabled;
    int       m_iLatentSendsBandwidth;
    CLuaMain* m_pLatentSendsLuaMain;
    ushort    m_usLatentSendsResourceNetId;

    CMtaVersion m_strPrevMinClientKickRequirement;
    CMtaVersion m_strPrevMinClientConnectRequirement;
    CMtaVersion m_strPrevLowestConnectedPlayerVersion;

    SharedUtil::CAsyncTaskScheduler* m_pAsyncTaskScheduler;

    bool m_DevelopmentModeEnabled;
    bool m_showClientTransferBox = true;

    int m_iMaxClientTriggeredEventsPerInterval = 100;
    int m_iClientTriggeredEventsIntervalMs = 1000;

    struct ClientTriggeredEventsInfo
    {
        long long m_llTicks = 0;
        uint32_t  m_uiCounter = 0;
    };

    std::map<CPlayer*, ClientTriggeredEventsInfo> m_mapClientTriggeredEvents;
};
