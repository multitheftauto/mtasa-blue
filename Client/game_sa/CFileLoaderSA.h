#pragma once

class CFileLoaderSA
{
public:
    CFileLoaderSA();
    ~CFileLoaderSA();

    void InstallHooks();
};

RpAtomic* CFileLoader_SetRelatedModelInfoCB(RpAtomic* atomic, RpClump* clump);
