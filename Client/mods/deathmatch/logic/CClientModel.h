/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientModel.h
 *  PURPOSE:     Model handling class
 *
 *****************************************************************************/

class CClientModel;

#pragma once

#include <list>

enum class eClientModelType
{
    PED,
    OBJECT,
    OBJECT_DAMAGEABLE,
    VEHICLE,
    TIMED_OBJECT,
    CLUMP,
    TXD,
};

class CResource;
class CClientManager;

class CClientModel final
{
    friend class CClientModelManager;

public:
    CClientModel(CClientManager* pManager, int iModelID, eClientModelType eModelType);
    ~CClientModel(void);

    int              GetModelID(void) const { return m_iModelID; };
    eClientModelType GetModelType(void) const { return m_eModelType; };
    bool             Allocate(ushort usParentID);
    bool             AllocateTXD(std::string& strTxdName);
    bool             Deallocate(void);
    void             SetParentResource(CResource* pResource) { m_pParentResource = pResource; }
    CResource*       GetParentResource(void) const { return m_pParentResource; }
    void             RestoreEntitiesUsingThisModel();

private:
    void RestoreDFF(CModelInfo* pModelInfo);
    void RestoreTXD(CModelInfo* pModelInfo);

protected:
    CClientManager* m_pManager;

    int              m_iModelID;
    eClientModelType m_eModelType;
    bool             m_bAllocatedByUs = false;
    CResource*       m_pParentResource = nullptr;            // Resource that allocated model
};
