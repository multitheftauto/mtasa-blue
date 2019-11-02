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

enum ePrimitiveData
{
    PRIMITIVE_DATA_XYZ = 1 << 0,
    PRIMITIVE_DATA_XY = 1 << 1,
    PRIMITIVE_DATA_UV = 1 << 2,
    PRIMITIVE_DATA_DIFFUSE = 1 << 3,
    PRIMITIVE_DATA_INDICES16 = 1 << 29,
    PRIMITIVE_DATA_INDICES32 = 1 << 30,
};
DECLARE_ENUM(ePrimitiveData);

static std::map<ePrimitiveData, size_t> primitiveDataSizeMap =
{
    {ePrimitiveData::PRIMITIVE_DATA_XYZ, 3},
    {ePrimitiveData::PRIMITIVE_DATA_XY, 2},
    {ePrimitiveData::PRIMITIVE_DATA_UV, 2},
    {ePrimitiveData::PRIMITIVE_DATA_DIFFUSE, 1},
};

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
