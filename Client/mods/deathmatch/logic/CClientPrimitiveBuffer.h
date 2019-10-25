/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/CClientPrimitiveBuffer.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClientEntity.h"

class CClientPrimitiveBuffer : public CClientEntity
{
public:
    CClientPrimitiveBuffer(class CClientManager* pManager, ElementID ID);
    virtual eClientEntityType GetType() const { return CCLIENTPRIMITIVEBUFFER; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink();
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    template <typename T>
    void CreateBuffer(std::vector<T>& vecVertexList, std::vector<int>& vecIndexList, int FVF);

private:
    LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
    int                     m_iBufferSize;
    int                     m_iIndicesCount;
    int                     m_iFacesCount;
    int                     m_iVertexCount;
    int                     m_FVF;
    int                     m_iTypeSize;
};
