/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

struct SModelCacheStats;

class CModelCacheManager
{
public:
    virtual ~CModelCacheManager() {}

    // CModelCacheManager interface
    virtual void DoPulse() = 0;
    virtual void GetStats(SModelCacheStats& outStats) = 0;
    virtual bool UnloadModel(uint32 usModelId) = 0;
    virtual void OnRestreamModel(uint32 usModelId) = 0;
    virtual void OnClientClose() = 0;
    virtual void UpdatePedModelCaching(const std::map<uint32, float>& newNeedCacheList) = 0;
    virtual void UpdateVehicleModelCaching(const std::map<uint32, float>& newNeedCacheList) = 0;
    virtual void AddModelToPersistentCache(uint32 usModelId) = 0;
};

CModelCacheManager* NewModelCacheManager();
