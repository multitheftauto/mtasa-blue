/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:  D3DOCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CVertexStreamBoundingBoxManager.h"

CVertexStreamBoundingBoxManager* CVertexStreamBoundingBoxManager::ms_Singleton = NULL;

namespace
{
    // Helper function
    float GetBoxDistanceSq ( const CVector& vecPosition, const CVector& vecBoxCenter, const float* fExtent, const CVector** vecBoxAxes )
    {
        CVector vecOffset = vecPosition - vecBoxCenter;
        float fDistSq = 0.f;

        // For each axis
        for ( int i = 0 ; i < 3 ; i++ )
        {
            // Project vecOffset on the axis
            float fDot = vecOffset.DotProduct ( vecBoxAxes[i] );

            // Add any distance outside the box on that axis
            if ( fDot < -fExtent[i] )
                fDistSq += ( fDot + fExtent[i] ) * ( fDot + fExtent[i] );
            else
            if ( fDot > fExtent[i] )
                fDistSq += ( fDot - fExtent[i] ) * ( fDot - fExtent[i] );
        }

        return fDistSq;
    }
}

///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::CVertexStreamBoundingBoxManager
//
//
//
///////////////////////////////////////////////////////////////
CVertexStreamBoundingBoxManager::CVertexStreamBoundingBoxManager ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::~CVertexStreamBoundingBoxManager
//
//
//
///////////////////////////////////////////////////////////////
CVertexStreamBoundingBoxManager::~CVertexStreamBoundingBoxManager ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::GetSingleton
//
// Static function
//
///////////////////////////////////////////////////////////////
CVertexStreamBoundingBoxManager* CVertexStreamBoundingBoxManager::GetSingleton ( void )
{
    if ( !ms_Singleton )
        ms_Singleton = new CVertexStreamBoundingBoxManager ();;
    return ms_Singleton;
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::OnDeviceCreate
//
//
//
///////////////////////////////////////////////////////////////
void CVertexStreamBoundingBoxManager::OnDeviceCreate ( IDirect3DDevice9* pDevice )
{
    m_pDevice = pDevice;
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::GetDistanceSqToGeometry
//
//
//
///////////////////////////////////////////////////////////////
float CVertexStreamBoundingBoxManager::GetDistanceSqToGeometry ( D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount )
{
    // Cache info
    SCurrentStateInfo2 state;

    // Save call arguments
    state.args.PrimitiveType = PrimitiveType;
    state.args.BaseVertexIndex = BaseVertexIndex;
    state.args.MinVertexIndex = MinVertexIndex;
    state.args.NumVertices = NumVertices;
    state.args.startIndex = startIndex;
    state.args.primCount = primCount;
  
    // Check can do this
    if ( !CheckCanDoThis ( state ) )
        return 0.f;

    CBox boundingBox;
    if ( !GetVertexStreamBoundingBox ( state, boundingBox ) )
        return 0.f;

    return CalcDistanceSq ( state, boundingBox );
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::CalcDistanceSq
//
// Calculate distance squared from the camera to the bounding box
//
///////////////////////////////////////////////////////////////
float CVertexStreamBoundingBoxManager::CalcDistanceSq ( const SCurrentStateInfo2& state, const CBox& boundingBox )
{
    // Get camera pos
    const D3DXMATRIX& matWorld = g_pDeviceState->TransformState.WORLD;
    const D3DXMATRIX& matView = g_pDeviceState->TransformState.VIEW;

    D3DXMATRIX matViewInv;
    D3DXMatrixInverse ( &matViewInv, NULL, &matView );
    const CVector& vecCamPos = (CVector&)matViewInv.m[3][0];

    // Get object rotation
    const CVector& vecObjRight = (CVector&)matWorld.m[0][0];
    const CVector& vecObjFwd   = (CVector&)matWorld.m[1][0];
    const CVector& vecObjUp    = (CVector&)matWorld.m[2][0];
    const CVector* vecBoxAxes[3] = { &vecObjRight, &vecObjFwd, &vecObjUp };

    // Adjust for off-center bounding box
    const CVector boxExtents = ( boundingBox.vecMax - boundingBox.vecMin ) * 0.5f;
    const CVector boxOrigin  = ( boundingBox.vecMax + boundingBox.vecMin ) * 0.5f;

    D3DXVECTOR4 vecResult;
    D3DXVec3Transform ( &vecResult, (D3DXVECTOR3*)&boxOrigin, &matWorld );

    // World position of the bounding box center
    const CVector& vecObjPos = (CVector&)vecResult;

    return GetBoxDistanceSq ( vecObjPos, vecCamPos, &boxExtents.fX, vecBoxAxes );
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::GetVertexStreamBoundingBox
//
//
//
///////////////////////////////////////////////////////////////
bool CVertexStreamBoundingBoxManager::GetVertexStreamBoundingBox ( SCurrentStateInfo2& state, CBox& outBoundingBox )
{
    // Get boundsinfo for this vertex buffer
    SStreamBoundsInfo* pStreamBoundsInfo = GetStreamBoundsInfo ( state.stream.pStreamData );
    if ( !pStreamBoundsInfo )
        pStreamBoundsInfo = CreateStreamBoundsInfo ( state );

    // Calc area we are going to use
    WORD viMinBased = state.args.MinVertexIndex + state.args.BaseVertexIndex;
    WORD viMaxBased = state.args.MinVertexIndex + state.args.NumVertices + state.args.BaseVertexIndex;

    uint ReadOffsetStart = viMinBased * state.stream.Stride + state.stream.OffsetInBytes;
    uint ReadOffsetSize = ( viMaxBased - viMinBased ) * state.stream.Stride;

    // See if area VB area needs updating
    if ( !pStreamBoundsInfo->ConvertedRanges.IsRangeSet ( ReadOffsetStart, ReadOffsetSize, outBoundingBox ) )
    {
        // Update area from VB area
        if ( !ComputeVertexStreamBoundingBox ( state, ReadOffsetStart, ReadOffsetSize, outBoundingBox ) )
            return false;

        // Store vertex range result
        pStreamBoundsInfo->ConvertedRanges.SetRange ( ReadOffsetStart, ReadOffsetSize, outBoundingBox );
    }

    return true;
}


/////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::ComputeVertexStreamBoundingBox
//
// Measure used vertices
//
/////////////////////////////////////////////////////////////
bool CVertexStreamBoundingBoxManager::ComputeVertexStreamBoundingBox ( SCurrentStateInfo2& state, uint ReadOffsetStart, uint ReadSize, CBox& outBoundingBox )
{
    IDirect3DVertexBuffer9* pStreamDataPT = state.stream.pStreamData;

    const uint StridePT = state.stream.Stride;

    uint NumVerts = ReadSize / StridePT;

    // Adjust for the offset in the stream
    ReadOffsetStart += state.stream.elementOffset;
    ReadSize -= state.stream.elementOffset;
    if ( ReadSize < 1 )
        return false;

    // Get the source vertex bytes
    std::vector < uchar > sourceArray;
    sourceArray.resize ( ReadSize );
    uchar* pSourceArrayBytes = &sourceArray[0];
    {
        void* pVertexBytesPT = NULL;
        if ( FAILED( pStreamDataPT->Lock ( ReadOffsetStart, ReadSize, &pVertexBytesPT, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY ) ) )
            return false;
        memcpy ( pSourceArrayBytes, pVertexBytesPT, ReadSize );
        pStreamDataPT->Unlock ();
    }

    // Compute bounds
    {
        // Get index data
        if ( FAILED( m_pDevice->GetIndices( &state.pIndexData ) ) )
            return false;

        // Get index buffer desc
        D3DINDEXBUFFER_DESC IndexBufferDesc;
        state.pIndexData->GetDesc ( &IndexBufferDesc );

        uint numIndices = state.args.primCount + 2;
        uint step = 1;
        if ( state.args.PrimitiveType == D3DPT_TRIANGLELIST )
        {
            numIndices = state.args.primCount * 3;
            step = 3;
        }
        assert ( IndexBufferDesc.Size >= ( numIndices + state.args.startIndex ) * 2 );

        // Get index buffer data
        std::vector < uchar > indexArray;
        indexArray.resize ( numIndices*2 );
        uchar* pIndexArrayBytes = &indexArray[0];
        {
            void* pIndexBytes = NULL;
            if ( FAILED( state.pIndexData->Lock ( state.args.startIndex*2, numIndices*2, &pIndexBytes, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY ) ) )
                return false;
            memcpy ( pIndexArrayBytes, pIndexBytes, numIndices*2 );
            state.pIndexData->Unlock ();
        }

        CVector& vecMin = outBoundingBox.vecMin;
        CVector& vecMax = outBoundingBox.vecMax;
        vecMin = CVector ( 9999, 9999, 9999 );
        vecMax = CVector ( -9999, -9999, -9999 );

        // For each triangle
        for ( uint i = 0 ; i < numIndices - 2 ; i += step )
        {
            // Get triangle vertex indici
            WORD v0 = ((WORD*)pIndexArrayBytes)[ i ];
            WORD v1 = ((WORD*)pIndexArrayBytes)[ i + 1 ];
            WORD v2 = ((WORD*)pIndexArrayBytes)[ i + 2 ];

            if ( v0 >= NumVerts || v1 >= NumVerts || v2 >= NumVerts )
                continue;   // vert index out of range

            if ( v0 == v1 || v0 == v2 || v1 == v2 )
                continue;   // degenerate tri

            // Get vertex positions from original stream
            CVector* pPos0 = (CVector*)( pSourceArrayBytes + v0 * StridePT );
            CVector* pPos1 = (CVector*)( pSourceArrayBytes + v1 * StridePT );
            CVector* pPos2 = (CVector*)( pSourceArrayBytes + v2 * StridePT );

            // Update min/max
            vecMin.fX = Min ( vecMin.fX, pPos0->fX );
            vecMin.fY = Min ( vecMin.fY, pPos0->fY );
            vecMin.fZ = Min ( vecMin.fZ, pPos0->fZ );
            vecMax.fX = Max ( vecMax.fX, pPos0->fX );
            vecMax.fY = Max ( vecMax.fY, pPos0->fY );
            vecMax.fZ = Max ( vecMax.fZ, pPos0->fZ );

            vecMin.fX = Min ( vecMin.fX, pPos1->fX );
            vecMin.fY = Min ( vecMin.fY, pPos1->fY );
            vecMin.fZ = Min ( vecMin.fZ, pPos1->fZ );
            vecMax.fX = Max ( vecMax.fX, pPos1->fX );
            vecMax.fY = Max ( vecMax.fY, pPos1->fY );
            vecMax.fZ = Max ( vecMax.fZ, pPos1->fZ );

            vecMin.fX = Min ( vecMin.fX, pPos2->fX );
            vecMin.fY = Min ( vecMin.fY, pPos2->fY );
            vecMin.fZ = Min ( vecMin.fZ, pPos2->fZ );
            vecMax.fX = Max ( vecMax.fX, pPos2->fX );
            vecMax.fY = Max ( vecMax.fY, pPos2->fY );
            vecMax.fZ = Max ( vecMax.fZ, pPos2->fZ );
        }
    }

    return true;
}


/////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::CheckCanDoThis
//
// Get state info and check can do this
//
/////////////////////////////////////////////////////////////
bool CVertexStreamBoundingBoxManager::CheckCanDoThis ( SCurrentStateInfo2& state )
{
    // Only tri-lists and tri-strips
    if ( state.args.PrimitiveType != D3DPT_TRIANGLESTRIP && state.args.PrimitiveType != D3DPT_TRIANGLELIST )
        return false;

    // Get vertex declaration
    if ( FAILED( m_pDevice->GetVertexDeclaration ( &state.decl.pVertexDeclaration ) ) )
        return false;

    // Get vertex declaration desc
    if ( state.decl.pVertexDeclaration )
    {
        if ( FAILED( state.decl.pVertexDeclaration->GetDeclaration ( state.decl.elements, &state.decl.numElements ) ) )
            return false;
    }

    // Find position element
    for ( uint i = 0 ; i < state.decl.numElements ; i++ )
    {
        const D3DVERTEXELEMENT9& element = state.decl.elements[i];

        if ( element.Usage == D3DDECLUSAGE_POSITION && element.Type == D3DDECLTYPE_FLOAT3 )
        {
            // Found it
            state.stream.elementOffset = element.Offset;

            // Get relevant vertex stream
            if ( FAILED( m_pDevice->GetStreamSource ( element.Stream, &state.stream.pStreamData, &state.stream.OffsetInBytes, &state.stream.Stride ) ) )
                return false;

            // Get vertex stream desc
            if ( !state.stream.pStreamData )
                return false;

            if ( FAILED( state.stream.pStreamData->GetDesc ( &state.decl.VertexBufferDesc ) ) )
                return false;

            if ( state.decl.VertexBufferDesc.Usage & D3DUSAGE_WRITEONLY )
                return false;

            return true;
        }
    }

    return false;
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::GetStreamBoundsInfo
//
//
//
///////////////////////////////////////////////////////////////
SStreamBoundsInfo* CVertexStreamBoundingBoxManager::GetStreamBoundsInfo ( IDirect3DVertexBuffer9* pStreamData )
{
    return MapFind ( m_StreamBoundsInfoMap, pStreamData );
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::CreateStreamBoundsInfo
//
//
//
///////////////////////////////////////////////////////////////
SStreamBoundsInfo* CVertexStreamBoundingBoxManager::CreateStreamBoundsInfo ( const SCurrentStateInfo2& state )
{
    SStreamBoundsInfo* pStreamBoundsInfo = MapFind ( m_StreamBoundsInfoMap, state.stream.pStreamData );
    if ( !pStreamBoundsInfo )
    {
        // Create it
        SStreamBoundsInfo info;

        // Save info
        MapSet ( m_StreamBoundsInfoMap, state.stream.pStreamData, info );
        pStreamBoundsInfo = MapFind ( m_StreamBoundsInfoMap, state.stream.pStreamData );
    }

    return pStreamBoundsInfo;
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::OnVertexBufferDestroy
//
// Remove vertex buffer info
//
///////////////////////////////////////////////////////////////
void CVertexStreamBoundingBoxManager::OnVertexBufferDestroy ( IDirect3DVertexBuffer9* pStreamData )
{
    SStreamBoundsInfo* pStreamBoundsInfo = GetStreamBoundsInfo ( pStreamData );
    if ( pStreamBoundsInfo )
        MapRemove ( m_StreamBoundsInfoMap, pStreamData );
}


///////////////////////////////////////////////////////////////
//
// CVertexStreamBoundingBoxManager::OnVertexBufferRangeInvalidated
//
// Force recalc of vertex buffer range when it is next used
//
///////////////////////////////////////////////////////////////
void CVertexStreamBoundingBoxManager::OnVertexBufferRangeInvalidated ( IDirect3DVertexBuffer9* pStreamData, uint Offset, uint Size )
{
    SStreamBoundsInfo* pStreamBoundsInfo = GetStreamBoundsInfo ( pStreamData );
    if ( pStreamBoundsInfo )
        pStreamBoundsInfo->ConvertedRanges.UnsetRange ( Offset, Size );
}
