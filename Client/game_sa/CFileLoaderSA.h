#pragma once

struct SRelatedModelInfo
{
    RpClump* pClump;
    bool bDeleteOldRwObject;
};

class CFileLoaderSA
{
public:
    CFileLoaderSA();
    ~CFileLoaderSA();

    static void StaticSetHooks();
};

bool CFileLoader_LoadAtomicFile(RwStream *stream, unsigned int modelId);
RpAtomic* CFileLoader_SetRelatedModelInfoCB(RpAtomic* atomic, SRelatedModelInfo* pRelatedModelInfo);
