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
#include "game/CModelInfo.h"

class CResource;
class CClientManager;

class CClientModel final
{
    friend class CClientModelManager;

public:
    CClientModel(CClientManager* pManager, int iModelID, eModelInfoType eModelType);
    ~CClientModel();

    int              GetModelID() const { return m_iModelID; };
    eModelInfoType   GetModelType() const { return m_eModelType; };
    bool             Allocate(ushort usParentID);
    bool             Deallocate();
    void             RestoreEntitiesUsingThisModel();
    void             SetParentResource(CResource* pResource) { m_pParentResource = pResource; }
    CResource*       GetParentResource() const { return m_pParentResource; }

protected:
    CClientManager* m_pManager;

    int              m_iModelID;
    eModelInfoType   m_eModelType;
    bool             m_bAllocatedByUs = false;
    CResource*       m_pParentResource = nullptr;            // Resource that allocated model
};
