#pragma
#ifndef __CIFPENGINE_H
#define __CIFPENGINE_H

class CIFPEngine
{
public:
    enum eRestoreAnimation
    {
        SINGLE,
        BLOCK,
        ALL
    };

    static std::shared_ptr < CClientIFP > EngineLoadIFP           ( CResource * pResource, CClientManager * pManager, const SString & strPath, const SString & strBlockName );
    static bool                           EngineReplaceAnimation  ( CClientEntity * pEntity, const SString & strInternalBlockName, const SString & strInternalAnimName, const SString & strCustomBlockName, const SString & strCustomAnimName );
    static bool                           EngineRestoreAnimation  ( CClientEntity * pEntity, const SString & strInternalBlockName, const SString & strInternalAnimName, eRestoreAnimation RestoreType );

};

#endif
