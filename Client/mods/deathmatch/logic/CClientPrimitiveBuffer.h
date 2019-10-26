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
    CVector  xyz;
    D3DCOLOR diffuse;
};

struct VertexXYZUVDiffuse
{
    CVector  xyz;
    float    u, v;
    D3DCOLOR diffuse;
};

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

    template <typename T>
    void CreateBuffer(std::vector<T>& vecVertexList, std::vector<int>& vecIndexList, int FVF);
    void CreateBuffer(std::vector<VertexXYZ>& vecVertexList, std::vector<int>& vecIndexList);
    void CreateBuffer(std::vector<VertexXYZUV>& vecVertexList, std::vector<int>& vecIndexList);
    void CreateBuffer(std::vector<VertexXYZDiffuse>& vecVertexList, std::vector<int>& vecIndexList);
    void CreateBuffer(std::vector<VertexXYZUVDiffuse>& vecVertexList, std::vector<int>& vecIndexList);

    void PreDraw();
    void Draw(CMatrix& matrix);

private:
    IDirect3DIndexBuffer9*       m_pIndexBuffer;
    IDirect3DVertexBuffer9*      m_pVertexBuffer;
    IDirect3DVertexDeclaration9* m_pVertexDeclaration;
    D3DPRIMITIVETYPE             m_iPrimitiveType = D3DPT_TRIANGLELIST;
    int                          m_iFaceCount;
    int                          m_iIndicesCount;
    int                          m_iVertexCount;
    int                          m_FVF;
    int                          m_iStrideSize;
    float                        buffer[24];
};
