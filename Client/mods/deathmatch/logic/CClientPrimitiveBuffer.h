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
#include "CClientPrimitiveBufferInterface.h"
#include "CClientEntity.h"

class CClientPrimitiveBuffer : public CClientPrimitiveBufferInterface, public CClientEntity
{
public:
    CClientPrimitiveBuffer(class CClientManager* pManager, ElementID ID);
    virtual eClientEntityType GetType() const { return CCLIENTPRIMITIVEBUFFER; }

    // Sorta a hack that these are required by CClientEntity...
    void Unlink();
    void GetPosition(CVector& vecPosition) const {};
    void SetPosition(const CVector& vecPosition){};

    void CreateIndexBuffer(std::vector<int>& vecIndexList);
    void CreateIndexBuffer(std::vector<unsigned short>& vecIndexList);

    void AddVertexBuffer(std::vector<CVector>& vecVertexList, ePrimitiveData primitiveData);
    void AddVertexBuffer(std::vector<CVector2D>& vecVertexList, ePrimitiveData primitiveData);
    void AddVertexBuffer(std::vector<D3DCOLOR>& vecVertexList, ePrimitiveData primitiveData);


    void PreDraw();
    void Draw(PrimitiveBufferSettings& settings);
    void SetPrimitiveType(D3DPRIMITIVETYPE ePrimitiveType) { m_ePrimitiveType = ePrimitiveType; }
    void SetFVF(int FVF) { m_FVF = FVF; }
    bool IsRequireMaterial() const { return m_bRequireMaterial; }
    void Finalize();

private:
    IDirect3DDevice9*                                 m_pDevice;
    IDirect3DIndexBuffer9*                            m_pIndexBuffer;
    IDirect3DVertexBuffer9*                           m_arrayVertexBuffer[8];
    int                                               m_iStrideSize[8];
    LPDIRECT3DVERTEXDECLARATION9                      m_pVertexDeclaration;
    D3DPRIMITIVETYPE                                  m_ePrimitiveType;
    std::vector<D3DVERTEXELEMENT9>                    m_vecVertexElements;
    int                                               m_iFaceCount;
    int                                               m_iIndicesCount;
    int                                               m_iVertexCount;
    int                                               m_FVF;
    float                                             buffer[24];
    bool                                              m_bUseIndexedPrimitives;
    bool                                              m_bRequireMaterial;
};
