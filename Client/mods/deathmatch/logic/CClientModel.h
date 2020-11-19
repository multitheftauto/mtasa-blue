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

#include <optional>

enum class eClientModelType
{
    PED,
    OBJECT,
    VEHICLE,
};

class CResource;
class CClientManager;

class CClientModel
{
    friend class CClientModelManager;

public:
    CClientModel(int iModelID, eClientModelType eModelType);
    ~CClientModel(void);
    
public:
    int              GetModelID(void) const { return m_iModelID; };
    eClientModelType GetModelType(void) const { return m_eModelType; };
    bool             Allocate(std::optional<ushort> usParentID);
    bool             Deallocate(void);
    void             RestoreEntitiesUsingThisModel();
    void             SetParentResource(CResource* pResource) { m_pParentResource = pResource; }
    CResource*       GetParentResource(void) const { return m_pParentResource; }

    static unsigned short GetDefaultParentModel(eClientModelType type);

protected:
    int              m_iModelID;
    eClientModelType m_eModelType;
    bool             m_bAllocatedByUs;
    CResource*       m_pParentResource; // Resource that allocated model
};
