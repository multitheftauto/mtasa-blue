/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientDFF.h
 *  PURPOSE:     .dff model handling class
 *
 *****************************************************************************/

class CClientDFF;

#pragma once

#include <list>
#include "CClientEntity.h"

struct RpClump;

struct SLoadedClumpInfo
{
    SLoadedClumpInfo() : bTriedLoad(false), pClump(NULL) {}
    bool     bTriedLoad;
    RpClump* pClump;
};

class CClientDFF final : public CClientEntity
{
    DECLARE_CLASS(CClientDFF, CClientEntity)
    friend class CClientDFFManager;

public:
    CClientDFF(class CClientManager* pManager, ElementID ID);
    ~CClientDFF();

    eClientEntityType GetType() const { return CCLIENTDFF; }

    bool Load(bool isRaw, SString input);

    bool AddClothingModel(const std::string& modelName);
    bool ReplaceModel(unsigned short usModel, bool bAlphaTransparency);

    bool HasReplaced(unsigned short usModel);

    bool RestoreModel(unsigned short usModel);
    void RestoreModels();

    static bool IsDFFData(const SString& strData);

    // After DoDeleteAll, unload models that had custom DFF replaced via ClearCustomModel.
    // ClearCustomModel only nulls the clump pointer - the custom geometry stays loaded in
    // GTA's streaming cache. This forces a re-stream from disk with original model data.
    static void FlushDeferredModelRestores();

    // Sorta a hack that these are required by CClientEntity...
    void Unlink() {};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition) {};

private:
    bool LoadFromFile(SString filePath);
    bool LoadFromBuffer(SString buffer);

    bool DoReplaceModel(unsigned short usModel, bool bAlphaTransparency);
    void UnloadDFF();

    bool InternalRestoreModel(unsigned short usModel);

    bool ReplaceClothes(ushort usModel);
    bool ReplaceObjectModel(RpClump* pClump, ushort usModel, bool bAlphaTransparency);
    bool ReplaceVehicleModel(RpClump* pClump, ushort usModel, bool bAlphaTransparency);
    bool ReplaceWeaponModel(RpClump* pClump, ushort usModel, bool bAlphaTransparency);
    bool ReplacePedModel(RpClump* pClump, ushort usModel, bool bAlphaTransparency);

    RpClump* GetLoadedClump(ushort usModelId);

    class CClientDFFManager* m_pDFFManager;

    SString                            m_strDffFilename;
    SString                            m_RawDataBuffer;
    bool                               m_bIsRawData = false;
    std::map<ushort, SLoadedClumpInfo> m_LoadedClumpInfoMap;

    std::list<unsigned short> m_Replaced;

    static std::vector<unsigned short> ms_DeferredModelRestores;
    static std::vector<RpClump*>       ms_DeferredClumpDestroys;
};
