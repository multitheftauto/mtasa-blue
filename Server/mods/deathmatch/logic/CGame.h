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
#include "packets/CLightsyncPacket.h"
#include "packets/CVehicleResyncPacket.h"
#include "packets/CKeysyncPacket.h"
#include "packets/CBulletsyncPacket.h"
#include "packets/CPedTaskPacket.h"
#include "packets/CCustomWeaponBulletSyncPacket.h"
#include "packets/CSyncSettingsPacket.h"
#include "packets/CVehicleInOutPacket.h"
#include "packets/CVehicleDamageSyncPacket.h"
#include "packets/CVehicleTrailerPacket.h"
#include "packets/CVoiceDataPacket.h"
#include "packets/CLuaEventPacket.h"
#include "packets/CDestroySatchelsPacket.h"
#include "packets/CDetonateSatchelsPacket.h"
#include "packets/CCustomDataPacket.h"
#include "packets/CHealthPacket.h"
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
class CHealthPacket;
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

typedef SFixedArray<bool, MAX_GARAGES> SGarageStates;

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
        enum {
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
    CGame();
    ~CGame();

    void GetTag(char* szInfoTag, int iInfoTag);
    void HandleInput(char* szCommand);

    void DoPulse();

    bool Start(int iArgumentCount, char* szArguments[]);
    void Stop();

    static bool StaticProcessPacket(unsigned char ucPacketID, const NetServerPlayerID& Socket, NetBitStreamInterface* BitStream, SNetExtraInfo* pNetExtraInfo);
    bool        ProcessPacket(CPacket& Packet);

    void SetIsFinished(bool bFinished) { m_bIsFinished = bFinished; };
    bool IsFinished() { return m_bIsFinished; };

    CMainConfig*            GetConfig() { return m_pMainConfig; }
    CHandlingManager*       GetHandlingManager() { return m_pHandlingManager; }
    CMapManager*            GetMapManager() { return m_pMapManager; }
    CPlayerManager*         GetPlayerManager() { return m_pPlayerManager; }
    CObjectManager*         GetObjectManager() { return m_pObjectManager; }
    CVehicleManager*        GetVehicleManager() { return m_pVehicleManager; }
    CTeamManager*           GetTeamManager() { return m_pTeamManager; }
    CUnoccupiedVehicleSync* GetUnoccupiedVehicleSync() { return m_pUnoccupiedVehicleSync; }
    CPedSync*               GetPedSync() { return m_pPedSync; }
    CRegisteredCommands*    GetRegisteredCommands() { return m_pRegisteredCommands; }
#ifdef WITH_OBJECT_SYNC
    CObjectSync* GetObjectSync() { return m_pObjectSync; }
#endif
    CConsole*                        GetConsole() { return m_pConsole; }
    CDatabaseManager*                GetDatabaseManager() { return m_pDatabaseManager; }
    CLuaCallbackManager*             GetLuaCallbackManager() { return m_pLuaCallbackManager; }
    CRegistryManager*                GetRegistryManager() { return m_pRegistryManager; }
    CRegistry*                       GetRegistry() { return m_pRegistry; }
    CAccountManager*                 GetAccountManager() { return m_pAccountManager; }
    CScriptDebugging*                GetScriptDebugging() { return m_pScriptDebugging; }
    CEvents*                         GetEvents() { return &m_Events; }
    CColManager*                     GetColManager() { return m_pColManager; }
    CLatentTransferManager*          GetLatentTransferManager() { return m_pLatentTransferManager; }
    CDebugHookManager*               GetDebugHookManager() { return m_pDebugHookManager; }
    CPedManager*                     GetPedManager() { return m_pPedManager; }
    CResourceManager*                GetResourceManager() { return m_pResourceManager; }
    CMarkerManager*                  GetMarkerManager() { return m_pMarkerManager; }
    CBlipManager*                    GetBlipManager() { return m_pBlipManager; }
    CPickupManager*                  GetPickupManager() { return m_pPickupManager; }
    CRadarAreaManager*               GetRadarAreaManager() { return m_pRadarAreaManager; }
    CGroups*                         GetGroups() { return m_pGroups; }
    CElementDeleter*                 GetElementDeleter() { return &m_ElementDeleter; }
    CConnectHistory*                 GetJoinFloodProtector() { return &m_FloodProtect; }
    CHTTPD*                          GetHTTPD() { return m_pHTTPD; }
    CSettings*                       GetSettings() { return m_pSettings; }
    CAccessControlListManager*       GetACLManager() { return m_pACLManager; }
    CBanManager*                     GetBanManager() { return m_pBanManager; }
    CRemoteCalls*                    GetRemoteCalls() { return m_pRemoteCalls; }
    CZoneNames*                      GetZoneNames() { return m_pZoneNames; }
    CClock*                          GetClock() { return m_pClock; }
    CWaterManager*                   GetWaterManager() { return m_pWaterManager; }
    CLightsyncManager*               GetLightSyncManager() { return &m_lightsyncManager; }
    CWeaponStatManager*              GetWeaponStatManager() { return m_pWeaponStatsManager; }
    CBuildingRemovalManager*         GetBuildingRemovalManager() { return m_pBuildingRemovalManager; }
    CCustomWeaponManager*            GetCustomWeaponManager() { return m_pCustomWeaponManager; }
    CFunctionUseLogger*              GetFunctionUseLogger() { return m_pFunctionUseLogger; }
    CMasterServerAnnouncer*          GetMasterServerAnnouncer() { return m_pMasterServerAnnouncer; }
    SharedUtil::CAsyncTaskScheduler* GetAsyncTaskScheduler() { return m_pAsyncTaskScheduler; }

    std::shared_ptr<CTrainTrackManager> GetTrainTrackManager() { return m_pTrainTrackManager; }

    void JoinPlayer(CPlayer& Player);
    void InitialDataStream(CPlayer& Player);
    void QuitPlayer(CPlayer& Player, CClient::eQuitReasons Reason = CClient::QUIT_QUIT, bool bSayInConsole = true, const char* szKickReason = "None",
                    const char* szResponsiblePlayer = "None");

    class CLuaManager* GetLuaManager() { return m_pLuaManager; };

    float GetGravity() { return m_fGravity; }
    void  SetGravity(float fGravity) { m_fGravity = fGravity; }

    unsigned char GetTrafficLightState() { return m_ucTrafficLightState; }
    void          SetTrafficLightState(unsigned char ucState) { m_ucTrafficLightState = ucState; }

    bool GetTrafficLightsLocked() { return m_bTrafficLightsLocked; }
    void SetTrafficLightsLocked(bool bLocked) { m_bTrafficLightsLocked = bLocked; }

    float GetJetpackMaxHeight() { return m_fJetpackMaxHeight; }
    void  SetJetpackMaxHeight(float fMaxHeight) { m_fJetpackMaxHeight = fMaxHeight; }

    float GetGameSpeed() { return m_fGameSpeed; }
    void  SetGameSpeed(float fGameSpeed) { m_fGameSpeed = fGameSpeed; }

    bool HasSkyGradient() { return m_bHasSkyGradient; }
    void SetHasSkyGradient(bool bHasSkyGradient) { m_bHasSkyGradient = bHasSkyGradient; }

    void GetSkyGradient(unsigned char& ucTR, unsigned char& ucTG, unsigned char& ucTB, unsigned char& ucBR, unsigned char& ucBG, unsigned char& ucBB)
    {
        ucTR = m_ucSkyGradientTR;
        ucTG = m_ucSkyGradientTG;
        ucTB = m_ucSkyGradientTB;
        ucBR = m_ucSkyGradientBR;
        ucBG = m_ucSkyGradientBG;
        ucBB = m_ucSkyGradientBB;
    }
    void SetSkyGradient(unsigned char& ucTR, unsigned char& ucTG, unsigned char& ucTB, unsigned char& ucBR, unsigned char& ucBG, unsigned char& ucBB)
    {
        m_ucSkyGradientTR = ucTR;
        m_ucSkyGradientTG = ucTG;
        m_ucSkyGradientTB = ucTB;
        m_ucSkyGradientBR = ucBR;
        m_ucSkyGradientBG = ucBG;
        m_ucSkyGradientBB = ucBB;
    }

    bool HasHeatHaze() { return m_bHasHeatHaze; }
    void SetHasHeatHaze(bool bHasHeatHaze) { m_bHasHeatHaze = bHasHeatHaze; }

    void GetHeatHaze(SHeatHazeSettings& heatHazeSettings) { heatHazeSettings = m_HeatHazeSettings; }
    void SetHeatHaze(const SHeatHazeSettings& heatHazeSettings) { m_HeatHazeSettings = heatHazeSettings; }

    bool GetInteriorSoundsEnabled() { return m_bInteriorSoundsEnabled; }
    void SetInteriorSoundsEnabled(bool bEnable) { m_bInteriorSoundsEnabled = bEnable; }

    bool HasWaterColor() { return m_bOverrideWaterColor; }
    void SetHasWaterColor(bool bOverrideWaterColor) { m_bOverrideWaterColor = bOverrideWaterColor; }

    void GetWaterColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha)
    {
        ucRed = m_ucWaterRed;
        ucGreen = m_ucWaterGreen;
        ucBlue = m_ucWaterBlue;
        ucAlpha = m_ucWaterAlpha;
    }
    void SetWaterColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha)
    {
        m_ucWaterRed = ucRed;
        m_ucWaterGreen = ucGreen;
        m_ucWaterBlue = ucBlue;
        m_ucWaterAlpha = ucAlpha;
    }

    bool HasRainLevel() { return m_bOverrideRainLevel; }
    void SetHasRainLevel(bool bOverrideRainLevel) { m_bOverrideRainLevel = bOverrideRainLevel; }

    float GetRainLevel() { return m_fRainLevel; }
    void  SetRainLevel(float& fRainLevel) { m_fRainLevel = fRainLevel; }

    bool HasSunSize() { return m_bOverrideSunSize; }
    void SetHasSunSize(bool bOverrideSunSize) { m_bOverrideSunSize = bOverrideSunSize; }

    float GetSunSize() { return m_fSunSize; }
    void  SetSunSize(float& fSunSize) { m_fSunSize = fSunSize; }

    bool HasSunColor() { return m_bOverrideSunColor; }
    void SetHasSunColor(bool bOverrideSunColor) { m_bOverrideSunColor = bOverrideSunColor; }

    void GetSunColor(unsigned char& ucCoreR, unsigned char& ucCoreG, unsigned char& ucCoreB, unsigned char& ucCoronaR, unsigned char& ucCoronaG,
                     unsigned char& ucCoronaB)
    {
        ucCoreR = m_ucSunCoreR;
        ucCoreG = m_ucSunCoreG;
        ucCoreB = m_ucSunCoreB;
        ucCoronaR = m_ucSunCoronaR;
        ucCoronaG = m_ucSunCoronaG;
        ucCoronaB = m_ucSunCoronaB;
    }
    void SetSunColor(unsigned char& ucCoreR, unsigned char& ucCoreG, unsigned char& ucCoreB, unsigned char& ucCoronaR, unsigned char& ucCoronaG,
                     unsigned char& ucCoronaB)
    {
        m_ucSunCoreR = ucCoreR;
        m_ucSunCoreG = ucCoreG;
        m_ucSunCoreB = ucCoreB;
        m_ucSunCoronaR = ucCoronaR;
        m_ucSunCoronaG = ucCoronaG;
        m_ucSunCoronaB = ucCoronaB;
    }

    bool HasWindVelocity() { return m_bOverrideWindVelocity; }
    void SetHasWindVelocity(bool bOverrideWindVelocity) { m_bOverrideWindVelocity = bOverrideWindVelocity; }

    void GetWindVelocity(float& fVelX, float& fVelY, float& fVelZ)
    {
        fVelX = m_fWindVelX;
        fVelY = m_fWindVelY;
        fVelZ = m_fWindVelZ;
    }
    void SetWindVelocity(float& fVelX, float& fVelY, float& fVelZ)
    {
        m_fWindVelX = fVelX;
        m_fWindVelY = fVelY;
        m_fWindVelZ = fVelZ;
    }

    bool HasFarClipDistance() { return m_bOverrideFarClip; }
    void SetHasFarClipDistance(bool bOverrideFarClip) { m_bOverrideFarClip = bOverrideFarClip; }

    float GetFarClipDistance() { return m_fFarClipDistance; }
    void  SetFarClipDistance(float& fFarClipDistance) { m_fFarClipDistance = fFarClipDistance; }

    bool HasFogDistance() { return m_bOverrideFogDistance; }
    void SetHasFogDistance(bool bOverrideFogDistance) { m_bOverrideFogDistance = bOverrideFogDistance; }

    float GetFogDistance() { return m_fFogDistance; }
    void  SetFogDistance(float& fFogDistance) { m_fFogDistance = fFogDistance; }

    float GetAircraftMaxHeight() { return m_fAircraftMaxHeight; }
    void  SetAircraftMaxHeight(float fMaxHeight) { m_fAircraftMaxHeight = fMaxHeight; }

    float GetAircraftMaxVelocity() { return m_fAircraftMaxVelocity; }
    void  SetAircraftMaxVelocity(float fVelocity)
    {
        m_fAircraftMaxVelocity = fVelocity;
        m_fAircraftMaxVelocity_Sq = fVelocity * fVelocity;
    }

    bool GetOcclusionsEnabled() { return m_bOcclusionsEnabled; }
    void SetOcclusionsEnabled(bool bOcclusionsEnabled) { m_bOcclusionsEnabled = bOcclusionsEnabled; }

    SGarageStates& GetGarageStates() { return m_bGarageStates; }

    void Lock();
    void Unlock();

    bool IsBeingDeleted() { return m_bBeingDeleted; }
    void ResetMapInfo();

    void        SetGlitchEnabled(const std::string& strGlitch, bool bEnabled);
    bool        IsGlitchEnabled(const std::string& strGlitch);
    bool        IsGlitchEnabled(eGlitchType cGlitch);
    eGlitchType GetGlitchIndex(const std::string& strGlitch) { return m_GlitchNames[strGlitch]; }
    bool        IsGlitch(const std::string& strGlitch) { return m_GlitchNames.count(strGlitch) > 0; }

    bool IsWorldSpecialPropertyEnabled(WorldSpecialProperty property) { return m_WorldSpecialProps[property]; }
    void SetWorldSpecialPropertyEnabled(WorldSpecialProperty property, bool isEnabled) { m_WorldSpecialProps[property] = isEnabled; }

    void SetCloudsEnabled(bool bEnabled);
    bool GetCloudsEnabled();

    void SetJetpackWeaponEnabled(eWeaponType weaponType, bool bEnabled);
    bool GetJetpackWeaponEnabled(eWeaponType weaponType);

    bool HasMoonSize() { return m_bOverrideMoonSize; }
    void SetHasMoonSize(bool bOverrideMoonSize) { m_bOverrideMoonSize = bOverrideMoonSize; }

    int  GetMoonSize() { return m_iMoonSize; }
    void SetMoonSize(int iMoonSize) { m_iMoonSize = iMoonSize; }

    void PrintLogOutputFromNetModule();
    void StartOpenPortsTest();

    bool IsServerFullyUp() { return m_bServerFullyUp; }

    ushort GetServerFPS() { return m_usFPS; }
    int    GetSyncFPS() { return m_iSyncFPS; }
    void   SetSyncFPS(int iSyncFPS) { m_iSyncFPS = iSyncFPS; }

    void HandleBackup();
    void HandleCrashDumpEncryption();
    void EnableLatentSends(bool bEnabled, int iBandwidth = 0, CLuaMain* pLuaMain = NULL, ushort usResourceNetId = 0xFFFF);
    void SendPacketBatchBegin(unsigned char ucPacketId, NetBitStreamInterface* pBitStream);
    bool SendPacket(unsigned char ucPacketID, const NetServerPlayerID& playerID, NetBitStreamInterface* pBitStream, bool bBroadcast,
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
    void Packet_changeHealth(class CHealthPacket& Packet);
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
    CRemoteCalls*           m_pRemoteCalls;
    CHTTPD*                 m_pHTTPD;
    CMainConfig*            m_pMainConfig;
    CBlipManager*           m_pBlipManager;
    CGroups*                m_pGroups;
    CColManager*            m_pColManager;
    CObjectManager*         m_pObjectManager;
    CPickupManager*         m_pPickupManager;
    CPlayerManager*         m_pPlayerManager;
    CRadarAreaManager*      m_pRadarAreaManager;
    CVehicleManager*        m_pVehicleManager;
    CPacketTranslator*      m_pPacketTranslator;
    CMapManager*            m_pMapManager;
    CElementDeleter         m_ElementDeleter;
    CConnectHistory         m_FloodProtect;
    CLuaManager*            m_pLuaManager;
    CScriptDebugging*       m_pScriptDebugging;
    CConsole*               m_pConsole;
    CUnoccupiedVehicleSync* m_pUnoccupiedVehicleSync;
    CPedSync*               m_pPedSync;
#ifdef WITH_OBJECT_SYNC
    CObjectSync* m_pObjectSync;
#endif
    CMarkerManager*            m_pMarkerManager;
    CClock*                    m_pClock;
    CBanManager*               m_pBanManager;
    CTeamManager*              m_pTeamManager;
    CCommandLineParser         m_CommandLineParser;
    CRegisteredCommands*       m_pRegisteredCommands;
    CDatabaseManager*          m_pDatabaseManager;
    CLuaCallbackManager*       m_pLuaCallbackManager;
    CRegistryManager*          m_pRegistryManager;
    CRegistry*                 m_pRegistry;
    CAccountManager*           m_pAccountManager;
    CLatentTransferManager*    m_pLatentTransferManager;
    CDebugHookManager*         m_pDebugHookManager;
    CPedManager*               m_pPedManager;
    CResourceManager*          m_pResourceManager;
    CAccessControlListManager* m_pACLManager;
    CSettings*                 m_pSettings;
    CZoneNames*                m_pZoneNames;
    ASE*                       m_pASE;
    CHandlingManager*          m_pHandlingManager;
    CRPCFunctions*             m_pRPCFunctions;
    CLanBroadcast*             m_pLanBroadcast;
    CWaterManager*             m_pWaterManager;

    CWeaponStatManager*      m_pWeaponStatsManager;
    CBuildingRemovalManager* m_pBuildingRemovalManager;

    std::shared_ptr<CTrainTrackManager> m_pTrainTrackManager;

    CCustomWeaponManager* m_pCustomWeaponManager;
    CFunctionUseLogger*   m_pFunctionUseLogger;

    char* m_szCurrentFileName;

    // This client represents the console input
    CConsoleClient* m_pConsoleClient;

    float m_fGravity;
    float m_fGameSpeed;
    float m_fJetpackMaxHeight;
    float m_fAircraftMaxHeight;
    float m_fAircraftMaxVelocity;
    float m_fAircraftMaxVelocity_Sq;
    bool  m_bOcclusionsEnabled;
    bool  m_bUsingMtaServerConf;

    unsigned char m_ucTrafficLightState;
    bool          m_bTrafficLightsLocked;
    long long     m_llLastTrafficUpdate;

    unsigned char m_ucSkyGradientTR, m_ucSkyGradientTG, m_ucSkyGradientTB;
    unsigned char m_ucSkyGradientBR, m_ucSkyGradientBG, m_ucSkyGradientBB;
    bool          m_bHasSkyGradient;

    SHeatHazeSettings m_HeatHazeSettings;
    bool              m_bHasHeatHaze;

    bool          m_bOverrideWaterColor;
    unsigned char m_ucWaterRed, m_ucWaterGreen, m_ucWaterBlue, m_ucWaterAlpha;

    bool m_bInteriorSoundsEnabled;

    bool  m_bOverrideRainLevel;
    float m_fRainLevel;

    bool  m_bOverrideSunSize;
    float m_fSunSize;

    bool m_bOverrideMoonSize;
    int  m_iMoonSize;

    bool          m_bOverrideSunColor;
    unsigned char m_ucSunCoreR, m_ucSunCoreG, m_ucSunCoreB, m_ucSunCoronaR, m_ucSunCoronaG, m_ucSunCoronaB;

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
