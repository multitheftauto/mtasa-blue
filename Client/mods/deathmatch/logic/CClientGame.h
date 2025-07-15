/*****************************************************************************
/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientGame.h
 *  PURPOSE:     Header for client game class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CPacketHandler.h"
#include <CClientCommon.h>
#include <CClientManager.h>
#include "Config.h"
#include "CBlendedWeather.h"
#include "CMovingObjectsManager.h"
#include "CNametags.h"
#include "CNetAPI.h"
#include "CNetworkStats.h"
#include "CSyncDebug.h"
#include "CServer.h"
#include "CTransferBox.h"
#include "rpc/CRPCFunctions.h"
#include "CUnoccupiedVehicleSync.h"
#include "CPedSync.h"
#include "CObjectSync.h"
#include "CPlayerMap.h"
#include "CClientTeamManager.h"
#include "CClientPedManager.h"
#include "lua/CLuaManager.h"
#include "CScriptDebugging.h"
#include "CZoneNames.h"
#include "CEvents.h"
#include "CResourceManager.h"
#include "CScriptKeyBinds.h"
#include "CElementDeleter.h"
#include "CRegisteredCommands.h"
#include "CClientGUIElement.h"
#include "CLocalServer.h"
#include "CVoiceRecorder.h"
#include "CSingularFileDownloadManager.h"
#include "CObjectRespawner.h"

#define HeliKill_List_Clear_Rate        500
#define MIN_PUSH_ANTISPAM_RATE          1500
#define INVALID_DOWNLOAD_PRIORITY_GROUP (INT_MIN)

class CClientModelCacheManager;
class CDebugHookManager;
class CResourceFileDownloadManager;
class CServerInfo;
enum class eAnimID;

struct SVehExtrapolateSettings
{
    int  iBaseMs;
    int  iScalePercent;
    int  iMaxMs;
    bool bEnabled;
};

struct SMiscGameSettings
{
    bool bUseAltPulseOrder;
    bool bAllowFastSprintFix;
    bool bAllowBadDrivebyHitboxFix;
    bool bAllowShotgunDamageFix;
};

struct ResetWorldPropsInfo
{
    bool resetSpecialProperties{};
    bool resetWorldProperties{};
    bool resetWeatherProperties{};
    bool resetLODs{};
    bool resetSounds{};
};

class CClientGame
{
    friend class CPacketHandler;
    typedef std::map<CAnimBlendAssociationSAInterface*, std::shared_ptr<CIFPAnimations> > AnimAssociations_type;
    typedef std::map<class CTaskSimpleRunNamedAnimSAInterface*, CClientPed*>              RunNamedAnimTask_type;

public:
    enum eStatus
    {
        STATUS_CONNECTING,
        STATUS_JOINING,
        STATUS_JOINED,
    };

    enum eServerType
    {
        SERVER_TYPE_NORMAL,
        SERVER_TYPE_LOCAL,
        SERVER_TYPE_EDITOR,
    };

    enum
    {
        WASTED_DIED,
        WASTED_DROWNED,
        WASTED_KILLED,
    };

    enum
    {
        DUMMY,
        PLAYER,
        VEHICLE,
        OBJECT,
        MARKER,
        BLIP,
        PICKUP,
        RADAR_AREA,
        SPAWNPOINT_DEPRECATED,
        REMOTECLIENT,
        CONSOLE,
        PATH_NODE,
        WORLD_MESH,
        TEAM,
        PED,
        COLSHAPE,
        SCRIPTFILE,
        WATER,
        WEAPON,
        _DATABASE_CONNECTION, // server only
        TRAIN_TRACK,
        ROOT,
        UNKNOWN,
        BUILDING,
        POINTLIGHTS,
    };

    enum
    {
        MARKER_CHECKPOINT,
        MARKER_RING,
        MARKER_CYLINDER,
        MARKER_ARROW,
        MARKER_CORONA,
        MARKER_INVALID = 0xFF,
    };

    enum
    {
        PICKUP_HEALTH,
        PICKUP_ARMOR,
        PICKUP_WEAPON,
        PICKUP_CUSTOM,
        PICKUP_LAST,
        PICKUP_INVALID = 0xFF,
    };

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

    enum eQuitReason
    {
        QUIT_QUIT,
        QUIT_KICK,
        QUIT_BAN,
        QUIT_CONNECTION_DESYNC,
        QUIT_TIMEOUT,
    };
    enum
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
        GLITCH_VEHICLE_RAPID_STOP,
        NUM_GLITCHES
    };

    class CStoredWeaponSlot
    {
    public:
        CStoredWeaponSlot()
        {
            ucType = 0;
            usAmmo = 0;
            usAmmoInClip = 0;
        }
        unsigned char  ucType;
        unsigned short usAmmo;
        unsigned short usAmmoInClip;
    };
    struct SScreenShotArgs
    {
        uint    uiMaxBandwidth;
        uint    uiMaxPacketSize;
        uint    uiServerSentTime;
        ushort  usResourceNetId;
        SString strTag;
    };
    struct SDelayedPacketInfo
    {
        CTickCount             useTickCount;
        unsigned char          ucPacketID;
        NetBitStreamInterface* pBitStream;
        NetPacketPriority      packetPriority;
        NetPacketReliability   packetReliability;
        ePacketOrdering        packetOrdering;
    };

public:
    ZERO_ON_NEW
    CClientGame(bool bLocalPlay = false);
    ~CClientGame();

    bool StartGame(const char* szNick, const char* szPassword, eServerType Type = SERVER_TYPE_NORMAL);
    bool StartLocalGame(eServerType Type, const char* szPassword = NULL);
    void SetupLocalGame(eServerType Type);
    // bool                                    StartGame                       ( void );
    bool IsLocalGame() const { return m_bLocalPlay; }
    bool OnCancelLocalGameClick(CGUIElement* pElement);

    void DoPulsePreFrame();
    void DoPulsePreHUDRender(bool bDidUnminimize, bool bDidRecreateRenderTargets);
    void DoPulsePostFrame();
    void DoPulses();
    void DoPulses2(bool bCalledFromIdle);

    uint GetFrameCount() { return m_uiFrameCount; }

    void        HandleException(CExceptionInformation* pExceptionInformation);
    static void HandleRadioNext(CControlFunctionBind*);
    static void HandleRadioPrevious(CControlFunctionBind*);

    bool IsNickValid(const char* szNick);
    bool IsNametagValid(const char* szNick);

    void StartPlayback();
    void EnablePacketRecorder(const char* szFilename);
    void InitVoice(bool bEnabled, unsigned int uiServerSampleRate, unsigned char ucQuality, unsigned int uiBitrate);

    bool IsWindowFocused() const { return m_bFocused; }

    // Accessors

    CVoiceRecorder*               GetVoiceRecorder() { return m_pVoiceRecorder; };
    CClientManager*               GetManager() { return m_pManager; };
    CClientObjectManager*         GetObjectManager() { return m_pObjectManager; };
    CClientPickupManager*         GetPickupManager() { return m_pPickupManager; };
    CClientPlayerManager*         GetPlayerManager() { return m_pPlayerManager; };
    CClientRadarMarkerManager*    GetRadarMarkerManager() { return m_pRadarMarkerManager; };
    CClientVehicleManager*        GetVehicleManager() { return m_pVehicleManager; };
    CClientTeamManager*           GetTeamManager() { return m_pTeamManager; };
    CClientPedManager*            GetPedManager() { return m_pPedManager; };
    CClientGUIManager*            GetGUIManager() { return m_pGUIManager; };
    CResourceManager*             GetResourceManager() { return m_pResourceManager; }
    CLuaManager*                  GetLuaManager() { return m_pLuaManager; }
    CScriptKeyBinds*              GetScriptKeyBinds() { return m_pScriptKeyBinds; }
    CScriptDebugging*             GetScriptDebugging() { return m_pScriptDebugging; }
    CRegisteredCommands*          GetRegisteredCommands() { return &m_RegisteredCommands; }
    CZoneNames*                   GetZoneNames() { return m_pZoneNames; };
    CNametags*                    GetNametags() { return m_pNametags; }
    CSyncDebug*                   GetSyncDebug() { return m_pSyncDebug; };
    CRPCFunctions*                GetRPCFunctions() { return m_pRPCFunctions; }
    CSingularFileDownloadManager* GetSingularFileDownloadManager() { return m_pSingularFileDownloadManager; };
    CServerInfo*                  GetServerInfo() { return m_ServerInfo.get(); }

    CClientEntity* GetRootEntity() { return m_pRootEntity; }
    CEvents*       GetEvents() { return &m_Events; }

    CBlendedWeather*       GetBlendedWeather() { return m_pBlendedWeather; };
    CNetAPI*               GetNetAPI() { return m_pNetAPI; };
    CPlayerMap*            GetPlayerMap() { return m_pPlayerMap; };
    CMovingObjectsManager* GetMovingObjectsManager() { return m_pMovingObjectsManager; }

    CClientPlayer*       GetLocalPlayer() { return m_pLocalPlayer; }
    const CClientPlayer* GetLocalPlayer() const { return m_pLocalPlayer; }
    void                 ResetLocalPlayer() { m_pLocalPlayer = nullptr; }

    CUnoccupiedVehicleSync* GetUnoccupiedVehicleSync() { return m_pUnoccupiedVehicleSync; }
    CPedSync*               GetPedSync() { return m_pPedSync; }
#ifdef WITH_OBJECT_SYNC
    CObjectSync* GetObjectSync() { return m_pObjectSync; }
#endif
    CLatentTransferManager*   GetLatentTransferManager() { return m_pLatentTransferManager; }
    CClientModelCacheManager* GetModelCacheManager() { return m_pModelCacheManager; }
    CDebugHookManager*        GetDebugHookManager() { return m_pDebugHookManager; }

    CElementDeleter*              GetElementDeleter() { return &m_ElementDeleter; }
    CObjectRespawner*             GetObjectRespawner() { return &m_ObjectRespawner; }
    CRemoteCalls*                 GetRemoteCalls() { return m_pRemoteCalls; }
    CResourceFileDownloadManager* GetResourceFileDownloadManager() { return m_pResourceFileDownloadManager; }

    CModelRenderer* GetModelRenderer() const noexcept { return m_pModelRenderer.get(); }

    SharedUtil::CAsyncTaskScheduler* GetAsyncTaskScheduler() { return m_pAsyncTaskScheduler; }

    // Status toggles
    void ShowNetstat(int iCmd);
    void ShowFPS(bool bShow) { m_bShowFPS = bShow; };

#if defined(MTA_DEBUG) || defined(MTA_BETA)
    void ShowSyncingInfo(bool bShow) { m_bShowSyncingInfo = bShow; };
#endif

#ifdef MTA_WEPSYNCDBG
    void ShowWepdata(const char* szNick);
#endif

#ifdef MTA_DEBUG
    void ShowWepdata(const char* szNick);
    void ShowTasks(const char* szNick);
    void ShowPlayer(const char* szNick);
    void SetMimic(unsigned int uiMimicCount);
    void SetMimicLag(bool bMimicLag) { m_bMimicLag = bMimicLag; };
    void SetDoPaintballs(bool bDoPaintballs) { m_bDoPaintballs = bDoPaintballs; }
    void ShowInterpolation(bool bShow) { m_bShowInterpolation = bShow; }
    bool IsShowingInterpolation() const { return m_bShowInterpolation; }
#endif

    CEntity*       GetTargetedGameEntity() { return m_pTargetedGameEntity; }
    CClientEntity* GetTargetedEntity() { return m_pTargetedEntity; }

    CClientPlayer* GetTargetedPlayer() { return (m_TargetedPlayerID != INVALID_ELEMENT_ID) ? m_pPlayerManager->Get(m_TargetedPlayerID) : NULL; }

    CClientEntity* GetDamageEntity() { return m_pDamageEntity; }
    ElementID      GetDamagerID() { return m_DamagerID; }
    unsigned char  GetDamageWeapon() { return m_ucDamageWeapon; }
    unsigned char  GetDamageBodyPiece() { return m_ucDamageBodyPiece; }
    bool           GetDamageSent() { return m_bDamageSent; }
    void           SetDamageSent(bool b) { m_bDamageSent = b; }

    void SetAllDimensions(unsigned short usDimension);
    void SetAllInteriors(unsigned char ucInterior);

    static bool StaticKeyStrokeHandler(const SString& strKey, bool bState, bool bIsConsoleInputKey);
    bool        KeyStrokeHandler(const SString& strKey, bool bState, bool bIsConsoleInputKey);
    static bool StaticCharacterKeyHandler(WPARAM wChar);
    bool        CharacterKeyHandler(WPARAM wChar);

    static void StaticProcessClientKeyBind(CKeyFunctionBind* pBind);
    void        ProcessClientKeyBind(CKeyFunctionBind* pBind);
    static void StaticProcessClientControlBind(CControlFunctionBind* pBind);
    void        ProcessClientControlBind(CControlFunctionBind* pBind);

    static void StaticProcessServerKeyBind(CKeyFunctionBind* pBind);
    void        ProcessServerKeyBind(CKeyFunctionBind* pBind);
    static void StaticProcessServerControlBind(CControlFunctionBind* pBind);
    void        ProcessServerControlBind(CControlFunctionBind* pBind);

    bool ProcessMessageForCursorEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool AreCursorEventsEnabled() { return m_bCursorEventsEnabled; }
    void SetCursorEventsEnabled(bool bCursorEventsEnabled) { m_bCursorEventsEnabled = bCursorEventsEnabled; }

    CClientPlayer* GetClosestRemotePlayer(const CVector& vecTemp, float fMaxDistance);

    void AddBuiltInEvents();

    const char* GetModRoot() { return m_strModRoot; };

    void SetGameSpeed(float fSpeed);
    void SetMinuteDuration(unsigned long ulDelay);
    long GetMoney() { return m_lMoney; }
    void SetMoney(long lMoney, bool bInstant = false);
    void SetWanted(DWORD dwWanted);

    void ResetAmmoInClip();

    void ResetMapInfo();

    void DoWastedCheck(ElementID damagerID = INVALID_ELEMENT_ID, unsigned char ucWeapon = 0xFF, unsigned char ucBodyPiece = 0xFF, AssocGroupId animGroup = 0,
                       AnimationId animId = 15);
    void SendPedWastedPacket(CClientPed* Ped, ElementID damagerID = INVALID_ELEMENT_ID, unsigned char ucWeapon = 0xFF, unsigned char ucBodyPiece = 0xFF,
                             AssocGroupId animGroup = 0, AnimationId animID = 15);

    CClientGUIElement* GetClickedGUIElement() { return m_pClickedGUIElement; }
    void               SetClickedGUIElement(CClientGUIElement* pElement) { m_pClickedGUIElement = NULL; }

    void SetInitiallyFadedOut(bool bFadedOut) { m_bInitiallyFadedOut = bFadedOut; };

    bool GetHudAreaNameDisabled() { return m_bHudAreaNameDisabled; };
    void SetHudAreaNameDisabled(bool bDisabled) { m_bHudAreaNameDisabled = bDisabled; };

    bool SetGlitchEnabled(unsigned char cGlitch, bool bEnabled);
    bool IsGlitchEnabled(unsigned char cGlitch);

    bool SetWorldSpecialProperty(const WorldSpecialProperty property, const bool enabled) noexcept;
    bool IsWorldSpecialProperty(const WorldSpecialProperty property);

    bool SetCloudsEnabled(bool bEnabled);
    bool GetCloudsEnabled();

    bool SetBirdsEnabled(bool bEnabled);
    bool GetBirdsEnabled();

    void SetWeaponRenderEnabled(bool enabled);
    bool IsWeaponRenderEnabled() const;

    void SetVehicleEngineAutoStartEnabled(bool enabled);
    bool IsVehicleEngineAutoStartEnabled() const;

    void ResetWorldProperties(const ResetWorldPropsInfo& resetPropsInfo);

    CTransferBox* GetTransferBox() { return m_pTransferBox; };

    void ChangeVehicleWeapon(bool bNext);
    void NotifyBigPacketProgress(unsigned long ulBytesReceived, unsigned long ulTotalSize);
    bool IsDownloadingBigPacket() const { return m_bReceivingBigPacket; }
    bool IsBeingDeleted() { return m_bBeingDeleted; }

    void SetDevelopmentMode(bool bEnable, bool bEnableWeb);
    bool GetDevelopmentMode() { return m_bDevelopmentMode; }
    void SetShowCollision(bool bEnable) { m_bShowCollision = bEnable; }
    bool GetShowCollision() { return m_bShowCollision; }
    void SetShowSound(bool bEnable) { m_bShowSound = bEnable; }
    bool GetShowSound() { return m_bShowSound; }

    void        TakePlayerScreenShot(uint uiSizeX, uint uiSizeY, const SString& strTag, uint uiQuality, uint uiMaxBandwidth, uint uiMaxPacketSize,
                                     CResource* pResource, uint uiServerSentTime);
    static void StaticGottenPlayerScreenShot(const CBuffer* pBuffer, uint uiTimeSpentInQueue, const SString& strError);
    void        GottenPlayerScreenShot(const CBuffer* pBuffer, uint uiTimeSpentInQueue, const SString& strError);
    void        ProcessDelayedSendList();

    void SetWeaponTypesUsingBulletSync(const std::set<eWeaponType>& weaponTypesUsingBulletSync);
    bool GetWeaponTypeUsesBulletSync(eWeaponType weaponType);

    SString GetHTTPURL() { return m_strHTTPDownloadURL; };
    void    ProjectileInitiateHandler(CClientProjectile* pProjectile);
    void    IdleHandler();
    void    OutputServerInfo();
    void    TellServerSomethingImportant(uint uiId, const SString& strMessage, uint uiSendLimitForThisId = 0);
    void    ChangeFloatPrecision(bool bHigh);
    bool    IsHighFloatPrecision() const;

    bool TriggerBrowserRequestResultEvent(const std::unordered_set<SString>& newPages);
    void RestreamModel(unsigned short usModel);
    void RestreamWorld();
    void ReinitMarkers();

    void OnWindowFocusChange(bool state);

private:
    // CGUI Callbacks
    bool OnKeyDown(CGUIKeyEventArgs Args);
    bool OnMouseClick(CGUIMouseEventArgs Args);
    bool OnMouseDoubleClick(CGUIMouseEventArgs Args);
    bool OnMouseButtonDown(CGUIMouseEventArgs Args);
    bool OnMouseButtonUp(CGUIMouseEventArgs Args);
    bool OnMouseMove(CGUIMouseEventArgs Args);
    bool OnMouseEnter(CGUIMouseEventArgs Args);
    bool OnMouseLeave(CGUIMouseEventArgs Args);
    bool OnMouseWheel(CGUIMouseEventArgs Args);
    bool OnMove(CGUIElement* pElement);
    bool OnSize(CGUIElement* pElement);
    bool OnFocusGain(CGUIFocusEventArgs Args);
    bool OnFocusLoss(CGUIFocusEventArgs Args);

    // Network update functions
    void DoVehicleInKeyCheck();
    void UpdatePlayerTarget();
    void UpdatePlayerWeapons();
    void UpdateTrailers();
    void UpdateFireKey();
    void UpdateStunts();

    // Internal key-bind callbacks
    static void StaticUpdateFireKey(CControlFunctionBind* pBind);

    void DrawFPS();

#ifdef MTA_DEBUG
    void DrawTasks(CClientPlayer* pPlayer);
    void DrawPlayerDetails(CClientPlayer* pPlayer);
    void UpdateMimics();
    void DoPaintballs();
    void DrawWeaponsyncData(CClientPlayer* pPlayer);
#endif

#ifdef MTA_WEPSYNCDBG
    void DrawWeaponsyncData(CClientPlayer* pPlayer);
#endif

    void DownloadSingularResourceFiles();

    void QuitPlayer(CClientPlayer* pPlayer, eQuitReason Reason);

    void Event_OnIngame();
    void Event_OnIngameAndConnected();

    void SetupGlobalLuaEvents();

    static bool                              StaticDamageHandler(CPed* pDamagePed, CEventDamage* pEvent);
    static void                              StaticDeathHandler(CPed* pKilledPed, unsigned char ucDeathReason, unsigned char ucBodyPart);
    static void                              StaticFireHandler(CFire* pFire);
    static bool                              StaticBreakTowLinkHandler(CVehicle* pTowedVehicle);
    static void                              StaticDrawRadarAreasHandler();
    static void                              StaticRender3DStuffHandler();
    static void                              StaticPreRenderSkyHandler();
    static void                              StaticRenderHeliLightHandler();
    static void                              StaticRenderEverythingBarRoadsHandler();
    static bool                              StaticChokingHandler(unsigned char ucWeaponType);
    static void                              StaticPreWorldProcessHandler();
    static void                              StaticPostWorldProcessHandler();
    static void                              StaticPostWorldProcessPedsAfterPreRenderHandler();
    static void                              StaticPreFxRenderHandler();
    static void                              StaticPostColorFilterRenderHandler();
    static void                              StaticPreHudRenderHandler();
    static void                              StaticCAnimBlendAssocDestructorHandler(CAnimBlendAssociationSAInterface* pThis);
    static CAnimBlendAssociationSAInterface* StaticAddAnimationHandler(RpClump* pClump, AssocGroupId animGroup, AnimationId animID);
    static CAnimBlendAssociationSAInterface* StaticAddAnimationAndSyncHandler(RpClump* pClump, CAnimBlendAssociationSAInterface* pAnimAssocToSyncWith,
                                                                              AssocGroupId animGroup, AnimationId animID);
    static bool                              StaticAssocGroupCopyAnimationHandler(CAnimBlendAssociationSAInterface* pAnimAssoc, RpClump* pClump,
                                                                                  CAnimBlendAssocGroupSAInterface* pAnimAssocGroup, eAnimID animID);
    static bool StaticBlendAnimationHierarchyHandler(CAnimBlendAssociationSAInterface* pAnimAssoc, CAnimBlendHierarchySAInterface** pOutAnimHierarchy,
                                                     int* pFlags, RpClump* pClump);
    static bool StaticProcessCollisionHandler(CEntitySAInterface* pThisInterface, CEntitySAInterface* pOtherInterface);
    static bool StaticVehicleCollisionHandler(CVehicleSAInterface*& pThisInterface, CEntitySAInterface* pOtherInterface, int iModelIndex,
                                              float fDamageImpulseMag, float fCollidingDamageImpulseMag, uint16 usPieceType, CVector vecCollisionPos,
                                              CVector vecCollisionVelocity, bool isProjectile);
    static bool StaticVehicleDamageHandler(CEntitySAInterface* pVehicleInterface, float fLoss, CEntitySAInterface* pAttackerInterface, eWeaponType weaponType,
                                           const CVector& vecDamagePos, uchar ucTyre);
    static bool StaticHeliKillHandler(CVehicleSAInterface* pHeli, CEntitySAInterface* pHitInterface);
    static bool StaticObjectDamageHandler(CObjectSAInterface* pObjectInterface, float fLoss, CEntitySAInterface* pAttackerInterface);
    static bool StaticObjectBreakHandler(CObjectSAInterface* pObjectInterface, CEntitySAInterface* pAttackerInterface);
    static bool StaticWaterCannonHandler(CVehicleSAInterface* pCannonVehicle, CPedSAInterface* pHitPed);
    static bool StaticVehicleFellThroughMapHandler(CVehicleSAInterface* pVehicle);
    static void StaticGameObjectDestructHandler(CEntitySAInterface* pObject);
    static void StaticGameVehicleDestructHandler(CEntitySAInterface* pVehicle);
    static void StaticGamePlayerDestructHandler(CEntitySAInterface* pPlayer);
    static void StaticGameProjectileDestructHandler(CEntitySAInterface* pProjectile);
    static void StaticGameModelRemoveHandler(ushort usModelId);
    static void StaticGameRunNamedAnimDestructorHandler(class CTaskSimpleRunNamedAnimSAInterface* pTask);
    static bool StaticWorldSoundHandler(const SWorldSoundEvent& event);
    static void StaticGameEntityRenderHandler(CEntitySAInterface* pEntity);
    static void StaticTaskSimpleBeHitHandler(CPedSAInterface* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId);
    static void StaticFxSystemDestructionHandler(void* pFxSAInterface);
    static void StaticPedStepHandler(CPedSAInterface* pPed, bool bFoot);
    static void StaticVehicleWeaponHitHandler(SVehicleWeaponHitEvent& event);

    static AnimationId StaticDrivebyAnimationHandler(AnimationId animGroup, AssocGroupId animId);
    static void        StaticAudioZoneRadioSwitchHandler(DWORD dwStationID);

    bool                              DamageHandler(CPed* pDamagePed, CEventDamage* pEvent);
    void                              DeathHandler(CPed* pKilledPed, unsigned char ucDeathReason, unsigned char ucBodyPart);
    void                              FireHandler(CFire* pFire);
    bool                              BreakTowLinkHandler(CVehicle* pTowedVehicle);
    void                              DrawRadarAreasHandler();
    void                              Render3DStuffHandler();
    void                              PreRenderSkyHandler();
    bool                              ChokingHandler(unsigned char ucWeaponType);
    void                              PreWorldProcessHandler();
    void                              PostWorldProcessHandler();
    void                              PostWorldProcessPedsAfterPreRenderHandler();
    void                              CAnimBlendAssocDestructorHandler(CAnimBlendAssociationSAInterface* pThis);
    CAnimBlendAssociationSAInterface* AddAnimationHandler(RpClump* pClump, AssocGroupId animGroup, AnimationId animID);
    CAnimBlendAssociationSAInterface* AddAnimationAndSyncHandler(RpClump* pClump, CAnimBlendAssociationSAInterface* pAnimAssocToSyncWith,
                                                                 AssocGroupId animGroup, AnimationId animID);
    bool        AssocGroupCopyAnimationHandler(CAnimBlendAssociationSAInterface* pAnimAssoc, RpClump* pClump, CAnimBlendAssocGroupSAInterface* pAnimAssocGroup,
                                               eAnimID animID);
    bool        BlendAnimationHierarchyHandler(CAnimBlendAssociationSAInterface* pAnimAssoc, CAnimBlendHierarchySAInterface** pOutAnimHierarchy, int* pFlags,
                                               RpClump* pClump);
    bool        ProcessCollisionHandler(CEntitySAInterface* pThisInterface, CEntitySAInterface* pOtherInterface);
    bool        VehicleCollisionHandler(CVehicleSAInterface*& pCollidingVehicle, CEntitySAInterface* pCollidedVehicle, int iModelIndex, float fDamageImpulseMag,
                                        float fCollidingDamageImpulseMag, uint16 usPieceType, CVector vecCollisionPos, CVector vecCollisionVelocity, bool isProjectile);
    bool        VehicleDamageHandler(CEntitySAInterface* pVehicleInterface, float fLoss, CEntitySAInterface* pAttackerInterface, eWeaponType weaponType,
                                     const CVector& vecDamagePos, uchar ucTyre);
    bool        HeliKillHandler(CVehicleSAInterface* pHeli, CEntitySAInterface* pHitInterface);
    bool        ObjectDamageHandler(CObjectSAInterface* pObjectInterface, float fLoss, CEntitySAInterface* pAttackerInterface);
    bool        ObjectBreakHandler(CObjectSAInterface* pObjectInterface, CEntitySAInterface* pAttackerInterface);
    bool        WaterCannonHitHandler(CVehicleSAInterface* pCannonVehicle, CPedSAInterface* pHitPed);
    bool        VehicleFellThroughMapHandler(CVehicleSAInterface* pVehicle);
    void        GameObjectDestructHandler(CEntitySAInterface* pObject);
    void        GameVehicleDestructHandler(CEntitySAInterface* pVehicle);
    void        GamePlayerDestructHandler(CEntitySAInterface* pPlayer);
    void        GameProjectileDestructHandler(CEntitySAInterface* pProjectile);
    void        GameModelRemoveHandler(ushort usModelId);
    void        GameRunNamedAnimDestructorHandler(class CTaskSimpleRunNamedAnimSAInterface* pTask);
    bool        WorldSoundHandler(const SWorldSoundEvent& event);
    void        TaskSimpleBeHitHandler(CPedSAInterface* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId);
    AnimationId DrivebyAnimationHandler(AnimationId animGroup, AssocGroupId animId);
    void        AudioZoneRadioSwitchHandler(DWORD dwStationID);

    static bool StaticProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    bool        ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static bool PreWeaponFire(CPlayerPed* pPlayerPed, bool bStopIfUsingBulletSync);
    static void PostWeaponFire();
    static void BulletImpact(CPed* pInitiator, CEntity* pVictim, const CVector* pStartPosition, const CVector* pEndPosition);
    static void BulletFire(CPed* pInitiator, const CVector* pStartPosition, const CVector* pEndPosition);

public:
    static bool StaticProcessPacket(unsigned char ucPacketID, NetBitStreamInterface& bitStream);

    bool ApplyPedDamageFromGame(eWeaponType weaponType, float fDamage, uchar ucHitZone, CClientPed* pDamagedPed, CClientEntity* pInflictingEntity,
                                CEventDamage* pEvent);

    bool VerifySADataFiles(int iEnableClientChecks = 0);
    void DebugElementRender();

    void SendExplosionSync(const CVector& vecPosition, eExplosionType Type, CClientEntity* pOrigin = nullptr, std::optional<VehicleBlowState> vehicleBlowState = std::nullopt);
    void SendFireSync(CFire* pFire);
    void SendProjectileSync(CClientProjectile* pProjectile);

    void           SetServerVersionSortable(const SString& strVersion) { m_strServerVersionSortable = strVersion; }
    const SString& GetServerVersionSortable() { return m_strServerVersionSortable; }

    void SetTransferringSingularFiles(bool bTransfer) { m_bTransferringSingularFiles = bTransfer; }
    bool IsTransferringSingularFiles() { return m_bTransferringSingularFiles; }

    void                           SetVehExtrapolateSettings(const SVehExtrapolateSettings& settings) { m_VehExtrapolateSettings = settings; }
    const SVehExtrapolateSettings& GetVehExtrapolateSettings() { return m_VehExtrapolateSettings; }
    void                           SetMiscGameSettings(const SMiscGameSettings& settings) { m_MiscGameSettings = settings; }
    const SMiscGameSettings&       GetMiscGameSettings() { return m_MiscGameSettings; }
    bool                           IsUsingAlternatePulseOrder(bool bAdvanceDelayCounter = false);
    void                           SetFileCacheRoot();
    const char*                    GetFileCacheRoot() { return m_strFileCacheRoot; }

    void                        InsertIFPPointerToMap(const unsigned int u32BlockNameHash, const std::shared_ptr<CClientIFP>& pIFP);
    void                        RemoveIFPPointerFromMap(const unsigned int u32BlockNameHash);
    std::shared_ptr<CClientIFP> GetIFPPointerFromMap(const unsigned int u32BlockNameHash);

    void        InsertPedPointerToSet(CClientPed* pPed);
    void        RemovePedPointerFromSet(CClientPed* pPed);
    CClientPed* GetClientPedByClump(const RpClump& Clump);

    void OnClientIFPUnload(const std::shared_ptr<CClientIFP>& IFP);

    void InsertAnimationAssociationToMap(CAnimBlendAssociationSAInterface* pAnimAssociation, const std::shared_ptr<CIFPAnimations>& pIFPAnimations);
    void RemoveAnimationAssociationFromMap(CAnimBlendAssociationSAInterface* pAnimAssociation);
    void InsertRunNamedAnimTaskToMap(class CTaskSimpleRunNamedAnimSAInterface* pTask, CClientPed* pPed);

    void PedStepHandler(CPedSAInterface* pPed, bool bFoot);
    void VehicleWeaponHitHandler(SVehicleWeaponHitEvent& event);

private:
    eStatus       m_Status;
    eServerType   m_ServerType;
    unsigned long m_ulTimeStart;
    unsigned long m_ulVerifyTimeStart;
    unsigned long m_ulLastClickTick;
    CVector2D     m_vecLastCursorPosition;
    bool          m_bWaitingForLocalConnect;
    bool          m_bErrorStartingLocal;
    int           m_iLocalConnectAttempts;

    bool m_bIsPlayingBack;
    bool m_bFirstPlaybackFrame;

    CClientManager*            m_pManager;
    CClientCamera*             m_pCamera;
    CClientGUIManager*         m_pGUIManager;
    CClientMarkerManager*      m_pMarkerManager;
    CClientObjectManager*      m_pObjectManager;
    CClientPickupManager*      m_pPickupManager;
    CClientPlayerManager*      m_pPlayerManager;
    CClientDisplayManager*     m_pDisplayManager;
    CClientVehicleManager*     m_pVehicleManager;
    CClientRadarAreaManager*   m_pRadarAreaManager;
    CClientRadarMarkerManager* m_pRadarMarkerManager;
    CClientPathManager*        m_pPathManager;
    CClientTeamManager*        m_pTeamManager;
    CClientPedManager*         m_pPedManager;
    CClientPointLightsManager* m_pPointLightsManager;
    CClientProjectileManager*  m_pProjectileManager;
    CRPCFunctions*             m_pRPCFunctions;
    CUnoccupiedVehicleSync*    m_pUnoccupiedVehicleSync;
    CPedSync*                  m_pPedSync;
#ifdef WITH_OBJECT_SYNC
    CObjectSync* m_pObjectSync;
#endif
    CMovingObjectsManager* m_pMovingObjectsManager;
    CNametags*             m_pNametags;
    CNetAPI*               m_pNetAPI;
    CNetworkStats*         m_pNetworkStats;
    CSyncDebug*            m_pSyncDebug;
    // CScreenshot*                          m_pScreenshot;
    CPlayerMap*                   m_pPlayerMap;
    CTransferBox*                 m_pTransferBox;
    CResourceManager*             m_pResourceManager;
    CScriptKeyBinds*              m_pScriptKeyBinds;
    CElementDeleter               m_ElementDeleter;
    CObjectRespawner              m_ObjectRespawner;
    CZoneNames*                   m_pZoneNames;
    CPacketHandler*               m_pPacketHandler;
    CLocalServer*                 m_pLocalServer;
    CLatentTransferManager*       m_pLatentTransferManager;
    bool                          m_bInitiallyFadedOut;
    bool                          m_bHudAreaNameDisabled;
    CSingularFileDownloadManager* m_pSingularFileDownloadManager;
    CClientModelCacheManager*     m_pModelCacheManager;
    CDebugHookManager*            m_pDebugHookManager;
    CRemoteCalls*                 m_pRemoteCalls;
    CResourceFileDownloadManager* m_pResourceFileDownloadManager;

    std::unique_ptr<CModelRenderer> m_pModelRenderer;

    // Revised facilities
    CServer m_Server;

    CVoiceRecorder* m_pVoiceRecorder;

    CClientPlayer* m_pLocalPlayer;
    ElementID      m_LocalID;
    SString        m_strLocalNick;

    CClientEntity*      m_pRootEntity;
    CLuaManager*        m_pLuaManager;
    CScriptDebugging*   m_pScriptDebugging;
    CRegisteredCommands m_RegisteredCommands;

    std::unique_ptr<CServerInfo> m_ServerInfo;

    // Map statuses
    SString m_strCurrentMapName;
    SString m_strModRoot;

    CBlendedWeather* m_pBlendedWeather;
    bool             m_bShowNametags;
    bool             m_bShowRadar;

    // Status booleans
    bool m_bGameLoaded;
    bool m_bTriggeredIngameAndConnected;
    bool m_bGracefulDisconnect;

    CEntity*       m_pTargetedGameEntity;
    CClientEntity* m_pTargetedEntity;
    bool           m_bTargetSent;
    ElementID      m_TargetedPlayerID;

    CClientEntity* m_pDamageEntity;
    ElementID      m_DamagerID;
    unsigned char  m_ucDamageWeapon;
    unsigned char  m_ucDamageBodyPiece;
    unsigned long  m_ulDamageTime;
    bool           m_bDamageSent;

    eWeaponSlot                            m_lastWeaponSlot;
    SFixedArray<DWORD, WEAPONSLOT_MAX + 1> m_wasWeaponAmmoInClip;

    bool m_bCursorEventsEnabled;
    bool m_bLocalPlay;

    bool m_bShowNetstat;
    bool m_bShowFPS;

    bool m_bTransferringSingularFiles;

    float m_fGameSpeed;
    long  m_lMoney;
    DWORD m_dwWanted;

    SFixedArray<bool, NUM_GLITCHES> m_Glitches;

    // Clouds Enabled
    bool m_bCloudsEnabled;
    // Birds Enabled
    bool m_bBirdsEnabled;

    unsigned long m_ulMinuteDuration;

    CClientGUIElement* m_pClickedGUIElement;

    SString m_strHTTPDownloadURL;

    bool          m_bReceivingBigPacket;
    unsigned long m_ulBigPacketSize;
    unsigned long m_ulBigPacketBytesReceivedBase;
    CTransferBox* m_pBigPacketTransferBox;

    CElapsedTimeHD m_TimeSliceTimer;
    uint           m_uiFrameCount;

    long long m_llLastTransgressionTime;
    SString   m_strLastDiagnosticStatus;

    bool m_bBeingDeleted;            // To enable speedy disconnect

    bool m_bWasMinimized;
    bool m_bFocused;

    // Cache for speeding up collision processing
public:
    std::map<CClientEntity*, bool> m_AllDisabledCollisions;

private:
    std::map<CEntitySAInterface*, CClientEntity*> m_CachedCollisionMap;
    bool                                          m_BuiltCollisionMapThisFrame;

#if defined(MTA_DEBUG) || defined(MTA_BETA)
    bool m_bShowSyncingInfo;
#endif

#ifdef MTA_DEBUG
    CClientPlayer*            m_pShowPlayerTasks;
    CClientPlayer*            m_pShowPlayer;
    std::list<CClientPlayer*> m_Mimics;
    bool                      m_bMimicLag;
    unsigned long             m_ulLastMimicLag;
    CVector                   m_vecLastMimicPos;
    CVector                   m_vecLastMimicMove;
    CVector                   m_vecLastMimicTurn;
    CVector                   m_vecLastMimicRot;
    bool                      m_bDoPaintballs;
    bool                      m_bShowInterpolation;
#endif
    bool m_bDevelopmentMode;
    bool m_bShowCollision;
    bool m_bShowSound;

private:
    CEvents                                     m_Events;
    std::list<SScreenShotArgs>                  m_ScreenShotArgList;
    ushort                                      m_usNextScreenShotId;
    std::list<SDelayedPacketInfo>               m_DelayedSendList;
    std::multimap<CClientVehicle*, CClientPed*> m_HeliCollisionsMap;
    CElapsedTime                                m_LastClearTime;
    SString                                     m_strServerVersionSortable;
    std::set<eWeaponType>                       m_weaponTypesUsingBulletSync;
    GUI_CALLBACK                                m_OnCancelLocalGameClick;

    SVehExtrapolateSettings m_VehExtrapolateSettings;
    SMiscGameSettings       m_MiscGameSettings;
    uint                    m_uiAltPulseOrderCounter;
    SString                 m_strACInfo;
    std::map<uint, uint>    m_SentMessageIds;

    bool              m_bLastKeyWasEscapeCancelled;
    std::set<SString> m_AllowKeyUpMap;
    uint              m_uiPrecisionCallDepth;
    SString           m_strFileCacheRoot;

    SharedUtil::CAsyncTaskScheduler* m_pAsyncTaskScheduler;

    ksignals::Delegate m_Delegate;

    // (unsigned int) Key is the hash of custom block name that is supplied to engineLoadIFP
    std::map<unsigned int, std::shared_ptr<CClientIFP> > m_mapOfIfpPointers;
    std::set<CClientPed*>                                m_setOfPedPointers;
    AnimAssociations_type                                m_mapOfCustomAnimationAssociations;
    // Key is the task and value is the CClientPed*
    RunNamedAnimTask_type m_mapOfRunNamedAnimTasks;
    
    long long m_timeLastDiscordStateUpdate;
};

extern CClientGame* g_pClientGame;
