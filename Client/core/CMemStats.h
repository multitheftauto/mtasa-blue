/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CMemStats.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

struct SModelCacheStats
{
    uint uiMaxNumPedModels;
    uint uiMaxNumVehicleModels;
    uint uiNumPedModels;
    uint uiNumVehicleModels;
};

//
// Memory state used in CMemStatsInterface
//
struct SMemStatsInfo
{
    ZERO_ON_NEW

    CProxyDirect3DDevice9::SMemoryState d3dMemory;
    CProxyDirect3DDevice9::SFrameStats  frameStats;
    SDxStatus                           dxStatus;
    size_t                              iProcessMemSizeKB;
    size_t                              iProcessTotalVirtualKB;
    size_t                              iStreamingMemoryUsed;
    size_t                              iStreamingMemoryAvailable;
    SRwResourceStats                    rwResourceStats;
    SClothesCacheStats                  clothesCacheStats;
    SShaderReplacementStats             shaderReplacementStats;
    SModelCacheStats                    modelCacheStats;

    union
    {
        uint uiArray[14];
        struct
        {
            uint uiPlayerModels_0_312;
            uint uiUnknown_313_317;
            uint uiWeaponModels_318_372;
            uint uiUnknown_373_399;
            uint uiVehicles_400_611;
            uint uiUnknown_612_999;
            uint uiUpgrades_1000_1193;
            uint uiUnknown_1194_19999;
            uint uiTextures_20000_24999;
            uint uiCollisions_25000_25254;
            uint uiIpls_25255_25510;
            uint uiPaths_25511_25574;
            uint uiAnims_25575_25754;
            uint uiTotal;
        };
    } modelInfo;
};

//
// CMemStatsInterface for measuring and displaying memory usage info
//
class CMemStatsInterface
{
public:
    virtual ~CMemStatsInterface() {}

    virtual void Draw() = 0;
    virtual void SetEnabled(bool bEnabled) = 0;
    virtual bool IsEnabled() = 0;
    virtual void SampleState(SMemStatsInfo& memStatsInfo) = 0;
};

CMemStatsInterface* GetMemStats();
