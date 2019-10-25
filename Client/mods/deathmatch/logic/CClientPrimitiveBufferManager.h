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
    PRIMITIVE_DATA_XYZ = 0x1,
    PRIMITIVE_DATA_UV = 0x2,
    PRIMITIVE_DATA_DIFFUSE = 0x4,
};

enum ePrimitiveFormat
{
    PRIMITIVE_FORMAT_XYZ = PRIMITIVE_DATA_XYZ,
    PRIMITIVE_FORMAT_XYZ_UV = PRIMITIVE_DATA_XYZ | PRIMITIVE_DATA_UV,
    PRIMITIVE_FORMAT_XYZ_DIFFUSE = PRIMITIVE_DATA_XYZ | PRIMITIVE_DATA_DIFFUSE,
    PRIMITIVE_FORMAT_XYZ_DIFFUSE_UV = PRIMITIVE_DATA_XYZ | PRIMITIVE_DATA_DIFFUSE | PRIMITIVE_DATA_UV
};
DECLARE_ENUM(ePrimitiveFormat);

struct VertexXYZ
{
    CVector xyz;
};

struct VertexXYZUV
{
    CVector xyz;
    float   u, v;
};

struct VertexXYZDiffuse
{
    CVector xyz;
    int     diffuse;
};

struct VertexXYZUVDiffuse
{
    CVector xyz;
    float   u, v;
    int     diffuse;
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
