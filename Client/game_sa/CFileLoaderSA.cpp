#include "StdInc.h"
#include "gamesa_renderware.h"
#include "CFileLoaderSA.h"

CFileLoaderSA::CFileLoaderSA()
{
    InstallHooks();
}

CFileLoaderSA::~CFileLoaderSA()
{

}

void CFileLoaderSA::InstallHooks()
{
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

static RpAtomic* CFileLoader_SetRelatedModelInfoCB(RpAtomic* atomic, RpClump* clump)
{
    static auto CModelInfo_ms_modelInfoPtrs = (CBaseModelInfoSAInterface**)ARRAY_ModelInfo;
    static unsigned int& gAtomicModelId = *reinterpret_cast<unsigned int*>(DWORD_AtomicsReplacerModelID);

    char name[24];
    auto pAtomicModelInfo = reinterpret_cast<CAtomicModelInfo*>(CModelInfo_ms_modelInfoPtrs[gAtomicModelId]);
    char* frameNodeName = GetFrameNodeName((RwFrame*)atomic->object.object.parent);
    bool bDamage = false;
    GetNameAndDamage(frameNodeName, (char*)&name, bDamage);
    CVisibilityPlugins_SetAtomicRenderCallback(atomic, 0);

    // Fix #359: engineReplaceModel memory leak
    // Delete the current atomic before setting a new one.
    pAtomicModelInfo->DeleteRwObject();
    if (bDamage)
    {
        auto pDamagableModelInfo = reinterpret_cast<CDamagableModelInfo*>(pAtomicModelInfo);
        pDamagableModelInfo->SetDamagedAtomic(atomic);
    }
    else
    {
        pAtomicModelInfo->SetAtomic(atomic);
    }
    RpClumpRemoveAtomic(clump, atomic);
    RwFrame* newFrame = RwFrameCreate();
    RpAtomicSetFrame(atomic, newFrame);
    CVisibilityPlugins_SetAtomicId(atomic, gAtomicModelId);
    return atomic;
}
