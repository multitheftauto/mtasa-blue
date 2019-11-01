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
    float x, y, z;
};

struct VertexXYZUV
{
    float x, y, z;
    float   u, v;
};

struct VertexXYZDiffuse
{
    float  x,y,z;
    D3DCOLOR diffuse;
};

struct VertexXYZDiffuseUV
{
    float  x,y,z;
    D3DCOLOR diffuse;
    float    u, v;
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
    void CreateBuffer(std::vector<T>& vecVertexList, std::vector<int>& vecIndexList, int FVF, D3DVERTEXELEMENT9 dwVertexElement[]);
    void CreateBuffer(std::vector<VertexXYZ>& vecVertexList, std::vector<int>& vecIndexList);
    void CreateBuffer(std::vector<VertexXYZUV>& vecVertexList, std::vector<int>& vecIndexList);
    void CreateBuffer(std::vector<VertexXYZDiffuse>& vecVertexList, std::vector<int>& vecIndexList);
    void CreateBuffer(std::vector<VertexXYZDiffuseUV>& vecVertexList, std::vector<int>& vecIndexList);

    void PreDraw();
    void Draw(PrimitiveBufferSettings& settings);

private:
    IDirect3DIndexBuffer9*        m_pIndexBuffer;
    IDirect3DVertexBuffer9*       m_pVertexBuffer;
    LPDIRECT3DVERTEXDECLARATION9  m_pVertexDeclaration;
    D3DPRIMITIVETYPE              m_iPrimitiveType = D3DPT_TRIANGLELIST;
    IDirect3DDevice9*             m_pDevice;
    int                           m_iFaceCount;
    int                           m_iIndicesCount;
    int                           m_iVertexCount;
    int                           m_FVF;
    int                           m_iStrideSize;
    float                         buffer[24];
};
