/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.cpp
 *  PURPOSE:     RenderWare mapping to Grand Theft Auto: San Andreas
 *               and miscellaneous rendering functions
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#include "StdInc.h"
#include <array>
#include <cstddef>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <CMatrix.h>
#include <core/CCoreInterface.h>
#define RWFUNC_IMPLEMENT
#include <game/RenderWareD3D.h>
#include "CColModelSA.h"
#include "CFileLoaderSA.h"
#include "CGameSA.h"
#include "CRenderWareSA.h"
#include "CRenderWareSA.ShaderMatching.h"
#include "gamesa_renderware.h"
#include "gamesa_renderware.hpp"

extern CCoreInterface* g_pCore;
extern CGameSA*        pGame;

void CRenderWareSA::DebugTxdAddRef(unsigned short usTxdId, const char* /*tag*/, bool /*enableSafetyPin*/)
{
    if (!pGame || !pGame->GetRenderWareSA())
        return;
    CTxdStore_AddRef(usTxdId);
}

void CRenderWareSA::DebugTxdRemoveRef(unsigned short usTxdId, const char* /*tag*/)
{
    if (!pGame || !pGame->GetRenderWareSA())
        return;
    CTxdStore_RemoveRef(usTxdId);
}

// RwFrameForAllObjects struct and callback used to replace dynamic vehicle parts
struct SReplaceParts
{
    const char*        szName;                    // name of the part you want to replace (e.g. 'door_lf' or 'door_rf')
    unsigned char      ucIndex;                   // index counter for internal usage (0 is the 'ok' part model, 1 is the 'dam' part model)
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static RwObject* ReplacePartsCB(RwObject* object, SReplaceParts* data)
{
    if (!object) [[unlikely]]
        return object;
    
    RpAtomic* Atomic = (RpAtomic*)object;
    char      szAtomicName[16] = {0};

    // iterate through our loaded atomics
    for (unsigned int i = 0; i < data->uiReplacements; i++)
    {
        // get the correct atomic name based on the object # in our parent frame
        if (data->ucIndex == 0)
            snprintf(&szAtomicName[0], 16, "%s_ok", data->szName);
        else
            snprintf(&szAtomicName[0], 16, "%s_dam", data->szName);

        // see if we have such an atomic in our replacement atomics array
        if (strncmp(&data->pReplacements[i].szName[0], &szAtomicName[0], 16) == 0)
        {
            // if so, override the geometry
            RpAtomicSetGeometry(Atomic, data->pReplacements[i].atomic->geometry, 0);

            // and copy the matrices
            RwFrameCopyMatrix(RpGetFrame(Atomic), RpGetFrame(data->pReplacements[i].atomic));

            object = (RwObject*)data->pReplacements[i].atomic;
            data->ucIndex++;
        }
    }

    return object;
}

// RpClumpForAllAtomics callback used to add atomics to a vehicle
static bool AddAllAtomicsCB(RpAtomic* atomic, void* pClump)
{
    if (!atomic || !pClump) [[unlikely]]
        return false;
    
    RpClump* data = reinterpret_cast<RpClump*>(pClump);
    RwFrame* pFrame = RpGetFrame(data);

    // add the atomic to the frame
    RpAtomicSetFrame(atomic, pFrame);
    RpClumpAddAtomic(data, atomic);

    return true;
}

// RpClumpForAllAtomics struct and callback used to replace all wheels with a given wheel model
struct SReplaceWheels
{
    const char*        szName;                    // name of the new wheel model
    RpClump*           pClump;                    // the vehicle's clump
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static bool ReplaceWheelsCB(RpAtomic* atomic, void* pData)
{
    if (!atomic || !pData) [[unlikely]]
        return false;
    
    SReplaceWheels* data = reinterpret_cast<SReplaceWheels*>(pData);
    RwFrame*        Frame = RpGetFrame(atomic);

    // find our wheel atomics
    if (strncmp(&Frame->szName[0], "wheel_lf_dummy", 16) == 0 || strncmp(&Frame->szName[0], "wheel_rf_dummy", 16) == 0 ||
        strncmp(&Frame->szName[0], "wheel_lm_dummy", 16) == 0 || strncmp(&Frame->szName[0], "wheel_rm_dummy", 16) == 0 ||
        strncmp(&Frame->szName[0], "wheel_lb_dummy", 16) == 0 || strncmp(&Frame->szName[0], "wheel_rb_dummy", 16) == 0)
    {
        // find a replacement atomic
        for (unsigned int i = 0; i < data->uiReplacements; i++)
        {
            // see if it's name is equal to the specified wheel
            if (strncmp(&data->pReplacements[i].szName[0], data->szName, 16) == 0)
            {
                // clone our wheel atomic
                RpAtomic* WheelAtomic = RpAtomicClone(data->pReplacements[i].atomic);
                RpAtomicSetFrame(WheelAtomic, Frame);

                // add it to the clump
                RpClumpAddAtomic(data->pClump, WheelAtomic);

                // delete the current atomic
                RpClumpRemoveAtomic(data->pClump, atomic);
                RpAtomicDestroy(atomic);  // Destroy removed atomic to prevent leak
            }
        }
    }

    return true;
}

// RpClumpForAllAtomics struct and callback used to replace all atomics for a vehicle
struct SReplaceAll
{
    RpClump*           pClump;                    // the vehicle's clump
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static bool ReplaceAllCB(RpAtomic* atomic, void* pData)
{
    if (!atomic || !pData) [[unlikely]]
        return false;
    
    SReplaceAll* data = reinterpret_cast<SReplaceAll*>(pData);
    RwFrame*     Frame = RpGetFrame(atomic);
    if (Frame == NULL)
        return true;

    // find a replacement atomic
    for (unsigned int i = 0; i < data->uiReplacements; i++)
    {
        // see if we can find an atomic with the same name
        if (strncmp(&data->pReplacements[i].szName[0], &Frame->szName[0], 16) == 0)
        {
            // copy the matrices
            RwFrameCopyMatrix(RpGetFrame(atomic), RpGetFrame(data->pReplacements[i].atomic));

            // set the new atomic's frame to the current one
            RpAtomicSetFrame(data->pReplacements[i].atomic, Frame);

            // override all engine and material related callbacks and pointers
            data->pReplacements[i].atomic->renderCallback = atomic->renderCallback;
            data->pReplacements[i].atomic->frame = atomic->frame;
            data->pReplacements[i].atomic->render = atomic->render;
            data->pReplacements[i].atomic->interpolator = atomic->interpolator;
            data->pReplacements[i].atomic->info = atomic->info;

            // add the new atomic to the vehicle clump
            RpClumpAddAtomic(data->pClump, data->pReplacements[i].atomic);

            // remove the current atomic
            RpClumpRemoveAtomic(data->pClump, atomic);
            RpAtomicDestroy(atomic);  // Destroy removed atomic to prevent leak
        }
    }

    return true;
}

// Load atomics from clump into container (caller manages lifetime)
struct SLoadAtomics
{
    RpAtomicContainer* pReplacements;             // replacement atomics
    unsigned int       uiReplacements;            // number of replacements
};
static bool LoadAtomicsCB(RpAtomic* atomic, void* pData)
{
    if (!atomic || !pData) [[unlikely]]
        return false;
    
    SLoadAtomics* data = reinterpret_cast<SLoadAtomics*>(pData);
    RwFrame*      Frame = RpGetFrame(atomic);

    if (!Frame) [[unlikely]]
        return false;

    // Add atomic to container
    data->pReplacements[data->uiReplacements].atomic = atomic;
    strncpy(&data->pReplacements[data->uiReplacements].szName[0], &Frame->szName[0], 16);
    data->pReplacements[data->uiReplacements].szName[16] = '\0';

    // and increment the counter
    data->uiReplacements++;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CRenderWareSA
//
//
/////////////////////////////////////////////////////////////////////////////
CRenderWareSA::CRenderWareSA()
{
    InitRwFunctions();

    InitTextureWatchHooks();
    m_pMatchChannelManager = new CMatchChannelManager();
    m_iRenderingEntityType = TYPE_MASK_WORLD;
    m_GTAVertexShadersDisabledTimer.SetMaxIncrement(1000, true);
    m_bGTAVertexShadersEnabled = true;
}

CRenderWareSA::~CRenderWareSA()
{
    SAFE_DELETE(m_pMatchChannelManager);
}

// Reads and parses a TXD file specified by a path (szTXD)
RwTexDictionary* CRenderWareSA::ReadTXD(const SString& strFilename, const SString& buffer)
{
    constexpr std::size_t   RW_CHUNK_HEADER_SIZE = 12;
    constexpr std::uint32_t RW_CHUNK_TYPE_TXD = 0x16;
    constexpr std::uint32_t MAX_SANE_CHUNK_SIZE = 512 * 1024 * 1024;

    if (buffer.empty() && strFilename.empty())
        return nullptr;

    const bool bUsingBuffer = !buffer.empty();

    std::uint32_t chunkType = 0;
    std::uint32_t chunkSize = 0;

    if (!bUsingBuffer)
    {
        SString headerData;
        if (!FileLoad(std::nothrow, strFilename, headerData, RW_CHUNK_HEADER_SIZE))
            return nullptr;

        if (headerData.size() < RW_CHUNK_HEADER_SIZE)
            return nullptr;

        std::memcpy(&chunkType, headerData.data(), sizeof(chunkType));
        std::memcpy(&chunkSize, headerData.data() + 4, sizeof(chunkSize));

        if (chunkType != RW_CHUNK_TYPE_TXD)
            return nullptr;

        if (chunkSize > MAX_SANE_CHUNK_SIZE)
            return nullptr;

        std::uint64_t fileSize = FileSize(strFilename);
        if (fileSize < RW_CHUNK_HEADER_SIZE + chunkSize)
            return nullptr;
    }
    else
    {
        if (buffer.size() < RW_CHUNK_HEADER_SIZE)
            return nullptr;

        std::memcpy(&chunkType, buffer.data(), sizeof(chunkType));
        std::memcpy(&chunkSize, buffer.data() + 4, sizeof(chunkSize));

        if (chunkType != RW_CHUNK_TYPE_TXD)
            return nullptr;

        if (chunkSize > MAX_SANE_CHUNK_SIZE)
            return nullptr;

        if (buffer.size() < RW_CHUNK_HEADER_SIZE + chunkSize)
            return nullptr;
    }

    RwStream* pStream = nullptr;
    RwBuffer  rwBuffer{};

    if (bUsingBuffer)
    {
        rwBuffer.ptr = const_cast<char*>(buffer.data());
        rwBuffer.size = static_cast<unsigned int>(buffer.size());
        pStream = RwStreamOpen(STREAM_TYPE_BUFFER, STREAM_MODE_READ, &rwBuffer);
    }
    else
    {
        pStream = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_READ, *strFilename);
    }

    if (!pStream)
        return nullptr;

    if (!RwStreamFindChunk(pStream, RW_CHUNK_TYPE_TXD, nullptr, nullptr))
    {
        RwStreamClose(pStream, nullptr);
        return nullptr;
    }

    RwTexDictionary* pTxd = RwTexDictionaryGtaStreamRead(pStream);
    RwStreamClose(pStream, nullptr);

    if (!pTxd)
        return nullptr;

    ScriptAddedTxd(pTxd);
    return pTxd;
}

// Reads and parses a DFF file specified by a path (szDFF) into a CModelInfo identified by the object id (usModelID)
// bLoadEmbeddedCollisions should be true for vehicles
// Any custom TXD should be imported before this call
RpClump* CRenderWareSA::ReadDFF(const SString& strFilename, const SString& buffer, unsigned short usModelID, bool bLoadEmbeddedCollisions)
{
    if (usModelID != 0)
    {
        CBaseModelInfoSAInterface* pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID];
        if (pModelInfo)
        {
            unsigned short usTxdId = pModelInfo->usTextureDictionary;
            SetTextureDict(usTxdId);
        }
    }

    constexpr std::size_t   RW_CHUNK_HEADER_SIZE = 12;
    constexpr std::uint32_t RW_CHUNK_TYPE_DFF = 0x10;
    constexpr std::uint32_t MAX_SANE_CHUNK_SIZE = 512 * 1024 * 1024;

    const bool bUsingBuffer = !buffer.empty();

    std::uint32_t chunkType = 0;
    std::uint32_t chunkSize = 0;

    if (!bUsingBuffer)
    {
        if (strFilename.empty())
            return nullptr;

        SString headerData;
        if (!FileLoad(std::nothrow, strFilename, headerData, RW_CHUNK_HEADER_SIZE))
            return nullptr;

        if (headerData.size() < RW_CHUNK_HEADER_SIZE)
            return nullptr;

        std::memcpy(&chunkType, headerData.data(), sizeof(chunkType));
        std::memcpy(&chunkSize, headerData.data() + 4, sizeof(chunkSize));

        if (chunkType != RW_CHUNK_TYPE_DFF)
            return nullptr;

        if (chunkSize > MAX_SANE_CHUNK_SIZE)
            return nullptr;

        std::uint64_t fileSize = FileSize(strFilename);
        if (fileSize < RW_CHUNK_HEADER_SIZE + chunkSize)
            return nullptr;
    }
    else
    {
        if (buffer.size() < RW_CHUNK_HEADER_SIZE)
            return nullptr;

        std::memcpy(&chunkType, buffer.data(), sizeof(chunkType));
        std::memcpy(&chunkSize, buffer.data() + 4, sizeof(chunkSize));

        if (chunkType != RW_CHUNK_TYPE_DFF)
            return nullptr;

        if (chunkSize > MAX_SANE_CHUNK_SIZE)
            return nullptr;

        if (buffer.size() < RW_CHUNK_HEADER_SIZE + chunkSize)
            return nullptr;
    }

    RwStream* pStream = nullptr;
    RwBuffer  rwBuffer{};

    if (bUsingBuffer)
    {
        rwBuffer.ptr = const_cast<char*>(buffer.data());
        rwBuffer.size = static_cast<unsigned int>(buffer.size());
        pStream = RwStreamOpen(STREAM_TYPE_BUFFER, STREAM_MODE_READ, &rwBuffer);
    }
    else
    {
        pStream = RwStreamOpen(STREAM_TYPE_FILENAME, STREAM_MODE_READ, *strFilename);
    }

    DWORD* pPool = (DWORD*)ARRAY_ModelInfo;

    if (!pStream)
        return nullptr;

    if (!RwStreamFindChunk(pStream, RW_CHUNK_TYPE_DFF, nullptr, nullptr))
    {
        RwStreamClose(pStream, nullptr);
        return nullptr;
    }

    if (bLoadEmbeddedCollisions)
    {
        // Vehicles have their collision loaded through the CollisionModel plugin, so we need to remove the current collision to prevent a memory leak.
        // This needs to be done here before reading the stream data, because plugins are read in RpClumpStreamRead.
        CModelInfo* modelInfo = pGame->GetModelInfo(usModelID);
        if (modelInfo)
        {
            if (auto* modelInfoInterface = modelInfo->GetInterface())
                ((void(__thiscall*)(CBaseModelInfoSAInterface*))0x4C4C40)(modelInfoInterface); // CBaseModelInfo::DeleteCollisionModel
        }

        // rockstar's collision hack
        // It sets the pointer CCollisionPlugin::ms_currentModel to the model info of the given vehicle in order to correctly set up the vehicle’s
        // collision during collision plugin reading (0x41B2BD).
        RpPrtStdGlobalDataSetStreamEmbedded((void*)pPool[usModelID]);

        // Call CVehicleModelInfo::UseCommonVehicleTexDicationary
        ((void(__cdecl*)())0x4C75A0)();
    }

    // read the clump with all its extensions
    RpClump* pClump = RpClumpStreamRead(pStream);

    if (bLoadEmbeddedCollisions)
    {
        // reset collision hack
        RpPrtStdGlobalDataSetStreamEmbedded(nullptr);

        // Call CVehicleModelInfo::StopUsingCommonVehicleTexDicationary
        ((void(__cdecl*)())0x4C75C0)();
    }

    RwStreamClose(pStream, nullptr);

    return pClump;
}

//
// Returns list of atomics inside a clump
// Uses a visited set to detect cycles in corrupted linked lists
//
void CRenderWareSA::GetClumpAtomicList(RpClump* pClump, std::vector<RpAtomic*>& outAtomicList)
{
    // Track visited atomics to detect cycles from corrupted linked lists.
    struct VisitedTracker
    {
        std::vector<RpAtomic*>* pList;
        std::unordered_set<void*> visited;
        std::size_t iterations;
        bool bStop;
        std::size_t maxAtomics;
        
        VisitedTracker(std::vector<RpAtomic*>* list, std::size_t maxCount)
            : pList(list), iterations(0), bStop(false), maxAtomics(maxCount)
        {
        }
    };

    auto ForAllAtomicsCB = [](RpAtomic* pAtomic, void* pData) -> bool {
        auto* t = reinterpret_cast<VisitedTracker*>(pData);

        if (t->bStop)
            return false;  // Stop iteration once we decided to bail

        if (++t->iterations > t->maxAtomics)
        {
            t->bStop = true;
            return false;
        }

        // Check if we've seen this atomic before (cycle detection)
        if (t->visited.find(pAtomic) != t->visited.end())
        {
            // Skip duplicates but keep iterating to avoid truncating legitshared atomics
            return true;
        }

        t->visited.insert(pAtomic);

        if (pAtomic && SharedUtil::IsReadablePointer(pAtomic, sizeof(RpAtomic)))
            t->pList->push_back(pAtomic);

        return true;
    };

    VisitedTracker tracker(&outAtomicList, 4096);

    RpClumpForAllAtomics(pClump, ForAllAtomicsCB, &tracker);
}

//
// Returns true if the clump geometry sort of matches
//
// ClumpA vs ClumpB(or)AtomicB
//
bool CRenderWareSA::DoContainTheSameGeometry(RpClump* pClumpA, RpClump* pClumpB, RpAtomic* pAtomicB)
{
    // Fast check if comparing one atomic
    if (pAtomicB)
    {
        RpGeometry* pGeometryA = ((RpAtomic*)((pClumpA->atomics.root.next) - 0x8))->geometry;
        RpGeometry* pGeometryB = pAtomicB->geometry;
        return pGeometryA == pGeometryB;
    }

    // Get atomic list from both sides
    std::vector<RpAtomic*> atomicListA;
    std::vector<RpAtomic*> atomicListB;
    GetClumpAtomicList(pClumpA, atomicListA);
    if (pClumpB)
        GetClumpAtomicList(pClumpB, atomicListB);
    if (pAtomicB)
        atomicListB.push_back(pAtomicB);

    // Count geometries that exist in both sides
    std::set<RpGeometry*> geometryListA;
    for (uint i = 0; i < atomicListA.size(); i++)
        MapInsert(geometryListA, atomicListA[i]->geometry);

    uint uiInBoth = 0;
    for (uint i = 0; i < atomicListB.size(); i++)
        if (MapContains(geometryListA, atomicListB[i]->geometry))
            uiInBoth++;

    // If less than 50% match then assume it is not the same
    if (uiInBoth * 2 < atomicListB.size() || atomicListB.size() == 0)
        return false;

    return true;
}

// Helpers for rebinding model textures to a different TXD.
// When a model's TXD slot is changed, its loaded RwObject still holds pointers
// to textures from the old TXD. These helpers update material texture pointers
// to reference matching textures from the new TXD by name lookup.
namespace
{
    // Hash functor for texture name lookup (bounded to RW_TEXTURE_NAME_LENGTH)
    struct TxdTextureNameHash
    {
        static constexpr std::size_t HASH_INIT = 2166136261u;
        static constexpr std::size_t HASH_MULTIPLIER = 16777619u;

        std::size_t operator()(const char* s) const noexcept
        {
            if (!s)
                return 0;
            std::size_t h = HASH_INIT;
            for (std::size_t i = 0; i < RW_TEXTURE_NAME_LENGTH && s[i]; ++i)
            {
                h ^= static_cast<unsigned char>(s[i]);
                h *= HASH_MULTIPLIER;
            }
            return h;
        }
    };

    struct TxdTextureNameEq
    {
        bool operator()(const char* a, const char* b) const noexcept
        {
            if (a == b)
                return true;
            if (!a || !b)
                return false;
            return strncmp(a, b, RW_TEXTURE_NAME_LENGTH) == 0;
        }
    };

    using TxdTextureMap = std::unordered_map<const char*, RwTexture*, TxdTextureNameHash, TxdTextureNameEq>;

    // Build name->texture map for a TXD slot. Keys point directly into RwTexture::name buffers.
    TxdTextureMap BuildTxdTextureMap(unsigned short usTxdId)
    {
        TxdTextureMap result;

        RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);
        if (!pTxd)
            return result;

        std::vector<RwTexture*> txdTextures;
        CRenderWareSA::GetTxdTextures(txdTextures, pTxd);

        if (txdTextures.empty())
            return result;

        result.reserve(txdTextures.size());
        for (RwTexture* pTexture : txdTextures)
        {
            if (!pTexture)
                continue;

            const char* name = pTexture->name;
            if (strnlen(name, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                result[name] = pTexture;
        }

        return result;
    }

    // Update each material's texture pointer to the matching texture from txdTextureMap.
    // Falls back to internal name mapping if direct lookup fails.
    void RebindAtomicMaterials(RpAtomic* pAtomic, const TxdTextureMap& txdTextureMap)
    {
        if (!pAtomic)
            return;

        RpGeometry* pGeometry = pAtomic->geometry;
        if (!pGeometry)
            return;

        RpMaterials& materials = pGeometry->materials;
        if (!materials.materials || materials.entries <= 0)
            return;

        constexpr int MAX_REASONABLE_MATERIALS = 10000;
        if (materials.entries > MAX_REASONABLE_MATERIALS)
            return;

        for (int idx = 0; idx < materials.entries; ++idx)
        {
            RpMaterial* pMaterial = materials.materials[idx];
            if (!pMaterial)
                continue;

            RwTexture* pOldTexture = pMaterial->texture;
            if (!pOldTexture)
                continue;

            const char* szTextureName = pOldTexture->name;
            if (!szTextureName[0])
                continue;

            if (strnlen(szTextureName, RW_TEXTURE_NAME_LENGTH) >= RW_TEXTURE_NAME_LENGTH)
                continue;

            RwTexture* pNewTexture = nullptr;
            auto itFound = txdTextureMap.find(szTextureName);
            if (itFound != txdTextureMap.end())
                pNewTexture = itFound->second;

            if (!pNewTexture)
            {
                const char* szInternalName = CRenderWareSA::GetInternalTextureName(szTextureName);
                if (szInternalName && szInternalName != szTextureName)
                {
                    if (strnlen(szInternalName, RW_TEXTURE_NAME_LENGTH) < RW_TEXTURE_NAME_LENGTH)
                    {
                        auto itInternal = txdTextureMap.find(szInternalName);
                        if (itInternal != txdTextureMap.end())
                            pNewTexture = itInternal->second;
                    }
                }
            }

            if (pNewTexture && pNewTexture != pOldTexture)
                RpMaterialSetTexture(pMaterial, pNewTexture);
        }
    }
}

void CRenderWareSA::RebindClumpTexturesToTxd(RpClump* pClump, unsigned short usTxdId)
{
    if (!pClump)
        return;

    TxdTextureMap txdTextureMap = BuildTxdTextureMap(usTxdId);
    if (txdTextureMap.empty())
        return;

    std::vector<RpAtomic*> atomicList;
    GetClumpAtomicList(pClump, atomicList);

    for (RpAtomic* pAtomic : atomicList)
        RebindAtomicMaterials(pAtomic, txdTextureMap);
}

void CRenderWareSA::RebindAtomicTexturesToTxd(RpAtomic* pAtomic, unsigned short usTxdId)
{
    if (!pAtomic || !pAtomic->geometry)
        return;

    TxdTextureMap txdTextureMap = BuildTxdTextureMap(usTxdId);
    if (txdTextureMap.empty())
        return;

    RebindAtomicMaterials(pAtomic, txdTextureMap);
}

// Replaces a vehicle/weapon/ped model
bool CRenderWareSA::ReplaceModel(RpClump* pNew, unsigned short usModelID, DWORD dwSetClumpFunction)
{
    auto CVehicleModelInfo_CVehicleStructure_Destructor = (void(__thiscall*)(CVehicleModelVisualInfoSAInterface * pThis))0x4C7410;
    auto CVehicleModelInfo_CVehicleStructure_release = (void(__cdecl*)(CVehicleModelVisualInfoSAInterface * pThis))0x4C9580;
    auto CBaseModelInfo_SetClump = (void(__thiscall*)(CBaseModelInfoSAInterface * pThis, RpClump * clump)) dwSetClumpFunction;

    CModelInfo* pModelInfo = pGame->GetModelInfo(usModelID);
    if (pModelInfo)
    {
        RpClump* pOldClump = (RpClump*)pModelInfo->GetRwObject();
        if (pOldClump != pNew && !DoContainTheSameGeometry(pNew, pOldClump, NULL))
        {
            const bool shouldResetUpgrades = (dwSetClumpFunction == FUNC_LoadVehicleModel);

            // Make new clump container for the model geometry
            // Clone twice as the geometry render order seems to be reversed each time it is cloned.
            RpClump* pTemp = RpClumpClone(pNew);
            RpClump* pNewClone = RpClumpClone(pTemp);
            RpClumpDestroy(pTemp);

            // Calling CVehicleModelInfo::SetClump() allocates a new CVehicleStructure.
            // So let's delete the old one first to avoid CPool<CVehicleStructure> depletion.
            if (dwSetClumpFunction == FUNC_LoadVehicleModel)
            {
                auto pVehicleModelInfoInterface = (CVehicleModelInfoSAInterface*)pModelInfo->GetInterface();
                if (pVehicleModelInfoInterface && pVehicleModelInfoInterface->pVisualInfo)
                {
                    auto pVisualInfo = pVehicleModelInfoInterface->pVisualInfo;
                    CVehicleModelInfo_CVehicleStructure_Destructor(pVisualInfo);
                    CVehicleModelInfo_CVehicleStructure_release(pVisualInfo);
                    pVehicleModelInfoInterface->pVisualInfo = nullptr;
                }
            }

            CBaseModelInfoSAInterface* pModelInfoInterface = pModelInfo->GetInterface();
            if (!pModelInfoInterface)
            {
                RpClumpDestroy(pNewClone);
                return false;
            }
            CBaseModelInfo_SetClump(pModelInfoInterface, pNewClone);

            CBaseModelInfoSAInterface* pInterfaceAfterSet = pModelInfoInterface;
            // Re-fetch interface pointer after SetClump (may relocate/change)
            pModelInfoInterface = pModelInfo->GetInterface();

            // Not happy
            if (!pModelInfoInterface)
            {
                CBaseModelInfoSAInterface* fallback = nullptr;
                auto* arrayEntry = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID];
                if (arrayEntry && SharedUtil::IsReadablePointer(arrayEntry, sizeof(*arrayEntry)))
                    fallback = arrayEntry;
                else if (pInterfaceAfterSet && SharedUtil::IsReadablePointer(pInterfaceAfterSet, sizeof(*pInterfaceAfterSet)))
                    fallback = pInterfaceAfterSet;
                bool restored = false;
                const bool oldClumpReadable = pOldClump && SharedUtil::IsReadablePointer(pOldClump, sizeof(*pOldClump));
                if (fallback && oldClumpReadable && SharedUtil::IsReadablePointer(fallback->VFTBL, sizeof(*fallback->VFTBL)))
                {
                    fallback->pRwObject = &pOldClump->object;
                    CBaseModelInfo_SetClump(fallback, pOldClump);
                    restored = true;
                }
                AddReportLog(8627, SString("ReplaceModel: lost interface after SetClump, model:%d restored:%d", usModelID, restored ? 1 : 0));
                RpClumpDestroy(pNewClone);
                return false;
            }

            if (shouldResetUpgrades)
                pModelInfo->ResetSupportedUpgrades();
            
            // Fix for custom DFF without embedded collision:
            // SetClump clears pColModel when DFF has no collision data, but vehicles need collision from .col pool.
            // Solution: Remove + Request + Load to restore pool-managed collision from data/vehicles.col
            if (dwSetClumpFunction == FUNC_LoadVehicleModel && !pModelInfoInterface->pColModel)
            {
                pGame->GetStreaming()->RemoveModel(usModelID);
                pGame->GetStreaming()->RequestModel(usModelID, 0x16);
                pGame->GetStreaming()->LoadAllRequestedModels(false, "CRenderWareSA::ReplaceVehicleModel");
                // Re-fetch interface pointer after model reload
                pModelInfoInterface = pModelInfo->GetInterface();
                if (!pModelInfoInterface)
                    return false;
            }
            
            RpClumpDestroy(pOldClump);
        }
    }

    return true;
}

// Replaces a vehicle model
bool CRenderWareSA::ReplaceVehicleModel(RpClump* pNew, unsigned short usModelID)
{
    return ReplaceModel(pNew, usModelID, FUNC_LoadVehicleModel);
}

// Replaces a weapon model
bool CRenderWareSA::ReplaceWeaponModel(RpClump* pNew, unsigned short usModelID)
{
    return ReplaceModel(pNew, usModelID, FUNC_LoadWeaponModel);
}

// Replaces a ped model
bool CRenderWareSA::ReplacePedModel(RpClump* pNew, unsigned short usModelID)
{
    // NOTE(botder): The game logic requires the animation hierarchy to be present (read: it's not a corrupt model),
    // otherwise it will crash (offset 0x3c51a8).
    if (!GetAnimHierarchyFromClump(pNew))
    {
        LogEvent(851, "Model not replaced", "CRenderWareSA::ReplacePedModel", SString("No anim hierarchy for ped model:%d", usModelID), 5421);
        return false;
    }

    return ReplaceModel(pNew, usModelID, FUNC_LoadPedModel);
}

// Reads and parses a COL file (versions 1-4: COLL, COL2, COL3, COL4)
CColModel* CRenderWareSA::ReadCOL(const SString& buffer)
{
    // Validate minimum buffer size
    if (buffer.size() < sizeof(ColModelFileHeader) + 16) [[unlikely]]
        return nullptr;

    const auto& header = *reinterpret_cast<const ColModelFileHeader*>(buffer.data());

    // Validate version field contains valid COL magic number
    // Version is 4-char fixed string (not null-terminated): "COLL", "COL2", "COL3", "COL4"
    constexpr std::array<std::array<char, 4>, 4> validVersions = {{
        {'C', 'O', 'L', 'L'},
        {'C', 'O', 'L', '2'},
        {'C', 'O', 'L', '3'},
        {'C', 'O', 'L', '4'}
    }};
    
    const bool isValidVersion = std::any_of(validVersions.begin(), validVersions.end(),
        [&header](const auto& valid) { 
            return std::equal(valid.begin(), valid.end(), header.version);
        });

    if (!isValidVersion) [[unlikely]]
    {
        // Explicitly limit to 4 characters
        AddReportLog(8622, SString("ReadCOL: Invalid version '%c%c%c%c' - expected COLL, COL2, COL3, or COL4",
            header.version[0], header.version[1], header.version[2], header.version[3]));
        return nullptr;
    }

    // Ensure name field is null-terminated to prevent buffer overrun
    const auto* nameEnd = static_cast<const char*>(std::memchr(header.name, '\0', sizeof(header.name)));
    if (!nameEnd) [[unlikely]]
    {
        AddReportLog(8623, "ReadCOL: Name field not null-terminated, may be truncated");
        return nullptr;
    }

    // Buffer is not modified by us, but GTA's functions expect non-const
    auto* pModelData = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(buffer.data())) + sizeof(ColModelFileHeader);

    // Create a new CColModel
    auto* pColModel = new CColModelSA();

    // Load appropriate collision version
    switch (header.version[3])
    {
        case 'L':
            LoadCollisionModel(pModelData, pColModel->GetInterface(), nullptr);
            break;
        case '2':
            LoadCollisionModelVer2(pModelData, header.size - 0x18, pColModel->GetInterface(), nullptr);
            break;
        case '3':
            LoadCollisionModelVer3(pModelData, header.size - 0x18, pColModel->GetInterface(), nullptr);
            break;
        case '4':
            // COL4 format has same structure as COL3 with one extra uint32 field in header
            // Must use Ver4 loader for correct offset calculations
            LoadCollisionModelVer4(pModelData, header.size - 0x18, pColModel->GetInterface(), nullptr);
            break;
        default:
            // Should never reach here due to validation above
            delete pColModel;
            return nullptr;
    }

    return pColModel;
}

// Loads all atomics from a clump into a container struct and returns the number of atomics it loaded
unsigned int CRenderWareSA::LoadAtomics(RpClump* pClump, RpAtomicContainer* pAtomics)
{
    if (!pClump || !pAtomics) [[unlikely]]
        return 0;
    
    // iterate through all atomics in the clump
    SLoadAtomics data = {0};
    data.pReplacements = pAtomics;
    RpClumpForAllAtomics(pClump, LoadAtomicsCB, &data);

    // return the number of atomics that were added to the container
    return data.uiReplacements;
}

// Replaces all atomics for a specific model
typedef struct
{
    unsigned short usTxdID;
    RpClump*       pClump;
} SAtomicsReplacer;
bool AtomicsReplacer(RpAtomic* pAtomic, void* data)
{
    SAtomicsReplacer* pData = reinterpret_cast<SAtomicsReplacer*>(data);
    SRelatedModelInfo relatedModelInfo = {0};
    relatedModelInfo.pClump = pData->pClump;
    relatedModelInfo.bDeleteOldRwObject = true;
    CFileLoader_SetRelatedModelInfoCB(pAtomic, &relatedModelInfo);

    // The above function adds a reference to the model's TXD. Remove it again.
    CRenderWareSA::DebugTxdRemoveRef(pData->usTxdID);
    return true;
}

bool CRenderWareSA::ReplaceAllAtomicsInModel(RpClump* pNew, unsigned short usModelID)
{
    if (!pNew) [[unlikely]]
        return false;
    
    CModelInfo* pModelInfo = pGame->GetModelInfo(usModelID);

    if (pModelInfo)
    {
        RpAtomic* pOldAtomic = (RpAtomic*)pModelInfo->GetRwObject();

        if (reinterpret_cast<RpClump*>(pOldAtomic) != pNew && !DoContainTheSameGeometry(pNew, NULL, pOldAtomic))
        {
            // Clone the clump that's to be replaced (FUNC_AtomicsReplacer removes the atomics from the source clump)
            RpClump* pCopy = RpClumpClone(pNew);
            if (!pCopy) [[unlikely]]
            {
                AddReportLog(8624, SString("ReplaceAllAtomicsInModel: RpClumpClone failed for model %d", usModelID));
                return false;
            }

            // Replace the atomics
            SAtomicsReplacer data;
            CBaseModelInfoSAInterface* pModelInfoInterface = pModelInfo->GetInterface();
            if (!pModelInfoInterface)
            {
                RpClumpDestroy(pCopy);
                return false;
            }
            data.usTxdID = pModelInfoInterface->usTextureDictionary;
            data.pClump = pCopy;

            MemPutFast<DWORD>((DWORD*)DWORD_AtomicsReplacerModelID, usModelID);
            RpClumpForAllAtomics(pCopy, AtomicsReplacer, &data);

            // Get rid of the now empty copied clump
            RpClumpDestroy(pCopy);
        }
    }

    return true;
}

// Replaces all atomics in a vehicle
void CRenderWareSA::ReplaceAllAtomicsInClump(RpClump* pDst, RpAtomicContainer* pAtomics, unsigned int uiAtomics)
{
    SReplaceAll data;
    data.pClump = pDst;
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    RpClumpForAllAtomics(pDst, ReplaceAllCB, &data);
}

// Replaces the wheels in a vehicle
void CRenderWareSA::ReplaceWheels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szWheel)
{
    // get the clump's frame
    RwFrame* pFrame = RpGetFrame(pClump);

    SReplaceWheels data;
    data.pClump = pClump;
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    data.szName = szWheel;
    RpClumpForAllAtomics(pClump, ReplaceWheelsCB, &data);
}

// Repositions an atomic
void CRenderWareSA::RepositionAtomic(RpClump* pDst, RpClump* pSrc, const char* szName)
{
    RwFrame* pDstFrame = RpGetFrame(pDst);
    RwFrame* pSrcFrame = RpGetFrame(pSrc);
    RwFrameCopyMatrix(RwFrameFindFrame(pDstFrame, szName), RwFrameFindFrame(pSrcFrame, szName));
}

// Adds the atomics from a source clump (pSrc) to a destination clump (pDst)
void CRenderWareSA::AddAllAtomics(RpClump* pDst, RpClump* pSrc)
{
    RpClumpForAllAtomics(pSrc, AddAllAtomicsCB, pDst);
}

// Replaces dynamic parts of the vehicle (models that have two different versions: 'ok' and 'dam'), such as doors
// szName should be without the part suffix (e.g. 'door_lf' or 'door_rf', and not 'door_lf_dummy')
bool CRenderWareSA::ReplacePartModels(RpClump* pClump, RpAtomicContainer* pAtomics, unsigned int uiAtomics, const char* szName)
{
    // get the part's dummy name
    char szDummyName[16] = {0};
    snprintf(&szDummyName[0], 16, "%s_dummy", szName);

    // get the clump's frame
    RwFrame* pFrame = RpGetFrame(pClump);

    // get the part's dummy frame
    RwFrame* pPart = RwFrameFindFrame(pFrame, &szDummyName[0]);
    if (pPart == NULL)
        return false;

    // now replace all the objects in the frame
    SReplaceParts data = {0};
    data.pReplacements = pAtomics;
    data.uiReplacements = uiAtomics;
    data.szName = szName;
    RwFrameForAllObjects(pPart, ReplacePartsCB, &data);

    return true;
}

// Replaces a CColModel for a specific object identified by the object id (usModelID)
void CRenderWareSA::ReplaceCollisions(CColModel* pCol, unsigned short usModelID)
{
    DWORD*        pPool = (DWORD*)ARRAY_ModelInfo;
    CColModelSA*  pColModel = (CColModelSA*)pCol;
    CModelInfoSA* pModelInfoSA = (CModelInfoSA*)(pGame->GetModelInfo(usModelID));

    // Apply some low-level hacks (copies the old col area and sets a flag)
    DWORD pColModelInterface = (DWORD)pColModel->GetInterface();
    DWORD pOldColModelInterface = *((DWORD*)pPool[usModelID] + 20);
    MemOrFast<BYTE>(pPool[usModelID] + 0x13, 8);
    MemPutFast<BYTE>(pColModelInterface + 40, *((BYTE*)(pOldColModelInterface + 40)));

    // TODO: It seems that on entering the game, when this function is executed, the modelinfo array for this
    // model is still zero, leading to a crash!
    [[maybe_unused]] const bool modelLoaded = pModelInfoSA->IsLoaded();
}

// Destroys a DFF instance
void CRenderWareSA::DestroyDFF(RpClump* pClump)
{
    if (pClump)
        RpClumpDestroy(pClump);
}

// Destroys a TXD instance
void CRenderWareSA::DestroyTXD(RwTexDictionary* pTXD)
{
    if (pTXD)
        RwTexDictionaryDestroy(pTXD);
}

// Destroys a texture instance
void CRenderWareSA::DestroyTexture(RwTexture* pTex)
{
    if (pTex)
    {
        ScriptRemovedTexture(pTex);
        RwTextureDestroy(pTex);
    }
}

bool CRenderWareSA::RwTexDictionaryRemoveTexture(RwTexDictionary* pTXD, RwTexture* pTex)
{
    if (!pTex || !pTXD ||
        !SharedUtil::IsReadablePointer(pTex, sizeof(RwTexture)) ||
        !SharedUtil::IsReadablePointer(pTXD, sizeof(RwTexDictionary)))
        return false;

    if (pTex->txd != pTXD)
        return false;

    RwListEntry* pNext = pTex->TXDList.next;
    RwListEntry* pPrev = pTex->TXDList.prev;

    if (!pNext || !pPrev ||
        !SharedUtil::IsReadablePointer(pNext, sizeof(RwListEntry)) ||
        !SharedUtil::IsReadablePointer(pPrev, sizeof(RwListEntry)))
        return false;

    if (pNext->prev != &pTex->TXDList || pPrev->next != &pTex->TXDList)
        return false;

    pPrev->next = pNext;
    pNext->prev = pPrev;
    pTex->TXDList.next = &pTex->TXDList;
    pTex->TXDList.prev = &pTex->TXDList;
    pTex->txd = nullptr;
    return true;
}

short CRenderWareSA::CTxdStore_GetTxdRefcount(unsigned short usTxdID)
{
    return *(short*)(*(*(DWORD**)0xC8800C) + 0xC * usTxdID + 4);
}

bool CRenderWareSA::RwTexDictionaryContainsTexture(RwTexDictionary* pTXD, RwTexture* pTex)
{
    if (!pTex || !pTXD ||
        !SharedUtil::IsReadablePointer(pTex, sizeof(RwTexture)) ||
        !SharedUtil::IsReadablePointer(pTXD, sizeof(RwTexDictionary)))
        return false;
    
    return pTex->txd == pTXD;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::TxdForceUnload
//
// 1. Unload txd from memory
// 2. Cross fingers
//
// Why do we do this?
// Player model adds (seemingly) unnecessary refs
// (Will crash if anything is actually using the txd)
//
// Handles custom txd replacements
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::TxdForceUnload(ushort usTxdId, bool bDestroyTextures)
{
    RwTexDictionary* pTxd = CTxdStore_GetTxd(usTxdId);
    if (!pTxd)
        return;

    constexpr int kMaxTextureUnrefs = 10000;
    constexpr int kMaxTxdUnrefs = 1000;

    // Optionally destroy textures first. Skipping is safer but leaks memory.
    if (bDestroyTextures)
    {
        // Remove extra refs from each texture, then destroy when refs == 1
        std::vector<RwTexture*> textureList;
        GetTxdTextures(textureList, pTxd);
        for (RwTexture* pTexture : textureList)
        {
            if (!pTexture)
                continue;

            if (pTexture->refs < 1)
                continue;

            int textureUnrefCount = 0;
            while (pTexture->refs > 1 && textureUnrefCount < kMaxTextureUnrefs)
            {
                RwTextureDestroy(pTexture);
                ++textureUnrefCount;
            }

            int remainingRefs = pTexture->refs;
            if (textureUnrefCount >= kMaxTextureUnrefs && remainingRefs > 1)
            {
                AddReportLog(8625, SString("TxdForceUnload: Texture unref limit hit for TXD %d (refs remaining: %d)", usTxdId, remainingRefs));
                continue;
            }

            if (remainingRefs == 1)
                RwTextureDestroy(pTexture);
        }
    }

    // Ensure at least one ref exists so RemoveRef can trigger cleanup
    if (CTxdStore_GetNumRefs(usTxdId) == 0)
        CRenderWareSA::DebugTxdAddRef(usTxdId);

    int txdUnrefCount = 0;
    while (CTxdStore_GetNumRefs(usTxdId) > 0 && txdUnrefCount < kMaxTxdUnrefs)
    {
        CRenderWareSA::DebugTxdRemoveRef(usTxdId);
        ++txdUnrefCount;
    }

    if (txdUnrefCount >= kMaxTxdUnrefs)
    {
        int remainingTxdRefs = CTxdStore_GetNumRefs(usTxdId);
        if (remainingTxdRefs > 0)
        {
            AddReportLog(8626, SString("TxdForceUnload: TXD unref limit hit for TXD %d (refs remaining: %d)", usTxdId, remainingTxdRefs));
        }
    }
}

namespace
{
    struct TextureMapping
    {
        const char* externalPrefix;    // e.g., "remap"
        const char* internalPrefix;    // e.g., "#emap"
        size_t      externalLength;    // strlen(externalPrefix)
        size_t      internalLength;    // strlen(internalPrefix)
    };

    // Static mappings for texture name transformations
    // GTA:SA renames certain texture prefixes internally in CVehicleModelInfo::FindTextureCB
    // This specifically handles vehicle paintjob textures.
    // Note: This may false positive on non-vehicle textures with matching prefixes,
    // but the fallback lookup pattern (try original first, then transformed) mitigates this.
    constexpr std::array<TextureMapping, 2> kTextureMappings = {{
        {"remap", "#emap", 5, 5},
        {"white", "@hite", 5, 5}
    }};
    
    // Thread-local buffers for transformed texture names (avoids allocation)
    // Note: These are overwritten on each call - do not store the returned pointer
    // for use after another call to these functions.
    thread_local char s_szInternalNameBuffer[RW_TEXTURE_NAME_LENGTH + 1];
    thread_local char s_szExternalNameBuffer[RW_TEXTURE_NAME_LENGTH + 1];
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetInternalTextureName
//
// Maps external texture names to internal GTA:SA names.
// Handles prefix-based matches (e.g., "remap_body" -> "#emap_body").
// Returns original name if no mapping exists.
//
// Note: Returns pointer to thread-local static buffer when transformation
// occurs. The returned string is only valid until the next call to this function
// on the same thread. Copy the result if you need to preserve it.
//
////////////////////////////////////////////////////////////////
const char* CRenderWareSA::GetInternalTextureName(const char* szExternalName)
{
    if (!szExternalName || !szExternalName[0])
        return szExternalName;

    for (const auto& mapping : kTextureMappings)
    {
        // Check if the external name starts with this prefix (case-insensitive)
        if (_strnicmp(szExternalName, mapping.externalPrefix, mapping.externalLength) == 0)
        {
            // Build internal name: internal prefix + rest of original name
            const char* szSuffix = szExternalName + mapping.externalLength;
            
            // Build the transformed name (only snprintf is safe for it)
            snprintf(s_szInternalNameBuffer, sizeof(s_szInternalNameBuffer), "%s%s", 
                     mapping.internalPrefix, szSuffix);
            
            return s_szInternalNameBuffer;
        }
    }
    return szExternalName;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetExternalTextureName
//
// Maps internal GTA:SA names to external texture names.
// Handles prefix-based matches (e.g., "#emap_body" -> "remap_body").
// Returns original name if no mapping exists.
//
// Note: Returns pointer to thread-local static buffer when transformation
// occurs. The returned string is only valid until the next call to this function
// on the same thread. Copy the result if you need to preserve it.
//
////////////////////////////////////////////////////////////////
const char* CRenderWareSA::GetExternalTextureName(const char* szInternalName)
{
    if (!szInternalName || !szInternalName[0])
        return szInternalName;

    for (const auto& mapping : kTextureMappings)
    {
        // Check if the internal name starts with this prefix (case-insensitive)
        if (_strnicmp(szInternalName, mapping.internalPrefix, mapping.internalLength) == 0)
        {
            // Build external name: external prefix + rest of original name
            const char* szSuffix = szInternalName + mapping.internalLength;
            
            // Build the transformed name (only snprintf is safe for it)
            snprintf(s_szExternalNameBuffer, sizeof(s_szExternalNameBuffer), "%s%s",
                     mapping.externalPrefix, szSuffix);
            
            return s_szExternalNameBuffer;
        }
    }
    return szInternalName;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTXDIDForModelID
//
// Get a TXD ID associated with the model ID
//
////////////////////////////////////////////////////////////////
ushort CRenderWareSA::GetTXDIDForModelID(ushort usModelID)
{
    if (usModelID >= pGame->GetBaseIDforTXD() && usModelID < pGame->GetBaseIDforCOL())
    {
        // Get global TXD ID instead
        return usModelID - pGame->GetBaseIDforTXD();
    }
    else
    {
        // Get the CModelInfo's TXD ID

        // Ensure valid
        if (usModelID >= pGame->GetBaseIDforTXD() || !((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID])
            return 0;

        return ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTextureNames
//
// Get list of texture names associated with the model
// Will try to load the model if needed
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetModelTextureNames(std::vector<SString>& outNameList, ushort usModelId)
{
    outNameList.clear();

    // Special case for CJ
    if (usModelId == 0)
    {
        outNameList.push_back("CJ");
        return;
    }

    ushort usTxdId = GetTXDIDForModelID(usModelId);

    if (usTxdId == 0)
        return;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd(usTxdId);

    bool bLoadedModel = false;
    if (!pTXD)
    {
        // Try model load
        bLoadedModel = true;
        pGame->GetModelInfo(usModelId)->Request(BLOCKING, "CRenderWareSA::GetModelTextureNames");
        pTXD = CTxdStore_GetTxd(usTxdId);
        
        // Revalidate TXD pointer after load - it may still be NULL or have been GC'd
        if (!pTXD)
        {
            if (bLoadedModel)
                ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
            return;
        }
    }

    std::vector<RwTexture*> textureList;
    GetTxdTextures(textureList, pTXD);

    for (RwTexture* pTexture : textureList)
    {
        // Fix for #emap corruption:
        // Some textures (like 'remap') are internally renamed to start with '#' (e.g. '#emap') by SA.
        // This causes issues when scripts try to access them by their original name.
        // We detect this case and return the expected name 'remap' instead.
        outNameList.push_back(GetExternalTextureName(pTexture->name));
    }

    if (bLoadedModel)
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTextures
//
// Get textures associated with the model
// Will try to load the model if needed
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::GetModelTextures(std::vector<std::tuple<std::string, CPixels>>& outTextureList, ushort usModelId, std::vector<SString> vTextureNames)
{
    outTextureList.clear();

    ushort usTxdId = GetTXDIDForModelID(usModelId);

    if (usTxdId == 0)
        return false;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd(usTxdId);

    bool bLoadedModel = false;
    if (!pTXD)
    {
        // Try model load
        bLoadedModel = true;
        pGame->GetModelInfo(usModelId)->Request(BLOCKING, "CRenderWareSA::GetModelTextures");
        pTXD = CTxdStore_GetTxd(usTxdId);
        
        // Revalidate TXD pointer after load - it may still be NULL or have been GC'd
        if (!pTXD)
        {
            if (bLoadedModel)
                ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
            return false;
        }
    }

    std::vector<RwTexture*> rwTextureList;
    GetTxdTextures(rwTextureList, pTXD);

    // If texture list is empty after enumeration
    if (rwTextureList.empty())
    {
        if (bLoadedModel)
            ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);
        return false;
    }

    // If any texture names specified in vTextureNames, we should only return these
    const bool bExcludeTextures = !vTextureNames.empty();

    for (RwTexture* pTexture : rwTextureList)
    {
        SString strTextureName = GetExternalTextureName(pTexture->name);

        bool    bValidTexture = false;

        if (bExcludeTextures)
        {
            for (const auto& str : vTextureNames)
            {
                if (WildcardMatchI(strTextureName, str))
                {
                    bValidTexture = true;
                }
            }
        }
        else
            bValidTexture = true;

        if (bValidTexture)
        {
            RwD3D9Raster* pD3DRaster = (RwD3D9Raster*)(&pTexture->raster->renderResource);
            CPixels       texture;
            g_pCore->GetGraphics()->GetPixelsManager()->GetTexturePixels(pD3DRaster->texture, texture);
            outTextureList.emplace_back(strTextureName, std::move(texture));
        }
    }

    if (bLoadedModel)
        ((void(__cdecl*)(unsigned short))FUNC_RemoveModel)(usModelId);

    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures
//
// If TXD must already be loaded
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetTxdTextures(std::vector<RwTexture*>& outTextureList, ushort usTxdId)
{
    if (usTxdId == 0)
        return;

    // Get the TXD corresponding to this ID
    RwTexDictionary* pTXD = CTxdStore_GetTxd(usTxdId);

    if (!pTXD)
        return;

    if (!SharedUtil::IsReadablePointer(pTXD, sizeof(*pTXD)))
        return;

    GetTxdTextures(outTextureList, pTXD);
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures
//
// Get textures from a TXD
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetTxdTextures(std::vector<RwTexture*>& outTextureList, RwTexDictionary* pTXD)
{
    if (!pTXD)
        return;

    // Validate TXD structure is readable (includes textures member)
    if (!SharedUtil::IsReadablePointer(pTXD, sizeof(*pTXD)))
        return;

    constexpr std::size_t kMaxReasonableTextures = 8192;
    if (outTextureList.size() >= kMaxReasonableTextures)
    {
        LogEvent(852, "Texture enumeration aborted", "CRenderWareSA::GetTxdTextures",
                 SString("Texture list already contains %zu textures (limit: %zu)", outTextureList.size(), kMaxReasonableTextures), 5422);
        return;
    }

    if (outTextureList.empty())
        outTextureList.reserve(16);

    // Manual iteration avoids freezes on bad TXD lists.
    RwListEntry* const pRoot = &pTXD->textures.root;
    RwListEntry*       pNode = pRoot->next;

    if (pNode == nullptr)
        return;

    if (pNode == pRoot)
        return;  // Empty TXD

    std::size_t iterations = 0;
    while (pNode != pRoot)
    {
        if (++iterations > kMaxReasonableTextures)
        {
            LogEvent(852, "Texture enumeration aborted", "CRenderWareSA::GetTxdTextures",
                     SString("Texture list enumeration exceeded %zu iterations (possible TXD corruption/cycle)", kMaxReasonableTextures), 5422);
            return;
        }

        if (!SharedUtil::IsReadablePointer(pNode, sizeof(RwListEntry)))
            return;

        RwTexture* pTexture = reinterpret_cast<RwTexture*>(reinterpret_cast<char*>(pNode) - offsetof(RwTexture, TXDList));
        if (!SharedUtil::IsReadablePointer(pTexture, sizeof(RwTexture)))
            return;

        outTextureList.push_back(pTexture);
        if (outTextureList.size() >= kMaxReasonableTextures)
            return;

        if (!SharedUtil::IsReadablePointer(pNode->next, sizeof(RwListEntry)))
            return;

        pNode = pNode->next;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures (unordered_set overload)
//
// Get textures from a TXD into unordered_set for O(1) contains
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::GetTxdTextures(std::unordered_set<RwTexture*>& outTextureSet, RwTexDictionary* pTXD)
{
    if (!pTXD)
        return;

    if (!SharedUtil::IsReadablePointer(pTXD, sizeof(*pTXD)))
        return;

    constexpr std::size_t kMaxReasonableTextures = 8192;
    if (outTextureSet.size() >= kMaxReasonableTextures)
    {
        LogEvent(852, "Texture enumeration aborted", "CRenderWareSA::GetTxdTextures",
                 SString("Texture set already contains %zu textures (limit: %zu)", outTextureSet.size(), kMaxReasonableTextures), 5422);
        return;
    }

    if (outTextureSet.empty())
        outTextureSet.reserve(16);

    // Manual iteration avoids freezes on bad TXD lists.
    RwListEntry* const pRoot = &pTXD->textures.root;
    RwListEntry*       pNode = pRoot->next;

    if (pNode == nullptr)
        return;

    if (pNode == pRoot)
        return;  // Empty TXD

    // The set size doesnt grow on duplicates; cap iterations separately.
    std::size_t iterations = 0;
    while (pNode != pRoot)
    {
        if (++iterations > kMaxReasonableTextures)
        {
            LogEvent(852, "Texture enumeration aborted", "CRenderWareSA::GetTxdTextures",
                     SString("Texture set enumeration exceeded %zu iterations (possible TXD corruption/cycle)", kMaxReasonableTextures), 5422);
            return;
        }

        if (!SharedUtil::IsReadablePointer(pNode, sizeof(RwListEntry)))
            return;

        RwTexture* pTexture = reinterpret_cast<RwTexture*>(reinterpret_cast<char*>(pNode) - offsetof(RwTexture, TXDList));
        if (!SharedUtil::IsReadablePointer(pTexture, sizeof(RwTexture)))
            return;

        if (outTextureSet.size() >= kMaxReasonableTextures)
            return;

        outTextureSet.insert(pTexture);

        if (!SharedUtil::IsReadablePointer(pNode->next, sizeof(RwListEntry)))
            return;

        pNode = pNode->next;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTextureName
//
// Only called by CRenderItemManager::GetVisibleTextureNames ?
//
////////////////////////////////////////////////////////////////
const char* CRenderWareSA::GetTextureName(CD3DDUMMY* pD3DData)
{
    STexInfo** ppTexInfo = MapFind(m_D3DDataTexInfoMap, pD3DData);
    if (ppTexInfo)
        return (*ppTexInfo)->strTextureName;
    if (!pD3DData)
        return FAKE_NAME_NO_TEXTURE;
    return "";
}

// Note: GetEmptyMapKey/GetDeletedMapKey for CD3DDUMMY* are now declared in CRenderWareSA.ShaderSupport.h
// to ensure they are visible before CFastHashMap<CD3DDUMMY*, ...> template instantiatin

RwFrame* CRenderWareSA::GetFrameFromName(RpClump* pRoot, SString strName)
{
    return RwFrameFindFrame(RpGetFrame(pRoot), strName);
}

void CRenderWareSA::CMatrixToRwMatrix(const CMatrix& mat, RwMatrix& rwOutMatrix)
{
    rwOutMatrix.right = (RwV3d&)mat.vRight;
    rwOutMatrix.up = (RwV3d&)mat.vFront;
    rwOutMatrix.at = (RwV3d&)mat.vUp;
    rwOutMatrix.pos = (RwV3d&)mat.vPos;
}

void CRenderWareSA::RwMatrixToCMatrix(const RwMatrix& rwMatrix, CMatrix& matOut)
{
    matOut.vRight = (CVector&)rwMatrix.right;
    matOut.vFront = (CVector&)rwMatrix.up;
    matOut.vUp = (CVector&)rwMatrix.at;
    matOut.vPos = (CVector&)rwMatrix.pos;
}

void CRenderWareSA::RwMatrixGetRotation(const RwMatrix& rwMatrix, CVector& vecOutRotation)
{
    CMatrix matTemp;
    RwMatrixToCMatrix(rwMatrix, matTemp);
    vecOutRotation = matTemp.GetRotation();
}

void CRenderWareSA::RwMatrixSetRotation(RwMatrix& rwInOutMatrix, const CVector& vecRotation)
{
    CMatrix matTemp;
    RwMatrixToCMatrix(rwInOutMatrix, matTemp);
    matTemp.SetRotation(vecRotation);
    rwInOutMatrix.right = (RwV3d&)matTemp.vRight;
    rwInOutMatrix.up = (RwV3d&)matTemp.vFront;
    rwInOutMatrix.at = (RwV3d&)matTemp.vUp;
}

void CRenderWareSA::RwMatrixGetPosition(const RwMatrix& rwMatrix, CVector& vecOutPosition)
{
    vecOutPosition = (CVector&)rwMatrix.pos;
}

void CRenderWareSA::RwMatrixSetPosition(RwMatrix& rwInOutMatrix, const CVector& vecPosition)
{
    rwInOutMatrix.pos = (RwV3d&)vecPosition;
}

void CRenderWareSA::RwMatrixGetScale(const RwMatrix& rwMatrix, CVector& vecOutScale)
{
    CMatrix matTemp;
    RwMatrixToCMatrix(rwMatrix, matTemp);
    vecOutScale = matTemp.GetScale();
}

void CRenderWareSA::RwMatrixSetScale(RwMatrix& rwInOutMatrix, const CVector& vecScale)
{
    CMatrix matTemp;
    RwMatrixToCMatrix(rwInOutMatrix, matTemp);
    matTemp.SetScale(vecScale);
    rwInOutMatrix.right = (RwV3d&)matTemp.vRight;
    rwInOutMatrix.up = (RwV3d&)matTemp.vFront;
    rwInOutMatrix.at = (RwV3d&)matTemp.vUp;
}
