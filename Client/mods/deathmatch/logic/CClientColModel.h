/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColModel.h
 *  PURPOSE:     Model collision (.col file) entity class
 *
 *****************************************************************************/

#pragma once

#include <list>
#include "CClientEntity.h"

class CClientColModel final : public CClientEntity
{
    DECLARE_CLASS(CClientColModel, CClientEntity)
public:
    CClientColModel(class CClientManager* pManager, ElementID ID);
    ~CClientColModel();

    eClientEntityType GetType() const { return CCLIENTCOL; }

    bool LoadCol(const SString& strFile, bool bIsRawData);
    bool Load(bool isRaw, SString input);

    bool IsLoaded() { return m_pColModel != NULL; };

    bool Replace(unsigned short usModel);
    void Restore(unsigned short usModel);
    void RestoreAll();

    bool        HasReplaced(unsigned short usModel);
    static bool IsCOLData(const SString& strData);

    // Sorta a hack that these are required by CClientEntity...
    void Unlink(){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

private:
    bool LoadFromFile(SString filePath);
    bool LoadFromBuffer(SString buffer);

    void InternalRestore(unsigned short usModel);

    class CClientColModelManager* m_pColModelManager;

    CColModel*                m_pColModel;
    std::list<unsigned short> m_Replaced;
};
