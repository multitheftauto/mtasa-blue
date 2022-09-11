/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CGame.h
 *  PURPOSE:     Game base interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// use this to check if you're using SA or VC headers
#define GTA_SA

typedef void(InRenderer)();

#include "Common.h"

#include "C3DMarkers.h"
#include "CAERadioTrackManager.h"
#include "CAnimBlendAssociation.h"
#include "CAnimBlock.h"
#include "CAnimManager.h"
#include "CAudioEngine.h"
#include "CAEAudioHardware.h"
#include "CAESoundManager.h"
#include "CAEVehicleAudioEntity.h"
#include "CAudioContainer.h"
#include "CCam.h"
#include "CCamera.h"
#include "CCarEnterExit.h"
#include "CClock.h"
#include "CCheckpoints.h"
#include "CControllerConfigManager.h"
#include "CCoronas.h"
#include "CEventDamage.h"
#include "CEventGunShot.h"
#include "CEventList.h"
#include "CExplosionManager.h"
#include "CFireManager.h"
#include "CFont.h"
#include "CFx.h"
#include "CFxSystem.h"
#include "CFxManager.h"
#include "CGarages.h"
#include "CHandlingManager.h"
#include "CHud.h"
#include "CKeyGen.h"
#include "CMenuManager.h"
#include "CModelInfo.h"
#include "CPad.h"
#include "CPathFind.h"
#include "CPedDamageResponse.h"
#include "CPedModelInfo.h"
#include "CPickups.h"
#include "CPlayerInfo.h"
#include "CPointLights.h"
#include "CPools.h"
#include "CPopulation.h"
#include "CProjectile.h"
#include "CProjectileInfo.h"
#include "CRadar.h"
#include "CRenderWare.h"
#include "CRestart.h"
#include "CRopes.h"
#include "CSettings.h"
#include "CStats.h"
#include "CStreaming.h"
#include "CTaskManagementSystem.h"
#include "CTasks.h"
#include "CTheCarGenerators.h"
#include "CVisibilityPlugins.h"
#include "CWaterManager.h"
#include "CWeaponStatManager.h"
#include "CWeather.h"
#include "CWeaponInfo.h"
#include "CWorld.h"
#include "TaskCarAccessories.h"
#include "CObjectGroupPhysicalProperties.h"
#include "CColStore.h"

#include <windows.h>

enum eEntityType;

typedef bool(PreWeaponFireHandler)(class CPlayerPed* pPlayer, bool bStopIfUsingBulletSync);
typedef void(PostWeaponFireHandler)();
typedef void(TaskSimpleBeHitHandler)(class CPedSAInterface* pPedAttacker, ePedPieceTypes hitBodyPart, int hitBodySide, int weaponId);

enum eGameVersion
{
    VERSION_ALL = 0,
    VERSION_EU_10 = 5,
    VERSION_US_10 = 11,
    VERSION_11 = 15,
    VERSION_20 = 20,
    VERSION_UNKNOWN = 0xFF,
};

struct SMatchChannelStats
{
    SString strTag;
    uint    uiNumMatchedTextures;
    uint    uiNumShaderAndEntities;
};

struct SShaderReplacementStats
{
    uint                               uiNumReplacementRequests;
    uint                               uiNumReplacementMatches;
    uint                               uiTotalTextures;
    uint                               uiTotalShaders;
    uint                               uiTotalEntitesRefed;
    std::map<uint, SMatchChannelStats> channelStatsList;
};

class __declspec(novtable) CGame
{
    typedef std::unique_ptr<CAnimBlendAssocGroup> AssocGroup_type;

public:
    virtual CPools*                   GetPools() = 0;
    virtual CPlayerInfo*              GetPlayerInfo() = 0;
    virtual CProjectileInfo*          GetProjectileInfo() = 0;
    virtual CRadar*                   GetRadar() = 0;
    virtual CRestart*                 GetRestart() = 0;
    virtual CClock*                   GetClock() = 0;
    virtual CCheckpoints*             GetCheckpoints() = 0;
    virtual CCoronas*                 GetCoronas() = 0;
    virtual CEventList*               GetEventList() = 0;
    virtual CFireManager*             GetFireManager() = 0;
    virtual CExplosionManager*        GetExplosionManager() = 0;
    virtual CGarages*                 GetGarages() = 0;
    virtual CHud*                     GetHud() = 0;
    virtual CWeather*                 GetWeather() = 0;
    virtual CWorld*                   GetWorld() = 0;
    virtual CCamera*                  GetCamera() = 0;
    virtual CPickups*                 GetPickups() = 0;
    virtual C3DMarkers*               Get3DMarkers() = 0;
    virtual CPad*                     GetPad() = 0;
    virtual CAERadioTrackManager*     GetAERadioTrackManager() = 0;
    virtual CAudioEngine*             GetAudioEngine() = 0;
    virtual CAEAudioHardware*         GetAEAudioHardware() = 0;
    virtual CAESoundManager*          GetAESoundManager() = 0;
    virtual CAudioContainer*          GetAudioContainer() = 0;
    virtual CMenuManager*             GetMenuManager() = 0;
    virtual CStats*                   GetStats() = 0;
    virtual CTasks*                   GetTasks() = 0;
    virtual CFont*                    GetFont() = 0;
    virtual CPathFind*                GetPathFind() = 0;
    virtual CPopulation*              GetPopulation() = 0;
    virtual CGameSettings*            GetSettings() = 0;
    virtual CCarEnterExit*            GetCarEnterExit() = 0;
    virtual CControllerConfigManager* GetControllerConfigManager() = 0;
    virtual CRenderWare*              GetRenderWare() = 0;
    virtual CHandlingManager*         GetHandlingManager() = 0;
    virtual CAnimManager*             GetAnimManager() = 0;
    virtual CStreaming*               GetStreaming() = 0;
    virtual CVisibilityPlugins*       GetVisibilityPlugins() = 0;
    virtual CKeyGen*                  GetKeyGen() = 0;
    virtual CRopes*                   GetRopes() = 0;
    virtual CFx*                      GetFx() = 0;
    virtual CFxManager*               GetFxManager() = 0;
    virtual CWaterManager*            GetWaterManager() = 0;
    virtual CWeaponStatManager*       GetWeaponStatManager() = 0;
    virtual CPointLights*             GetPointLights() = 0;
    virtual CColStore*                GetCollisionStore() = 0;

    virtual CWeaponInfo* GetWeaponInfo(eWeaponType weapon, eWeaponSkill skill = WEAPONSKILL_STD) = 0;
    virtual CModelInfo*  GetModelInfo(DWORD dwModelID, bool bCanBeInvalid = false) = 0;

    virtual DWORD        GetSystemTime() = 0;
    virtual BOOL         IsAtMenu() = 0;
    virtual BOOL         IsGameLoaded() = 0;
    virtual VOID         StartGame() = 0;
    virtual VOID         SetSystemState(eSystemState State) = 0;
    virtual eSystemState GetSystemState() = 0;
    virtual VOID         Pause(bool bPaused) = 0;
    virtual bool         IsPaused() = 0;
    virtual bool         IsInForeground() = 0;
    virtual VOID         DisableRenderer(bool bDisabled) = 0;
    virtual VOID         SetRenderHook(InRenderer* pInRenderer) = 0;
    virtual VOID         TakeScreenshot(char* szFileName) = 0;
    virtual DWORD*       GetMemoryValue(DWORD dwOffset) = 0;
    virtual void         SetTimeScale(float fTimeScale) = 0;
    virtual float        GetFPS() = 0;
    virtual float        GetTimeStep() = 0;
    virtual float        GetOldTimeStep() = 0;
    virtual float        GetTimeScale() = 0;

    virtual void Initialize() = 0;
    virtual void Reset() = 0;
    virtual void Terminate() = 0;

    virtual BOOL InitLocalPlayer(class CClientPed* pClientPed) = 0;

    virtual float GetGravity() = 0;
    virtual void  SetGravity(float fGravity) = 0;

    virtual float GetGameSpeed() = 0;
    virtual void  SetGameSpeed(float fSpeed) = 0;

    virtual unsigned long GetMinuteDuration() = 0;
    virtual void          SetMinuteDuration(unsigned long ulDelay) = 0;

    virtual unsigned char GetBlurLevel() = 0;
    virtual void          SetBlurLevel(unsigned char ucLevel) = 0;

    virtual void SetJetpackWeaponEnabled(eWeaponType weaponType, bool bEnabled);
    virtual bool GetJetpackWeaponEnabled(eWeaponType weaponType);

    virtual eGameVersion GetGameVersion() = 0;

    virtual bool IsCheatEnabled(const char* szCheatName) = 0;
    virtual bool SetCheatEnabled(const char* szCheatName, bool bEnable) = 0;
    virtual void ResetCheats() = 0;

    virtual bool IsRandomFoliageEnabled() = 0;
    virtual void SetRandomFoliageEnabled(bool bEnable) = 0;

    virtual bool IsMoonEasterEggEnabled() = 0;
    virtual void SetMoonEasterEggEnabled(bool bEnable) = 0;

    virtual CWeapon*     CreateWeapon() = 0;
    virtual CWeaponStat* CreateWeaponStat(eWeaponType weaponType, eWeaponSkill weaponSkill) = 0;

    virtual bool VerifySADataFileNames() = 0;
    virtual bool PerformChecks() = 0;
    virtual int& GetCheckStatus() = 0;

    virtual void SetAsyncLoadingFromScript(bool bScriptEnabled, bool bScriptForced) = 0;
    virtual void SuspendASyncLoading(bool bSuspend, uint uiAutoUnsuspendDelay = 0) = 0;
    virtual bool IsASyncLoadingEnabled(bool bIgnoreSuspend = false) = 0;

    virtual bool HasCreditScreenFadedOut() = 0;
    virtual void FlushPendingRestreamIPL() = 0;
    virtual void ResetModelLodDistances() = 0;
    virtual void ResetAlphaTransparencies() = 0;
    virtual void DisableVSync() = 0;
    virtual void ResetModelTimes() = 0;

    virtual void  OnPedContextChange(CPed* pPedContext) = 0;
    virtual CPed* GetPedContext() = 0;

    virtual void GetShaderReplacementStats(SShaderReplacementStats& outStats) = 0;

    virtual void SetPreWeaponFireHandler(PreWeaponFireHandler* pPreWeaponFireHandler) = 0;
    virtual void SetPostWeaponFireHandler(PostWeaponFireHandler* pPostWeaponFireHandler) = 0;
    virtual void SetTaskSimpleBeHitHandler(TaskSimpleBeHitHandler* pTaskSimpleBeHitHandler) = 0;

    virtual CObjectGroupPhysicalProperties* GetObjectGroupPhysicalProperties(unsigned char ucObjectGroup) = 0;

    virtual int32_t GetBaseIDforDFF() = 0;
    virtual int32_t GetBaseIDforTXD() = 0;
    virtual int32_t GetBaseIDforCOL() = 0;
    virtual int32_t GetBaseIDforIPL() = 0;
    virtual int32_t GetBaseIDforDAT() = 0;
    virtual int32_t GetBaseIDforIFP() = 0;
    virtual int32_t GetBaseIDforRRR() = 0;
    virtual int32_t GetBaseIDforSCM() = 0;
    virtual int32_t GetCountOfAllFileIDs() = 0;
};
