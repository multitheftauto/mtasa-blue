/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <optional>
#include <game/CStreaming.h>
#include <game/CModelInfo.h>
#include <game/CSettings.h>
#include <game/CWorld.h>
#include "CModelCacheManager.h"

namespace
{
    struct SModelCacheInfo
    {
        SModelCacheInfo() : fClosestDistSq(0), bIsModelCachedHere(false), bIsModelLoadedByGame(false) {}
        CTickCount lastNeeded;
        CTickCount firstNeeded;
        float      fClosestDistSq;
        bool       bIsModelCachedHere;
        bool       bIsModelLoadedByGame;
    };

    // When the game itself tracks the model in its loaded ped/vehicle groups, keep our cache ref
    // until the game has more than this many entries left. These floors mirror what the game's
    // own cleanup uses when it decides which group models it can unload.
    constexpr uint PED_STREAMING_FLOOR = 4u;
    constexpr uint VEHICLE_STREAMING_FLOOR_EXTERIOR = 7u;
    constexpr uint VEHICLE_STREAMING_FLOOR_INTERIOR = 4u;
}  // namespace

///////////////////////////////////////////////////////////////
//
//
// CModelCacheManager
//
//
///////////////////////////////////////////////////////////////
class CModelCacheManagerImpl : public CModelCacheManager
{
public:
    ZERO_ON_NEW

    // CModelCacheManager interface
    virtual void DoPulse();
    virtual void GetStats(SModelCacheStats& outStats);
    virtual bool UnloadModel(ushort usModelId);
    virtual void OnRestreamModel(ushort usModelId);
    virtual void OnClientClose();
    virtual void UpdatePedModelCaching(const std::map<ushort, float>& newNeedCacheList);
    virtual void UpdateVehicleModelCaching(const std::map<ushort, float>& newNeedCacheList);
    virtual void SetCustomLimits(std::optional<size_t> numVehicles, std::optional<size_t> numPeds);

    // CModelCacheManagerImpl methods
    CModelCacheManagerImpl();
    ~CModelCacheManagerImpl();

    void PreLoad();
    void RemoveCacheRefs(std::map<ushort, SModelCacheInfo>& currentCacheInfoMap);
    void UpdateModelCaching(const std::map<ushort, float>& newNeededList, std::map<ushort, SModelCacheInfo>& currentCacheInfoMap, uint uiMaxCachedAllowed);
    int  GetModelRefCount(ushort usModelId);
    void AddModelRefCount(ushort usModelId);
    void SubModelRefCount(ushort usModelId);
    bool TryReleaseCachedModel(ushort usModelId, SModelCacheInfo& info, const char* contextTag, uint& uiNumModelsCachedHereOnly);

protected:
    CGame*     m_pGame{};
    int        m_iFrameCounter{};
    CTickCount m_TickCountNow{};
    bool       m_bDonePreLoad{};
    uint       m_uiMaxCachedPedModels{};
    bool       m_IsUsingCustomPedCacheLimit{};  //< If `true` the value is set by the scripter, otherwise is calculated in `DoPulse()`
    uint       m_uiMaxCachedVehicleModels{};
    bool       m_IsUsingCustomVehicleCacheLimit{};  //< If `true` the value is set by the scripter, otherwise is calculated in `DoPulse()`
    std::map<ushort, SModelCacheInfo> m_PedModelCacheInfoMap{};
    std::map<ushort, SModelCacheInfo> m_VehicleModelCacheInfoMap{};
};

///////////////////////////////////////////////////////////////
//
// Global new
//
//
///////////////////////////////////////////////////////////////
CModelCacheManager* NewModelCacheManager()
{
    return new CModelCacheManagerImpl();
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::CModelCacheManagerImpl
//
///////////////////////////////////////////////////////////////
CModelCacheManagerImpl::CModelCacheManagerImpl()
{
    m_pGame = CCore::GetSingleton().GetGame();
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::~CModelCacheManagerImpl
//
// Clean up when quitting
//
///////////////////////////////////////////////////////////////
CModelCacheManagerImpl::~CModelCacheManagerImpl()
{
    OnClientClose();
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::OnClientClose
//
// Clean up when client.dll unloads
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::OnClientClose()
{
    RemoveCacheRefs(m_PedModelCacheInfoMap);
    RemoveCacheRefs(m_VehicleModelCacheInfoMap);

    m_IsUsingCustomPedCacheLimit = m_IsUsingCustomVehicleCacheLimit = false;
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::PreLoad
//
// Cache all weapons and upgrades
//
// Peds KB:                64,832 KB         7-312     306  296 valid, 10 not so valid               219   KB/model             4.45/MB
// Weapons KB:             470               321-372   52   39 valid, 3 invalid(329,332,340)         470   KB all weapons
// Upgrades KB:            2,716             1000-1193 194  all valid                              2,716   KB all upgrades
// Vehicles(400-499) KB:   14,622                                                                    140   KB/model             7/MB
// Vehicles(500-599) KB:   14,888
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::PreLoad()
{
    if (m_bDonePreLoad)
        return;

    m_bDonePreLoad = true;

    CTickCount startTicks = CTickCount::Now();
#if 0
    for ( uint i = 321 ; i <= 372 ; i++ )
    {
        if ( CClientPedManager::IsValidWeaponModel ( i ) )
            AddModelRefCount ( i );
    }
#endif
    m_pGame->GetStreaming()->LoadAllRequestedModels(false);

    // Get current limits
    int bSlowMethod = GetApplicationSettingInt(DIAG_PRELOAD_UPGRADES_SLOW);
    int iLowestUnsafeUpgrade = GetApplicationSettingInt(DIAG_PRELOAD_UPGRADES_LOWEST_UNSAFE);
    SetApplicationSetting(DIAG_CRASH_EXTRA_MSG, "** AUTO FIXING CRASH (Step 1/2) **\n\n** Please continue and connect to a server **");

    // Crashed during previous PreLoad?
    if (WatchDogIsSectionOpen(WD_SECTION_PRELOAD_UPGRADES))
    {
        AddReportLog(8545, SString("PreLoad Upgrades - Crash detect - bSlowMethod:%d  iLowestUnsafeUpgrade:%d", bSlowMethod, iLowestUnsafeUpgrade));
        iLowestUnsafeUpgrade = GetApplicationSettingInt(DIAG_PRELOAD_UPGRADE_ATTEMPT_ID);
        bSlowMethod = 1;
        SetApplicationSettingInt(DIAG_PRELOAD_UPGRADES_LOWEST_UNSAFE, iLowestUnsafeUpgrade);
        SetApplicationSettingInt(DIAG_PRELOAD_UPGRADES_SLOW, bSlowMethod);
        SetApplicationSetting(DIAG_CRASH_EXTRA_MSG, "** AUTO FIXING CRASH (Step 2/2) **\n\n** Please continue and connect to a server **");
    }

    if (iLowestUnsafeUpgrade == 0)
        iLowestUnsafeUpgrade = 1194;

    // PreLoad upgrades
    WatchDogBeginSection(WD_SECTION_PRELOAD_UPGRADES);
    {
        for (int i = 1000; i < iLowestUnsafeUpgrade; i++)
        {
            if (bSlowMethod)
                SetApplicationSettingInt(DIAG_PRELOAD_UPGRADE_ATTEMPT_ID, i);

            AddModelRefCount(static_cast<ushort>(i));

            if (bSlowMethod)
                m_pGame->GetStreaming()->LoadAllRequestedModels(false);
        }

        m_pGame->GetStreaming()->LoadAllRequestedModels(false);
    }
    WatchDogCompletedSection(WD_SECTION_PRELOAD_UPGRADES);
    SetApplicationSetting(DIAG_CRASH_EXTRA_MSG, "");

    // Report if LowestUnsafeUpgrade has fallen
    int iPrevHiScore = GetApplicationSettingInt(DIAG_PRELOAD_UPGRADES_HISCORE);
    SetApplicationSettingInt(DIAG_PRELOAD_UPGRADES_HISCORE, iLowestUnsafeUpgrade);
    if (iPrevHiScore > iLowestUnsafeUpgrade)
        AddReportLog(8544, SString("PreLoad Upgrades - LowestUnsafeUpgrade fallen from %d to %d", iPrevHiScore, iLowestUnsafeUpgrade));

    CTickCount deltaTicks = CTickCount::Now() - startTicks;
    OutputDebugLine(SString("CModelCacheManagerImpl::PreLoad completed in %d ms", deltaTicks.ToInt()));
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::GetStats
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::GetStats(SModelCacheStats& outStats)
{
    outStats.uiMaxNumPedModels = m_uiMaxCachedPedModels;
    outStats.uiMaxNumVehicleModels = m_uiMaxCachedVehicleModels;
    outStats.uiNumPedModels = 0;
    outStats.uiNumVehicleModels = 0;

    for (std::map<ushort, SModelCacheInfo>::const_iterator iter = m_PedModelCacheInfoMap.begin(); iter != m_PedModelCacheInfoMap.end(); ++iter)
        if (iter->second.bIsModelCachedHere)
            outStats.uiNumPedModels++;

    for (std::map<ushort, SModelCacheInfo>::const_iterator iter = m_VehicleModelCacheInfoMap.begin(); iter != m_VehicleModelCacheInfoMap.end(); ++iter)
        if (iter->second.bIsModelCachedHere)
            outStats.uiNumVehicleModels++;
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::SetCustomLimits
//
// Function to set custom limits, instead of calculating them automatically.
// If the optional is empty, the value is restored to the automatic one
// otherwise it is set to whatever value the opt contains
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::SetCustomLimits(std::optional<size_t> numVehicles, std::optional<size_t> numPeds)
{
    if (m_IsUsingCustomPedCacheLimit = numPeds.has_value())
    {
        m_uiMaxCachedPedModels = *numPeds;
    }
    if (m_IsUsingCustomVehicleCacheLimit = numVehicles.has_value())
    {
        m_uiMaxCachedVehicleModels = *numVehicles;
    }
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::DoPulse
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::DoPulse()
{
    m_TickCountNow = CTickCount::Now();

    PreLoad();

    // Adjust cache numbers depending on amount of streaming memory allocated
    //
    //  64MB streaming = 2+1 MB for peds & vehicles          9 peds + 7 veh
    //  96MB streaming = 4+4 MB for peds & vehicles         18 peds + 28 veh
    //  128MB streaming = 8+8 MB for peds & vehicles        36 peds + 56 veh
    //  256MB streaming = 16+8 MB for peds & vehicles       72 peds + 56 veh
    //
    const auto iStreamingMemoryAvailableKB = *(int*)0x08A5A80;
    if (!m_IsUsingCustomPedCacheLimit)
    {
        SSamplePoint<float> pedPoints[] = {{65536, 9}, {98304, 18}, {131072, 36}, {262144, 72}};
        m_uiMaxCachedPedModels = (int)EvalSamplePosition<float>(pedPoints, NUMELMS(pedPoints), (float)iStreamingMemoryAvailableKB);
    }
    if (!m_IsUsingCustomVehicleCacheLimit)
    {
        SSamplePoint<float> vehPoints[] = {{65536, 7}, {98304, 28}, {131072, 56}, {262144, 56}};
        m_uiMaxCachedVehicleModels = (int)EvalSamplePosition<float>(vehPoints, NUMELMS(vehPoints), (float)iStreamingMemoryAvailableKB);
    }
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::UpdatePedModelCaching
//
//
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::UpdatePedModelCaching(const std::map<ushort, float>& newNeedCacheList)
{
    DoPulse();
    UpdateModelCaching(newNeedCacheList, m_PedModelCacheInfoMap, m_uiMaxCachedPedModels);
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::UpdateVehicleModelCaching
//
//
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::UpdateVehicleModelCaching(const std::map<ushort, float>& newNeedCacheList)
{
    DoPulse();
    UpdateModelCaching(newNeedCacheList, m_VehicleModelCacheInfoMap, m_uiMaxCachedVehicleModels);
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::RemoveCacheRefs
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::RemoveCacheRefs(std::map<ushort, SModelCacheInfo>& currentCacheInfoMap)
{
    for (std::map<ushort, SModelCacheInfo>::iterator iter = currentCacheInfoMap.begin(); iter != currentCacheInfoMap.end(); ++iter)
    {
        const ushort     usModelId = iter->first;
        SModelCacheInfo& info = iter->second;

        if (info.bIsModelCachedHere)
        {
            SubModelRefCount(usModelId);
            info.bIsModelCachedHere = false;
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::UpdateModelCaching
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::UpdateModelCaching(const std::map<ushort, float>& newNeedCacheList, std::map<ushort, SModelCacheInfo>& currentCacheInfoMap,
                                                uint uiMaxCachedAllowed)
{
    // Update some flags and remove info for uncached and unneeded
    for (std::map<ushort, SModelCacheInfo>::iterator iter = currentCacheInfoMap.begin(); iter != currentCacheInfoMap.end();)
    {
        const ushort     usModelId = iter->first;
        SModelCacheInfo& info = iter->second;

        info.bIsModelLoadedByGame = GetModelRefCount(usModelId) > (info.bIsModelCachedHere ? 1 : 0);

        if (!info.bIsModelLoadedByGame && !info.bIsModelCachedHere)
        {
            if ((m_TickCountNow - info.lastNeeded).ToInt() > 1000)
            {
                // Not loaded, cached or needed for 1 second, so remove info
                currentCacheInfoMap.erase(iter++);
                continue;
            }
        }
        ++iter;
    }

    // Update current from new needed
    for (std::map<ushort, float>::const_iterator iter = newNeedCacheList.begin(); iter != newNeedCacheList.end(); ++iter)
    {
        SModelCacheInfo& info = MapGet(currentCacheInfoMap, iter->first);
        info.fClosestDistSq = iter->second;
        info.lastNeeded = m_TickCountNow;
        if (info.firstNeeded.ToInt() == 0)
            info.firstNeeded = m_TickCountNow;
    }

    uint uiNumModelsCachedHereOnly = 0;

    // Multimaps so that models sharing a priority key all stay selectable: with a plain map,
    // insertion would silently overwrite earlier entries with an equal key, hiding valid
    // eviction or cache candidates from the loops below.
    std::multimap<uint, ushort> maybeUncacheUnneededList;
    std::multimap<uint, ushort> maybeUncacheNeededList;
    std::multimap<uint, ushort> maybeCacheList;

    // Update active
    for (std::map<ushort, SModelCacheInfo>::iterator iter = currentCacheInfoMap.begin(); iter != currentCacheInfoMap.end(); ++iter)
    {
        const ushort     usModelId = iter->first;
        SModelCacheInfo& info = iter->second;

        if (info.bIsModelLoadedByGame)
        {
            info.lastNeeded = m_TickCountNow;

            // Add cache ref here so when game tries to unload the model, we can keep it loaded
            if (!info.bIsModelCachedHere)
            {
                AddModelRefCount(usModelId);
                info.bIsModelCachedHere = true;
            }
        }
        else
        {
            if (info.bIsModelCachedHere)
            {
                uiNumModelsCachedHereOnly++;
                // Update cached models that could be uncached
                uint uiTicksSinceLastNeeded = (m_TickCountNow - info.lastNeeded).ToInt();
                if (uiTicksSinceLastNeeded > 0)
                    maybeUncacheUnneededList.emplace(uiTicksSinceLastNeeded, usModelId);
                else
                    maybeUncacheNeededList.emplace((int)info.fClosestDistSq, usModelId);
            }
            else
            {
                if (info.lastNeeded == m_TickCountNow)
                {
                    // Update uncached models that could be cached
                    uint uiTicksSinceFirstNeeded = (m_TickCountNow - info.firstNeeded).ToInt();
                    maybeCacheList.emplace(uiTicksSinceFirstNeeded, usModelId);
                }
            }
        }
    }

    // If at or above cache limit, try to uncache unneeded first.
    // A model is only released when TryReleaseCachedModel decides the game no longer depends on it,
    // and its map entry is kept so its history survives for the next demand.
    bool bReleasedModel = false;
    auto AttemptReleaseFromList = [&](const std::multimap<uint, ushort>& candidateList, const char* contextTag) -> bool
    {
        for (auto it = candidateList.rbegin(); it != candidateList.rend(); ++it)
        {
            const ushort modelId = it->second;
            auto         cacheIt = currentCacheInfoMap.find(modelId);
            if (cacheIt == currentCacheInfoMap.end())
                continue;

            SModelCacheInfo& candidateInfo = cacheIt->second;
            assert(candidateInfo.bIsModelCachedHere);

            if (TryReleaseCachedModel(modelId, candidateInfo, contextTag, uiNumModelsCachedHereOnly))
                return true;
        }
        return false;
    };

    if (uiNumModelsCachedHereOnly >= uiMaxCachedAllowed && !maybeUncacheUnneededList.empty())
        bReleasedModel = AttemptReleaseFromList(maybeUncacheUnneededList, "UncacheUnneeded");

    // Only uncache from the needed list if above limit, furthest away first
    if (!bReleasedModel && uiNumModelsCachedHereOnly > uiMaxCachedAllowed && !maybeUncacheNeededList.empty())
        bReleasedModel = AttemptReleaseFromList(maybeUncacheNeededList, "UncacheNeeded");

    // Cache if room
    if (!maybeCacheList.empty() && uiNumModelsCachedHereOnly < uiMaxCachedAllowed)
    {
        // Cache one which has been waiting the longest
        const ushort     usModelId = maybeCacheList.rbegin()->second;
        SModelCacheInfo* pInfo = MapFind(currentCacheInfoMap, usModelId);
        assert(pInfo);
        assert(!pInfo->bIsModelCachedHere);
        AddModelRefCount(usModelId);
        pInfo->bIsModelCachedHere = true;
        OutputDebugLine(SString("[Cache] Start caching model %d", usModelId));
    }
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::GetModelRefCount
//
///////////////////////////////////////////////////////////////
int CModelCacheManagerImpl::GetModelRefCount(ushort usModelId)
{
    CModelInfo* pModelInfo = m_pGame->GetModelInfo(usModelId);
    if (pModelInfo)
        return pModelInfo->GetRefCount();
    return 0;
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::AddModelRefCount
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::AddModelRefCount(ushort usModelId)
{
    CModelInfo* pModelInfo = m_pGame->GetModelInfo(usModelId);
    if (pModelInfo)
        pModelInfo->ModelAddRef(NON_BLOCKING, "cache");
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::SubModelRefCount
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::SubModelRefCount(ushort usModelId)
{
    CModelInfo* pModelInfo = m_pGame->GetModelInfo(usModelId);
    if (pModelInfo)
        pModelInfo->RemoveRef();
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::UnloadModel
//
// Remove model and associated txd from memory
//
///////////////////////////////////////////////////////////////
bool CModelCacheManagerImpl::UnloadModel(ushort usModelId)
{
    // Stream out usages in the client module
    CClientBase* pClientBase = CModManager::GetSingleton().GetClient();
    if (pClientBase)
        pClientBase->RestreamModel(usModelId);

    // Uncache usages here
    OnRestreamModel(usModelId);

    // Ensure model and txd are removed
    CModelInfo* pModelInfo = m_pGame->GetModelInfo(usModelId);
    if (pModelInfo)
        return pModelInfo->ForceUnload();

    return false;
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::OnRestreamModel
//
// Uncache here, now.
//
///////////////////////////////////////////////////////////////
void CModelCacheManagerImpl::OnRestreamModel(ushort usModelId)
{
    // Keep forced restream untouched: callers expect full removal when restreaming.
    std::map<ushort, SModelCacheInfo>* mapList[] = {&m_PedModelCacheInfoMap, &m_VehicleModelCacheInfoMap};

    for (uint i = 0; i < NUMELMS(mapList); i++)
    {
        std::map<ushort, SModelCacheInfo>& cacheInfoMap = *mapList[i];

        SModelCacheInfo* pInfo = MapFind(cacheInfoMap, usModelId);
        if (pInfo)
        {
            if (pInfo->bIsModelCachedHere)
            {
                SubModelRefCount(usModelId);
                pInfo->bIsModelCachedHere = false;
                MapRemove(cacheInfoMap, usModelId);
                OutputDebugLine(SString("[Cache] End caching model %d  (OnRestreamModel)", usModelId));
            }
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CModelCacheManagerImpl::TryReleaseCachedModel
//
// Drop our cache ref on a model, unless the game still depends on it.
// Only called when the cache is at or above its limit.
//
///////////////////////////////////////////////////////////////
bool CModelCacheManagerImpl::TryReleaseCachedModel(ushort usModelId, SModelCacheInfo& info, const char* contextTag, uint& uiNumModelsCachedHereOnly)
{
    assert(info.bIsModelCachedHere);

    CStreaming* pStreaming = m_pGame->GetStreaming();
    if (!pStreaming)
        return false;

    CStreamingInfo* pStreamingInfo = pStreaming->GetStreamingInfo(usModelId);
    if (!pStreamingInfo)
        return false;

    // The game marks models that it must keep loaded for missions or its own systems.
    // Dropping our ref on those would only cause the game to stream them right back in.
    if (pStreamingInfo->flg & (STREAMING_FLAG_MISSION_REQUIRED | STREAMING_FLAG_GAME_REQUIRED))
        return false;

    // Someone else still holds a ref, so our release would not free the model anyway.
    if (GetModelRefCount(usModelId) > 1)
        return false;

    CModelInfo*          pModelInfo = m_pGame->GetModelInfo(usModelId, true);
    const eModelInfoType modelType = pModelInfo ? pModelInfo->GetModelType() : eModelInfoType::UNKNOWN;

    // If the game still tracks this model in its loaded ped group, keep it cached while
    // the group is near empty, so we don't fight the game over which models stay loaded.
    if (modelType == eModelInfoType::PED && pStreaming->IsModelInLoadedPedGroup(usModelId))
    {
        if (pStreaming->GetNumPedsLoaded() <= PED_STREAMING_FLOOR)
            return false;
    }

    // Same for the loaded vehicle group, with a smaller floor inside interiors.
    // CWorld::GetCurrentArea reads the same CGame::currArea that the game's own
    // vehicle floor logic checks, so one lookup covers both cases.
    if (modelType == eModelInfoType::VEHICLE && pStreaming->IsModelInLoadedVehicleGroup(usModelId))
    {
        const uint vehicleCount = pStreaming->GetNumLoadedVehicles();
        const bool bIsInterior = m_pGame->GetWorld() && m_pGame->GetWorld()->GetCurrentArea() != 0;
        const uint minVehicleBudget = bIsInterior ? VEHICLE_STREAMING_FLOOR_INTERIOR : VEHICLE_STREAMING_FLOOR_EXTERIOR;
        if (vehicleCount <= minVehicleBudget)
            return false;
    }

    SubModelRefCount(usModelId);
    info.bIsModelCachedHere = false;
    info.lastNeeded = m_TickCountNow;
    // Also reset the first-needed time. Otherwise a still-needed model would keep its old
    // firstNeeded and win the recache list on the next pulse, causing a release/cache cycle.
    info.firstNeeded = m_TickCountNow;
    info.bIsModelLoadedByGame = false;

    if (uiNumModelsCachedHereOnly > 0)
        uiNumModelsCachedHereOnly--;

    OutputDebugLine(SString("[Cache] End caching model %d  (%s)", usModelId, contextTag));

    return true;
}
