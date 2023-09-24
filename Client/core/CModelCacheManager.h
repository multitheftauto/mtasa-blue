/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <optional>

struct SModelCacheStats;

class CModelCacheManager
{
public:
    virtual ~CModelCacheManager() {}

    // CModelCacheManager interface
    virtual void DoPulse() = 0;
    virtual void GetStats(SModelCacheStats& outStats) = 0;
    virtual bool UnloadModel(ushort usModelId) = 0;
    virtual void OnRestreamModel(ushort usModelId) = 0;
    virtual void OnClientClose() = 0;
    virtual void UpdatePedModelCaching(const std::map<ushort, float>& newNeedCacheList) = 0;
    virtual void UpdateVehicleModelCaching(const std::map<ushort, float>& newNeedCacheList) = 0;
    virtual void AddModelToPersistentCache(ushort usModelId) = 0;
    virtual void SetCustomLimits(std::optional<size_t> numVehicles, std::optional<size_t> numPeds) = 0;
};

CModelCacheManager* NewModelCacheManager();
