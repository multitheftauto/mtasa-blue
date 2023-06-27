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
    VEHICLE,
    TIMED_OBJECT,
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
    unsigned char    GetModelInfoType() const;
    bool             Allocate(ushort usParentID);
    bool             MakeClumpModel();
    bool             MakeAtomicModel();
    bool             Deallocate(void);
    void             RestoreEntitiesUsingThisModel();
    void             SetParentResource(CResource* pResource) { m_pParentResource = pResource; }
    CResource*       GetParentResource(void) const { return m_pParentResource; }

protected:
    CClientManager* m_pManager;

    int              m_iModelID;
    eClientModelType m_eModelType;
    bool             m_bAllocatedByUs = false;
    CResource*       m_pParentResource = nullptr;            // Resource that allocated model
    unsigned char    m_originalModelInfoType = 255; // 255 - unchanged
};
