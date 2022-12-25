#include "StdInc.h"
#include "gamesa_renderware.h"
#include "CFileLoaderSA.h"

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
}

class CAtomicModelInfo
{
public:

    void CAtomicModelInfo::DeleteRwObject()
    {
        ((void(__thiscall *)(CAtomicModelInfo*))(*(void ***)this)[8])(this);
    }

    void CAtomicModelInfo::SetAtomic(RpAtomic* atomic)
    {
        ((void(__thiscall *)(CAtomicModelInfo*, RpAtomic*))(*(void ***)this)[15])(this, atomic);
    }

};

class CDamagableModelInfo
{
public:
    void CDamagableModelInfo::SetDamagedAtomic(RpAtomic* atomic)
    {
        ((void(__thiscall *)(CDamagableModelInfo*, RpAtomic*))0x4C48D0)(this, atomic);
    }
};

static char* GetFrameNodeName(RwFrame* frame) {
    return ((char* (__cdecl*)(RwFrame*))0x72FB30)(frame);
}

static void GetNameAndDamage(char const* nodeName, char* outName, bool& outDamage) {
    return ((void(__cdecl*)(char const*, char*, bool&))0x5370A0)(nodeName, outName, outDamage);
}

static void CVisibilityPlugins_SetAtomicRenderCallback(RpAtomic *pRpAtomic, RpAtomic * (*renderCB)(RpAtomic *)) {
    return ((void(__cdecl*)(RpAtomic *, RpAtomic * (*renderCB)(RpAtomic *)))0x7328A0)(pRpAtomic, renderCB);
}

static void CVisibilityPlugins_SetAtomicId(RpAtomic* pRpAtomic, int id) {
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

static auto CModelInfo_ms_modelInfoPtrs = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;
static unsigned int& gAtomicModelId = *reinterpret_cast<unsigned int*>(DWORD_AtomicsReplacerModelID);

bool CFileLoader_LoadAtomicFile(RwStream *stream, unsigned int modelId)
{
    CBaseModelInfoSAInterface* pBaseModelInfo = CModelInfo_ms_modelInfoPtrs[modelId];
    auto pAtomicModelInfo = reinterpret_cast<CAtomicModelInfo*>(pBaseModelInfo);

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

        RpClumpForAllAtomics(pReadClump, (RpClumpForAllAtomicsCB_t)CFileLoader_SetRelatedModelInfoCB, &relatedModelInfo);
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
    char name[24];
    CBaseModelInfoSAInterface* pBaseModelInfo = CModelInfo_ms_modelInfoPtrs[gAtomicModelId];
    auto pAtomicModelInfo = reinterpret_cast<CAtomicModelInfo*>(pBaseModelInfo);
    RwFrame* pOldFrame = reinterpret_cast<RwFrame*>(atomic->object.object.parent);
    char* frameNodeName = GetFrameNodeName(pOldFrame);
    bool bDamage = false;
    GetNameAndDamage(frameNodeName, (char*)&name, bDamage);
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
