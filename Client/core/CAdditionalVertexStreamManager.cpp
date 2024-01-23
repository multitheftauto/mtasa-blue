/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAdditionalVertexStreamManager.h"

CAdditionalVertexStreamManager* CAdditionalVertexStreamManager::ms_Singleton = nullptr;

namespace
{
    // Helper functions for this file only

    // Convert size of PT stream to sizeof of N stream
    std::uint32_t ConvertPTSize(std::uint32_t SizePT) { return SizePT * 12 / 20; }

    // Get 64 bit key for a triangle by using the ordered vertex indices
    std::int64_t getTriKey(WORD a, WORD b, WORD c)
    {
        WORD tmp;
        if (b < a)
        {
            tmp = b;
            b = a;
            a = tmp;
        }
        if (c < b)
        {
            tmp = c;
            c = b;
            b = tmp;
        }
        if (b < a)
        {
            tmp = b;
            b = a;
            a = tmp;
        }
        return (((std::int64_t)a) << 32) | (((std::int64_t)b) << 16) | ((std::int64_t)c);
    }
}            // namespace

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::CAdditionalVertexStreamManager
//
//
//
///////////////////////////////////////////////////////////////
CAdditionalVertexStreamManager::CAdditionalVertexStreamManager()
{
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::~CAdditionalVertexStreamManager
//
//
//
///////////////////////////////////////////////////////////////
CAdditionalVertexStreamManager::~CAdditionalVertexStreamManager()
{
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::GetSingleton
//
// Static function
//
///////////////////////////////////////////////////////////////
CAdditionalVertexStreamManager* CAdditionalVertexStreamManager::GetSingleton()
{
    if (!ms_Singleton)
        ms_Singleton = new CAdditionalVertexStreamManager();
    return ms_Singleton;
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::OnDeviceCreate
//
//
//
///////////////////////////////////////////////////////////////
void CAdditionalVertexStreamManager::OnDeviceCreate(IDirect3DDevice9* pDevice)
{
    m_pDevice = pDevice;
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::MaybeSetAdditionalVertexStream
//
// Returns true if did set
//
///////////////////////////////////////////////////////////////
bool CAdditionalVertexStreamManager::MaybeSetAdditionalVertexStream(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices,
                                                                    UINT startIndex, UINT primCount)
{
    // Cache info
    SCurrentStateInfo state;

    // Save call arguments
    state.args.PrimitiveType = PrimitiveType;
    state.args.BaseVertexIndex = BaseVertexIndex;
    state.args.MinVertexIndex = MinVertexIndex;
    state.args.NumVertices = NumVertices;
    state.args.startIndex = startIndex;
    state.args.primCount = primCount;

    // Cache info about state streams etc
    UpdateCurrentStateInfo(state);

    // For now, this only works if the original has 3 decl elements (0:D, 1:P, 1:T) and stream 1 has a stride of 20
    if (!CheckCanDoThis(state))
        return false;

    SetAdditionalVertexStream(state);
    return true;
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::SetAdditionalVertexStream
//
//
//
///////////////////////////////////////////////////////////////
void CAdditionalVertexStreamManager::SetAdditionalVertexStream(SCurrentStateInfo& state)
{
    HRESULT hr;

    // Get matching custom N vbuffer for std PT vbuffer
    SAdditionalStreamInfo* pAdditionalInfo = GetAdditionalStreamInfo(state.stream1.pStreamData);
    if (!pAdditionalInfo)
        pAdditionalInfo = CreateAdditionalStreamInfo(state);

    std::uint32_t StridePT = 20;
    std::uint32_t StrideN = 12;

    // Calc area we are going to use
    std::uint32_t viMinBased = state.args.MinVertexIndex + state.args.BaseVertexIndex;
    std::uint32_t viMaxBased = state.args.MinVertexIndex + state.args.NumVertices + state.args.BaseVertexIndex;

    std::uint32_t ReadOffsetStart = viMinBased * StridePT + state.stream1.OffsetInBytes;
    std::uint32_t ReadOffsetSize = (viMaxBased - viMinBased) * StridePT;

    std::uint32_t OffsetInBytesN = ConvertPTSize(state.stream1.OffsetInBytes);
    std::uint32_t WriteOffsetStart = viMinBased * StrideN + OffsetInBytesN;
    std::uint32_t WriteOffsetSize = (viMaxBased - viMinBased) * StrideN;

    assert(WriteOffsetStart == ConvertPTSize(ReadOffsetStart));
    assert(WriteOffsetSize == ConvertPTSize(ReadOffsetSize));

    // See if area VB area needs updating
    if (!pAdditionalInfo->ConvertedRanges.IsRangeSet(WriteOffsetStart, WriteOffsetSize))
    {
        // Update VB area
        UpdateAdditionalStreamContent(state, pAdditionalInfo, ReadOffsetStart, ReadOffsetSize, WriteOffsetStart, WriteOffsetSize);
        pAdditionalInfo->ConvertedRanges.SetRange(WriteOffsetStart, WriteOffsetSize);
    }

    // Save old declaration
    hr = m_pDevice->GetVertexDeclaration(&m_pOldVertexDeclaration);

    // Set declaration
    hr = g_pProxyDevice->SetVertexDeclaration(pAdditionalInfo->pVertexDeclaration);

    // Set additional stream
    std::uint32_t OffsetInBytes = ConvertPTSize(state.stream1.OffsetInBytes);
    hr = m_pDevice->SetStreamSource(2, pAdditionalInfo->pStreamData, OffsetInBytes, pAdditionalInfo->Stride);
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::MaybeUnsetAdditionalVertexStream
//
//
//
///////////////////////////////////////////////////////////////
void CAdditionalVertexStreamManager::MaybeUnsetAdditionalVertexStream()
{
    HRESULT hr;
    if (!m_pOldVertexDeclaration)
        return;
    
    // Set prev declaration
    hr = g_pProxyDevice->SetVertexDeclaration(m_pOldVertexDeclaration);
    SAFE_RELEASE(m_pOldVertexDeclaration);

    // Unset additional stream
    hr = m_pDevice->SetStreamSource(2, nullptr, 0, 0);
}

/////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::UpdateAdditionalStreamContent
//
// Generate data in the additional stream
//
/////////////////////////////////////////////////////////////
bool CAdditionalVertexStreamManager::UpdateAdditionalStreamContent(SCurrentStateInfo& state,
    SAdditionalStreamInfo* pAdditionalInfo, std::uint32_t ReadOffsetStart,
    std::uint32_t ReadSize, std::uint32_t WriteOffsetStart, std::uint32_t WriteSize
) {
    // HRESULT hr;
    IDirect3DVertexBuffer9* pStreamDataPT = state.stream1.pStreamData;
    IDirect3DVertexBuffer9* pStreamDataN = pAdditionalInfo->pStreamData;
    std::uint32_t StridePT = 20;
    std::uint32_t StrideN = 12;
    std::uint32_t NumVerts = ReadSize / StridePT;
    assert(NumVerts == WriteSize / StrideN);

    // Get the source vertex bytes
    std::vector<uchar> sourceArray;
    sourceArray.resize(ReadSize);
    uchar* pSourceArrayBytes = &sourceArray[0];
    {
        void* pVertexBytesPT = nullptr;
        if (FAILED(pStreamDataPT->Lock(ReadOffsetStart, ReadSize, &pVertexBytesPT, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY)))
            return false;
        memcpy(pSourceArrayBytes, pVertexBytesPT, ReadSize);
        pStreamDataPT->Unlock();
    }

    // Create dest byte buffer
    std::vector<uchar> destArray;
    destArray.resize(WriteSize);
    uchar* pDestArrayBytes = &destArray[0];

    // Compute dest bytes
    {
        // Get index buffer
        if (FAILED(m_pDevice->GetIndices(&state.pIndexData)))
            return false;

        // Get index buffer desc
        D3DINDEXBUFFER_DESC IndexBufferDesc;
        state.pIndexData->GetDesc(&IndexBufferDesc);

        std::uint32_t numIndices = state.args.primCount + 2;
        std::uint32_t step = 1;
        if (state.args.PrimitiveType == D3DPT_TRIANGLELIST)
        {
            numIndices = state.args.primCount * 3;
            step = 3;
        }
        assert(IndexBufferDesc.Size >= (numIndices + state.args.startIndex) * 2);

        // Get index buffer data
        std::vector<uchar> indexArray;
        indexArray.resize(ReadSize);
        uchar* pIndexArrayBytes = &indexArray[0];
        {
            void* pIndexBytes = nullptr;
            if (FAILED(state.pIndexData->Lock(state.args.startIndex * 2, numIndices * 2, &pIndexBytes, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY)))
                return false;
            memcpy(pIndexArrayBytes, pIndexBytes, numIndices * 2);
            state.pIndexData->Unlock();
        }

        // Calc normals
        std::vector<CVector> NormalList;
        NormalList.insert(NormalList.end(), NumVerts, CVector());

        std::map<std::int64_t, CVector> doneTrisMap;

        // For each triangle
        for (auto i = 0; i < numIndices - 2; i += step)
        {
            // Get triangle vertex indici
            WORD v0 = ((WORD*)pIndexArrayBytes)[i];
            WORD v1 = ((WORD*)pIndexArrayBytes)[i + 1];
            WORD v2 = ((WORD*)pIndexArrayBytes)[i + 2];

            if (v0 >= NumVerts || v1 >= NumVerts || v2 >= NumVerts)
                continue;            // vert index out of range

            if (v0 == v1 || v0 == v2 || v1 == v2)
                continue;            // degenerate tri

            // Get vertex positions from original stream
            CVector* pPos0 = (CVector*)(pSourceArrayBytes + v0 * 20);
            CVector* pPos1 = (CVector*)(pSourceArrayBytes + v1 * 20);
            CVector* pPos2 = (CVector*)(pSourceArrayBytes + v2 * 20);

            // Calculate the normal
            CVector Dir1 = *pPos2 - *pPos1;
            CVector Dir2 = *pPos0 - *pPos1;

            CVector Normal = Dir1;
            Normal.CrossProduct(&Dir2);
            Normal.Normalize();

            // Flip normal if triangle was flipped
            if (state.args.PrimitiveType == D3DPT_TRIANGLESTRIP && (i & 1))
                Normal = -Normal;

            // Try to improve results by ignoring duplicated triangles
            std::int64_t key = getTriKey(v0, v1, v2);
            if (CVector* pDoneTriPrevNormal = MapFind(doneTrisMap, key))
            {
                // Already done this tri - Keep prev tri if it has a better 'up' rating
                if (pDoneTriPrevNormal->fZ > Normal.fZ)
                    continue;

                // Remove effect of prev tri
                NormalList[v0] -= *pDoneTriPrevNormal;
                NormalList[v1] -= *pDoneTriPrevNormal;
                NormalList[v2] -= *pDoneTriPrevNormal;
            }
            MapSet(doneTrisMap, key, Normal);

            // Add normal weight to used vertices
            NormalList[v0] += Normal;
            NormalList[v1] += Normal;
            NormalList[v2] += Normal;
        }

        // Validate normals and set dest data
        for (auto i = 0; i < NumVerts; i++)
        {
            // Validate
            CVector&               Normal = NormalList[i];
            static constexpr float FLOAT_EPSILON = 0.0001f;
            if (Normal.Normalize() < FLOAT_EPSILON)
                Normal = CVector(0, 0, 1);

            // Set
            CVector* pNormal = (CVector*)(pDestArrayBytes + i * 12);
            *pNormal = Normal;
        }
    }

    // Set the dest bytes
    {
        void* pVertexBytesN = nullptr;
        if (FAILED(pStreamDataN->Lock(WriteOffsetStart, WriteSize, &pVertexBytesN, D3DLOCK_NOSYSLOCK)))
            return false;
        memcpy(pVertexBytesN, pDestArrayBytes, WriteSize);
        pStreamDataN->Unlock();
    }

    return true;
}

/////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::UpdateCurrentStateInfo
//
// Get state render state setting that could be useful
//
/////////////////////////////////////////////////////////////
bool CAdditionalVertexStreamManager::UpdateCurrentStateInfo(SCurrentStateInfo& state)
{
    // Get vertex declaration
    if (FAILED(m_pDevice->GetVertexDeclaration(&state.decl.pVertexDeclaration)))
        return false;

    // Get vertex declaration desc
    if (state.decl.pVertexDeclaration)
    {
        if (FAILED(state.decl.pVertexDeclaration->GetDeclaration(state.decl.elements, &state.decl.numElements)))
            return false;
    }

    // Get vertex stream
    if (FAILED(m_pDevice->GetStreamSource(1, &state.stream1.pStreamData, &state.stream1.OffsetInBytes, &state.stream1.Stride)))
        return nullptr;

    // Get vertex stream desc
    if (state.stream1.pStreamData)
    {
        if (FAILED(state.stream1.pStreamData->GetDesc(&state.decl.VertexBufferDesc1)))
            return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::CheckCanDoThis
//
// For now, this only works if the original has 3 decl elements (0:D, 1:P, 1:T) and stream 1 has a stride of 20
//
/////////////////////////////////////////////////////////////
bool CAdditionalVertexStreamManager::CheckCanDoThis(const SCurrentStateInfo& state)
{
    if (state.decl.numElements != 4)
        return false;

    // Check vertex declaration requirements
    const D3DVERTEXELEMENT9* elements = state.decl.elements;
    if (elements[0].Stream != 0 || elements[0].Type != D3DDECLTYPE_D3DCOLOR || elements[0].Usage != D3DDECLUSAGE_COLOR)
        return false;

    if (elements[1].Stream != 1 || elements[1].Type != D3DDECLTYPE_FLOAT3 || elements[1].Usage != D3DDECLUSAGE_POSITION)
        return false;

    if (elements[2].Stream != 1 || elements[2].Type != D3DDECLTYPE_FLOAT2 || elements[2].Usage != D3DDECLUSAGE_TEXCOORD)
        return false;

    if (elements[3].Stream != 255)
        return false;

    // Check vertex stream
    if (!state.stream1.pStreamData)
        return false;

    if (state.stream1.Stride != 20)
        return false;

    if (state.args.PrimitiveType != D3DPT_TRIANGLESTRIP && state.args.PrimitiveType != D3DPT_TRIANGLELIST)
        return false;

    return true;
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::GetAdditionalStreamInfo
//
//
//
///////////////////////////////////////////////////////////////
SAdditionalStreamInfo* CAdditionalVertexStreamManager::GetAdditionalStreamInfo(IDirect3DVertexBuffer9* pStreamData1)
{
    return MapFind(m_AdditionalStreamInfoMap, pStreamData1);
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::CreateAdditionalStreamInfo
//
//
//
///////////////////////////////////////////////////////////////
SAdditionalStreamInfo* CAdditionalVertexStreamManager::CreateAdditionalStreamInfo(const SCurrentStateInfo& state)
{
    SAdditionalStreamInfo* pAdditionalInfo = MapFind(m_AdditionalStreamInfoMap, state.stream1.pStreamData);
    if (pAdditionalInfo)
        return pAdditionalInfo;
    
    // Create it
    SAdditionalStreamInfo info;

    // Create new decleration
    D3DVERTEXELEMENT9 elements[MAXD3DDECLLENGTH];
    assert(state.decl.numElements > 3 && state.decl.numElements < 5);
    memcpy(elements, state.decl.elements, state.decl.numElements * sizeof(D3DVERTEXELEMENT9));

    D3DVERTEXELEMENT9* declNew = &elements[state.decl.numElements - 1];
    elements[state.decl.numElements] = *declNew;

    declNew->Stream = 2;
    declNew->Offset = 0;
    declNew->Type = D3DDECLTYPE_FLOAT3;
    declNew->Method = D3DDECLMETHOD_DEFAULT;
    declNew->Usage = D3DDECLUSAGE_NORMAL;
    declNew->UsageIndex = 0;
    if (FAILED(m_pDevice->CreateVertexDeclaration(elements, &info.pVertexDeclaration)))
        return false;

    // Create new stream
    info.Stride = sizeof(float) * 3;
    UINT Size2 = ConvertPTSize(state.decl.VertexBufferDesc1.Size);
    if (FAILED(m_pDevice->CreateVertexBuffer(Size2, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &info.pStreamData, nullptr)))
        return false;

    // Save info
    MapSet(m_AdditionalStreamInfoMap, state.stream1.pStreamData, info);
    pAdditionalInfo = MapFind(m_AdditionalStreamInfoMap, state.stream1.pStreamData);

    return pAdditionalInfo;
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::OnVertexBufferDestroy
//
// Remove matching additional vertex buffer
//
///////////////////////////////////////////////////////////////
void CAdditionalVertexStreamManager::OnVertexBufferDestroy(IDirect3DVertexBuffer9* pStreamData1)
{
    SAdditionalStreamInfo* pAdditionalInfo = GetAdditionalStreamInfo(pStreamData1);
    if (pAdditionalInfo)
    {
        pAdditionalInfo->pStreamData->Release();
        pAdditionalInfo->pVertexDeclaration->Release();
        MapRemove(m_AdditionalStreamInfoMap, pStreamData1);
    }
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::OnVertexBufferRangeInvalidated
//
// Force recalc of matching additional vertex buffer range, when it is next used
//
///////////////////////////////////////////////////////////////
void CAdditionalVertexStreamManager::OnVertexBufferRangeInvalidated(IDirect3DVertexBuffer9* pStreamData1,
    std::uint32_t Offset, std::uint32_t Size
) {
    SAdditionalStreamInfo* pAdditionalInfo = GetAdditionalStreamInfo(pStreamData1);
    if (!pAdditionalInfo)
        return;
    
    pAdditionalInfo->ConvertedRanges.UnsetRange(Offset, Size);
}
