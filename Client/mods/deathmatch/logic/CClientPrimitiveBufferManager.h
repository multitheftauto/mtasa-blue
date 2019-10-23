/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientPrimitiveBufferManager.cpp
 *  PURPOSE:     Text PrimitiveBuffer manager class
 *
 *****************************************************************************/

class CClientPrimitiveBufferManager;

#pragma once

#include "CClientManager.h"
#include <list>

class CClientPrimitiveBuffer;

class CClientPrimitiveBufferManager
{
    friend class CClientPrimitiveBuffer;

public:
    CClientPrimitiveBufferManager(CClientManager* pManager);
    ~CClientPrimitiveBufferManager();

    unsigned int    Count() { return static_cast<unsigned int>(m_List.size()); };
    CClientPrimitiveBuffer* Get(unsigned long ulID);

    void AddToList(CClientPrimitiveBuffer* pPrimitiveBuffer);
    void RemoveFromList(CClientPrimitiveBuffer* pPrimitiveBuffer);
    void RemoveAll();

private:
    std::list<CClientPrimitiveBuffer*> m_List;
    bool                               m_bCanRemoveFromList;
    CClientManager*                    m_pManager;
};
