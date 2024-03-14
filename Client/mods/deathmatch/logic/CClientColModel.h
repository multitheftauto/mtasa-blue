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

class CColModel;

class CClientColModel final : public CClientEntity
{
    DECLARE_CLASS(CClientColModel, CClientEntity)
public:
    CClientColModel(class CClientManager* pManager, ElementID ID);
    ~CClientColModel();

    eClientEntityType GetType() const { return CCLIENTCOL; }

    bool Load(bool isRaw, SString input);

    bool IsLoaded() { return m_pColModel != NULL; };

    bool Replace(std::uint16_t usModel);
    void Restore(std::uint16_t usModel);
    void RestoreAll();

    bool        HasReplaced(std::uint16_t usModel);
    static bool IsCOLData(const SString& strData);

    // Sorta a hack that these are required by CClientEntity...
    void Unlink(){};
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

private:
    bool LoadFromFile(SString filePath);
    bool LoadFromBuffer(SString buffer);

    void InternalRestore(std::uint16_t usModel);

    class CClientColModelManager* m_pColModelManager;

    CColModel*                m_pColModel;
    std::list<std::uint16_t> m_Replaced;
};
