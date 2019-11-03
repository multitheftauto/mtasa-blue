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

    CClientPrimitiveBuffer* Create();

    void Delete(CClientPrimitiveBuffer* pPrimitiveBuffer);
    void DeleteAll();

    std::list<CClientPrimitiveBuffer*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CClientPrimitiveBuffer*>::const_iterator IterEnd() { return m_List.end(); };

    CClientPrimitiveBuffer* Get(unsigned long ulID);
    static CClientPrimitiveBuffer* Get(ElementID ID);

private:
    void AddToList(CClientPrimitiveBuffer* pPrimitiveBuffer) { m_List.push_back(pPrimitiveBuffer); };
    void RemoveFromList(CClientPrimitiveBuffer* pPrimitiveBuffer);

private:
    CClientManager* m_pManager;

    std::list<CClientPrimitiveBuffer*> m_List;
};
