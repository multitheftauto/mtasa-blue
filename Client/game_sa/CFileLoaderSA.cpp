/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CFileLoaderSA.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"
#include "CFileLoaderSA.h"
#include "CModelInfoSA.h"

CFileLoaderSA::CFileLoaderSA()
{
}

CFileLoaderSA::~CFileLoaderSA()
{
}

void CFileLoaderSA::StaticSetHooks()
{
    HookInstall(0x5371F0, (DWORD)CFileLoader_LoadAtomicFile, 5);
    HookInstall(0x537150, (DWORD)CFileLoader_SetRelatedModelInfoCB, 5);
    HookInstall(0x538690, (DWORD)CFileLoader_LoadObjectInstance, 5);
}

CEntitySAInterface* CFileLoaderSA::LoadObjectInstance(SFileObjectInstance* obj)
{
    // Second argument is model name. It's unused in the function
    return ((CEntitySAInterface * (__cdecl*)(SFileObjectInstance*, const char*))0x538090)(obj, nullptr);
}

class CAtomicModelInfo
{
public:
    void CAtomicModelInfo::DeleteRwObject() { ((void(__thiscall*)(CAtomicModelInfo*))(*(void***)this)[8])(this); }

    void CAtomicModelInfo::SetAtomic(RpAtomic* atomic) { ((void(__thiscall*)(CAtomicModelInfo*, RpAtomic*))(*(void***)this)[15])(this, atomic); }
};

class CDamagableModelInfo
{
public:
    void CDamagableModelInfo::SetDamagedAtomic(RpAtomic* atomic) { ((void(__thiscall*)(CDamagableModelInfo*, RpAtomic*))0x4C48D0)(this, atomic); }
};

static char* GetFrameNodeName(RwFrame* frame)
{
    return ((char*(__cdecl*)(RwFrame*))0x72FB30)(frame);
}

// Originally there was a possibility for this function to cause buffer overflow
// It should be fixed here.
template <size_t OutBuffSize>
void GetNameAndDamage(const char* nodeName, char (&outName)[OutBuffSize], bool& outDamage)
{
    const auto nodeNameLen = strlen(nodeName);

    const auto NodeNameEndsWith = [=](const char* with) {
        const auto withLen = strlen(with);
        // dassert(withLen <= nodeNameLen);
        return withLen <= nodeNameLen /*dont bother checking otherwise, because it might cause a crash*/
               && strncmp(nodeName + nodeNameLen - withLen, with, withLen) == 0;
    };

    // Copy `nodeName` into `outName` with `off` trimmed from the end
    // Eg.: `dmg_dam` with `off = 4` becomes `dmg`
    const auto TerminatedCopy = [&](size_t off) {
        dassert(nodeNameLen - off < OutBuffSize);
        strncpy_s(outName, nodeName,
                  std::min(nodeNameLen - off, OutBuffSize - 1));            // By providing `OutBuffSize - 1` it is ensured the array will be null terminated
    };

    if (NodeNameEndsWith("_dam"))
    {
        outDamage = true;
        TerminatedCopy(sizeof("_dam") - 1);
    }
    else
    {
        outDamage = false;
        if (NodeNameEndsWith("_l0") || NodeNameEndsWith("_L0"))
        {
            TerminatedCopy(sizeof("_l0") - 1);
        }
        else
        {
            dassert(nodeNameLen < OutBuffSize);
            strncpy_s(outName, OutBuffSize, nodeName, OutBuffSize - 1);
        }
    }
}

static void CVisibilityPlugins_SetAtomicRenderCallback(RpAtomic* pRpAtomic, RpAtomic* (*renderCB)(RpAtomic*))
{
    return ((void(__cdecl*)(RpAtomic*, RpAtomic * (*renderCB)(RpAtomic*)))0x7328A0)(pRpAtomic, renderCB);
}

static void CVisibilityPlugins_SetAtomicId(RpAtomic* pRpAtomic, int id)
{
    return ((void(__cdecl*)(RpAtomic*, int))0x732230)(pRpAtomic, id);
}

static void CVehicleModelInfo_UseCommonVehicleTexDicationary()
{
    ((void(__cdecl*)())0x4C75A0)();
}

static void CVehicleModelInfo_StopUsingCommonVehicleTexDicationary()
{
    ((void(__cdecl*)())0x4C75C0)();
}

static auto          CModelInfo_ms_modelInfoPtrs = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;
static unsigned int& gAtomicModelId = *reinterpret_cast<unsigned int*>(DWORD_AtomicsReplacerModelID);

bool CFileLoader_LoadAtomicFile(RwStream* stream, unsigned int modelId)
{
    CBaseModelInfoSAInterface* pBaseModelInfo = CModelInfo_ms_modelInfoPtrs[modelId];
    auto                       pAtomicModelInfo = reinterpret_cast<CAtomicModelInfo*>(pBaseModelInfo);

    bool bUseCommonVehicleTexDictionary = false;
    if (pAtomicModelInfo && pBaseModelInfo->bWetRoadReflection)
    {
        bUseCommonVehicleTexDictionary = true;
        CVehicleModelInfo_UseCommonVehicleTexDicationary();
    }

    const unsigned int rwID_CLUMP = 16;
    if (RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
    {
        RpClump* pReadClump = RpClumpStreamRead(stream);
        if (!pReadClump)
        {
            if (bUseCommonVehicleTexDictionary)
            {
                CVehicleModelInfo_StopUsingCommonVehicleTexDicationary();
            }
            return false;
        }

        gAtomicModelId = modelId;
        SRelatedModelInfo relatedModelInfo = {0};
        relatedModelInfo.pClump = pReadClump;
        relatedModelInfo.bDeleteOldRwObject = false;

        RpClumpForAllAtomics(pReadClump, reinterpret_cast<RpClumpForAllAtomicsCB_t>(CFileLoader_SetRelatedModelInfoCB), &relatedModelInfo);
        RpClumpDestroy(pReadClump);
    }

    if (!pBaseModelInfo->pRwObject)
    {
        return false;
    }

    if (bUseCommonVehicleTexDictionary)
    {
        CVehicleModelInfo_StopUsingCommonVehicleTexDicationary();
    }
    return true;
}

RpAtomic* CFileLoader_SetRelatedModelInfoCB(RpAtomic* atomic, SRelatedModelInfo* pRelatedModelInfo)
{
    char                       name[24];
    CBaseModelInfoSAInterface* pBaseModelInfo = CModelInfo_ms_modelInfoPtrs[gAtomicModelId];
    auto                       pAtomicModelInfo = reinterpret_cast<CAtomicModelInfo*>(pBaseModelInfo);
    RwFrame*                   pOldFrame = reinterpret_cast<RwFrame*>(atomic->object.object.parent);
    char*                      frameNodeName = GetFrameNodeName(pOldFrame);
    bool                       bDamage = false;
    GetNameAndDamage(frameNodeName, name, bDamage);
    CVisibilityPlugins_SetAtomicRenderCallback(atomic, 0);

    RpAtomic* pOldAtomic = reinterpret_cast<RpAtomic*>(pBaseModelInfo->pRwObject);
    if (bDamage)
    {
        auto pDamagableModelInfo = reinterpret_cast<CDamagableModelInfo*>(pAtomicModelInfo);
        pDamagableModelInfo->SetDamagedAtomic(atomic);
    }
    else
    {
        pAtomicModelInfo->SetAtomic(atomic);
    }

    RpClumpRemoveAtomic(pRelatedModelInfo->pClump, atomic);
    RwFrame* newFrame = RwFrameCreate();
    RpAtomicSetFrame(atomic, newFrame);
    CVisibilityPlugins_SetAtomicId(atomic, gAtomicModelId);

    // Fix #359: engineReplaceModel memory leak
    if (!bDamage && pRelatedModelInfo->bDeleteOldRwObject)
    {
        if (pOldAtomic)
        {
            RpAtomicDestroy(pOldAtomic);
        }

        if (pOldFrame)
        {
            RwFrameDestroy(pOldFrame);
        }
    }
    return atomic;
}

CEntitySAInterface* CFileLoader_LoadObjectInstance(const char* szLine)
{
    char                szModelName[24];
    SFileObjectInstance inst;

    sscanf(szLine, "%d %s %d %f %f %f %f %f %f %f %d", &inst.modelID, szModelName, &inst.interiorID, &inst.position.fX, &inst.position.fY, &inst.position.fZ,
           &inst.rotation.fX, &inst.rotation.fY, &inst.rotation.fZ, &inst.rotation.fW, &inst.lod);

    /*
       A quaternion is must be normalized. GTA is relying on an internal R* exporter and everything is OK,
       but custom exporters might not contain the normalization. And we must do it instead.
   */
    const float fLenSq = inst.rotation.LengthSquared();
    if (fLenSq > 0.0f && std::fabs(fLenSq - 1.0f) > std::numeric_limits<float>::epsilon())
        inst.rotation /= std::sqrt(fLenSq);

    return ((CEntitySAInterface * (__cdecl*)(SFileObjectInstance*))0x538090)(&inst);
}
