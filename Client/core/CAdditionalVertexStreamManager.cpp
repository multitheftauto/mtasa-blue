/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAdditionalVertexStreamManager.h"
#include "DXHook/CProxyDirect3DDevice9.h"
#include <limits>
#include <mutex>

CAdditionalVertexStreamManager* CAdditionalVertexStreamManager::ms_Singleton = nullptr;

namespace
{
    // Helper functions for this file only

    uint ConvertPTOffset(uint OffsetPT)
    {
        return (OffsetPT / 20) * 12;
    }

    // Convert size of PT stream to sizeof of N stream (rounded up to the next vertex)
    uint ConvertPTSize(uint SizePT)
    {
        uint64_t padded = static_cast<uint64_t>(SizePT) + 19ull;
        uint64_t result = (padded / 20ull) * 12ull;
        if (result > std::numeric_limits<uint>::max())
            return std::numeric_limits<uint>::max();
        return static_cast<uint>(result);
    }

    struct STriKey
    {
        uint32_t v0;
        uint32_t v1;
        uint32_t v2;

        bool operator<(const STriKey& rhs) const
        {
            if (v0 != rhs.v0)
                return v0 < rhs.v0;
            if (v1 != rhs.v1)
                return v1 < rhs.v1;
            return v2 < rhs.v2;
        }
    };

    // Get ordered key for a triangle by using the sorted vertex indices
    STriKey GetTriKey(uint32_t a, uint32_t b, uint32_t c)
    {
        uint32_t tmp;
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
        STriKey key = {a, b, c};
        return key;
    }

    std::mutex g_singletonMutex;
}  // namespace

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
    SAFE_RELEASE(m_pOldVertexDeclaration);

    for (auto iter = m_AdditionalStreamInfoMap.begin(); iter != m_AdditionalStreamInfoMap.end(); ++iter)
    {
        SAdditionalStreamInfo& info = iter->second;
        SAFE_RELEASE(info.pStreamData);
        SAFE_RELEASE(info.pVertexDeclaration);
    }

    m_AdditionalStreamInfoMap.clear();
    m_pDevice = nullptr;
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
    std::lock_guard<std::mutex> guard(g_singletonMutex);
    if (!ms_Singleton)
        ms_Singleton = new CAdditionalVertexStreamManager();
    return ms_Singleton;
}

CAdditionalVertexStreamManager* CAdditionalVertexStreamManager::GetExistingSingleton()
{
    std::lock_guard<std::mutex> guard(g_singletonMutex);
    return ms_Singleton;
}

void CAdditionalVertexStreamManager::DestroySingleton()
{
    std::lock_guard<std::mutex> guard(g_singletonMutex);
    if (!ms_Singleton)
        return;

    delete ms_Singleton;
    ms_Singleton = nullptr;
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
    if (!m_pDevice)
        return false;

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
    if (!UpdateCurrentStateInfo(state))
        return false;

    // For now, this only works if the original has 3 decl elements (0:D, 1:P, 1:T) and stream 1 has a stride of 20
    if (!CheckCanDoThis(state))
        return false;

    return SetAdditionalVertexStream(state);
}

///////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::SetAdditionalVertexStream
//
//
//
///////////////////////////////////////////////////////////////
bool CAdditionalVertexStreamManager::SetAdditionalVertexStream(SCurrentStateInfo& state)
{
    // Get matching custom N vbuffer for std PT vbuffer
    SAdditionalStreamInfo* pAdditionalInfo = GetAdditionalStreamInfo(state.stream1.pStreamData);
    if (!pAdditionalInfo)
        pAdditionalInfo = CreateAdditionalStreamInfo(state);

    if (!pAdditionalInfo)
        return false;

    uint StridePT = 20;
    uint StrideN = 12;

    const INT baseVertexIndex = state.args.BaseVertexIndex;

    const int64_t maxUint = static_cast<int64_t>(std::numeric_limits<uint>::max());
    const int64_t viMinBasedSigned = static_cast<int64_t>(state.args.MinVertexIndex) + static_cast<int64_t>(baseVertexIndex);
    if (viMinBasedSigned < 0 || viMinBasedSigned > maxUint)
        return false;

    const int64_t viMaxBasedSigned = viMinBasedSigned + static_cast<int64_t>(state.args.NumVertices);
    if (viMaxBasedSigned < viMinBasedSigned || viMaxBasedSigned > maxUint)
        return false;

    uint viMinBased = static_cast<uint>(viMinBasedSigned);
    uint viMaxBased = static_cast<uint>(viMaxBasedSigned);
    uint vertexSpan = viMaxBased - viMinBased;

    uint64_t readOffsetStart64 = static_cast<uint64_t>(viMinBased) * StridePT + state.stream1.OffsetInBytes;
    if (readOffsetStart64 > std::numeric_limits<uint>::max())
        return false;
    uint ReadOffsetStart = static_cast<uint>(readOffsetStart64);

    uint64_t readOffsetSize64 = static_cast<uint64_t>(vertexSpan) * StridePT;
    if (readOffsetSize64 > std::numeric_limits<uint>::max())
        return false;
    uint ReadOffsetSize = static_cast<uint>(readOffsetSize64);

    uint     OffsetInBytesN = ConvertPTOffset(state.stream1.OffsetInBytes);
    uint64_t writeOffsetStart64 = static_cast<uint64_t>(viMinBased) * StrideN + OffsetInBytesN;
    if (writeOffsetStart64 > std::numeric_limits<uint>::max())
        return false;
    uint WriteOffsetStart = static_cast<uint>(writeOffsetStart64);

    uint64_t writeOffsetSize64 = static_cast<uint64_t>(vertexSpan) * StrideN;
    if (writeOffsetSize64 > std::numeric_limits<uint>::max())
        return false;
    uint WriteOffsetSize = static_cast<uint>(writeOffsetSize64);

    assert(WriteOffsetStart == ConvertPTOffset(ReadOffsetStart));
    assert(WriteOffsetSize == ConvertPTSize(ReadOffsetSize));

    // See if area VB area needs updating
    if (!pAdditionalInfo->ConvertedRanges.IsRangeSet(WriteOffsetStart, WriteOffsetSize))
    {
        if (!UpdateAdditionalStreamContent(state, pAdditionalInfo, ReadOffsetStart, ReadOffsetSize, WriteOffsetStart, WriteOffsetSize))
            return false;

        pAdditionalInfo->ConvertedRanges.SetRange(WriteOffsetStart, WriteOffsetSize);
    }

    IDirect3DVertexDeclaration9* pPreviousDecl = nullptr;
    if (FAILED(m_pDevice->GetVertexDeclaration(&pPreviousDecl)))
        return false;

    CScopedActiveProxyDevice proxyDevice;
    if (!proxyDevice)
    {
        SAFE_RELEASE(pPreviousDecl);
        return false;
    }

    if (FAILED(proxyDevice->SetVertexDeclaration(pAdditionalInfo->pVertexDeclaration)))
    {
        SAFE_RELEASE(pPreviousDecl);
        return false;
    }

    uint OffsetInBytes = ConvertPTOffset(state.stream1.OffsetInBytes);
    if (FAILED(m_pDevice->SetStreamSource(2, pAdditionalInfo->pStreamData, OffsetInBytes, pAdditionalInfo->Stride)))
    {
        proxyDevice->SetVertexDeclaration(pPreviousDecl);
        SAFE_RELEASE(pPreviousDecl);
        return false;
    }

    SAFE_RELEASE(m_pOldVertexDeclaration);
    m_pOldVertexDeclaration = pPreviousDecl;
    return true;
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
    if (!m_pDevice)
        return;

    if (m_pOldVertexDeclaration)
    {
        HRESULT    hr = m_pDevice->TestCooperativeLevel();
        const bool bDeviceOperational = (hr == D3D_OK);

        if (bDeviceOperational)
        {
            // Set prev declaration
            CScopedActiveProxyDevice proxyDevice;
            if (proxyDevice)
                proxyDevice->SetVertexDeclaration(m_pOldVertexDeclaration);

            // Unset additional stream
            m_pDevice->SetStreamSource(2, nullptr, 0, 0);
        }

        SAFE_RELEASE(m_pOldVertexDeclaration);
    }
}

/////////////////////////////////////////////////////////////
//
// CAdditionalVertexStreamManager::UpdateAdditionalStreamContent
//
// Generate data in the additional stream
//
/////////////////////////////////////////////////////////////
bool CAdditionalVertexStreamManager::UpdateAdditionalStreamContent(SCurrentStateInfo& state, SAdditionalStreamInfo* pAdditionalInfo, uint ReadOffsetStart,
                                                                   uint ReadSize, uint WriteOffsetStart, uint WriteSize)
{
    // HRESULT hr;
    IDirect3DVertexBuffer9* pStreamDataPT = state.stream1.pStreamData;
    IDirect3DVertexBuffer9* pStreamDataN = pAdditionalInfo->pStreamData;
    uint                    StridePT = 20;
    uint                    StrideN = 12;
    uint                    NumVerts = ReadSize / StridePT;
    assert(NumVerts == WriteSize / StrideN);

    if (ReadSize == 0 || WriteSize == 0)
        return false;

    if ((ReadSize % StridePT) != 0)
        return false;

    if ((WriteSize % StrideN) != 0)
        return false;

    if (WriteSize != NumVerts * StrideN)
        return false;

    if (NumVerts == 0)
        return false;

    const UINT bufferSizePT = state.decl.VertexBufferDesc1.Size;
    if (ReadOffsetStart > bufferSizePT)
        return false;

    if (ReadSize > bufferSizePT - ReadOffsetStart)
        return false;

    D3DVERTEXBUFFER_DESC destDesc;
    if (FAILED(pStreamDataN->GetDesc(&destDesc)))
        return false;

    if (WriteOffsetStart > destDesc.Size)
        return false;

    if (WriteSize > destDesc.Size - WriteOffsetStart)
        return false;

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
        if (FAILED(m_pDevice->GetIndices(&state.pIndexData)) || !state.pIndexData)
            return false;

        // Get index buffer desc
        D3DINDEXBUFFER_DESC IndexBufferDesc;
        state.pIndexData->GetDesc(&IndexBufferDesc);

        uint indexStride = 0;
        if (IndexBufferDesc.Format == D3DFMT_INDEX16)
            indexStride = sizeof(WORD);
        else if (IndexBufferDesc.Format == D3DFMT_INDEX32)
            indexStride = sizeof(DWORD);
        else
            return false;

        uint numIndices = state.args.primCount + 2;
        uint step = 1;
        if (state.args.PrimitiveType == D3DPT_TRIANGLELIST)
        {
            numIndices = state.args.primCount * 3;
            step = 3;
        }

        size_t startByte = static_cast<size_t>(state.args.startIndex) * indexStride;
        size_t requiredBytes = static_cast<size_t>(numIndices) * indexStride;
        if (startByte > IndexBufferDesc.Size)
            return false;

        if (requiredBytes > IndexBufferDesc.Size - startByte)
            return false;

        std::vector<uint32_t> indices(numIndices);
        {
            void* pIndexBytes = nullptr;
            if (FAILED(
                    state.pIndexData->Lock(static_cast<UINT>(startByte), static_cast<UINT>(requiredBytes), &pIndexBytes, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY)))
                return false;

            if (indexStride == sizeof(WORD))
            {
                const WORD* pSrc = static_cast<const WORD*>(pIndexBytes);
                for (uint i = 0; i < numIndices; ++i)
                    indices[i] = pSrc[i];
            }
            else
            {
                const DWORD* pSrc = static_cast<const DWORD*>(pIndexBytes);
                for (uint i = 0; i < numIndices; ++i)
                    indices[i] = pSrc[i];
            }

            state.pIndexData->Unlock();
        }

        // Calc normals
        std::vector<CVector> NormalList;
        NormalList.insert(NormalList.end(), NumVerts, CVector());

        std::map<STriKey, CVector> doneTrisMap;

        // For each triangle
        for (uint i = 0; i < numIndices - 2; i += step)
        {
            // Get triangle vertex indici
            uint32_t v0 = indices[i];
            uint32_t v1 = indices[i + 1];
            uint32_t v2 = indices[i + 2];

            if (v0 >= NumVerts || v1 >= NumVerts || v2 >= NumVerts)
                continue;  // vert index out of range

            if (v0 == v1 || v0 == v2 || v1 == v2)
                continue;  // degenerate tri

            // Get vertex positions from original stream
            CVector* pPos0 = reinterpret_cast<CVector*>(pSourceArrayBytes + v0 * StridePT);
            CVector* pPos1 = reinterpret_cast<CVector*>(pSourceArrayBytes + v1 * StridePT);
            CVector* pPos2 = reinterpret_cast<CVector*>(pSourceArrayBytes + v2 * StridePT);

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
            STriKey key = GetTriKey(v0, v1, v2);
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
        for (uint i = 0; i < NumVerts; i++)
        {
            // Validate
            CVector&               Normal = NormalList[i];
            static constexpr float FLOAT_EPSILON = 0.0001f;
            if (Normal.Normalize() < FLOAT_EPSILON)
                Normal = CVector(0, 0, 1);

            // Set
            CVector* pNormal = reinterpret_cast<CVector*>(pDestArrayBytes + i * StrideN);
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
    if (!m_pDevice)
        return false;

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
        return false;

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
    if (!pAdditionalInfo)
    {
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
            return nullptr;

        // Create new stream
        info.Stride = sizeof(float) * 3;
        UINT Size2 = ConvertPTSize(state.decl.VertexBufferDesc1.Size);
        if (FAILED(m_pDevice->CreateVertexBuffer(Size2, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &info.pStreamData, nullptr)))
        {
            SAFE_RELEASE(info.pVertexDeclaration);
            return nullptr;
        }

        // Save info
        MapSet(m_AdditionalStreamInfoMap, state.stream1.pStreamData, info);
        pAdditionalInfo = MapFind(m_AdditionalStreamInfoMap, state.stream1.pStreamData);
    }

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
void CAdditionalVertexStreamManager::OnVertexBufferRangeInvalidated(IDirect3DVertexBuffer9* pStreamData1, uint Offset, uint Size)
{
    SAdditionalStreamInfo* pAdditionalInfo = GetAdditionalStreamInfo(pStreamData1);
    if (pAdditionalInfo)
    {
        if (Size == 0)
        {
            pAdditionalInfo->ConvertedRanges = CRanges();
            return;
        }

        uint convertedOffset = ConvertPTOffset(Offset);

        uint verticesTouched = Size / 20;
        if ((Size % 20) != 0)
            ++verticesTouched;

        if (verticesTouched == 0)
            verticesTouched = 1;

        uint64_t convertedSize64 = static_cast<uint64_t>(verticesTouched) * static_cast<uint64_t>(pAdditionalInfo->Stride);
        uint     convertedSize = convertedSize64 > std::numeric_limits<uint>::max() ? std::numeric_limits<uint>::max() : static_cast<uint>(convertedSize64);

        pAdditionalInfo->ConvertedRanges.UnsetRange(convertedOffset, convertedSize);
    }
}
