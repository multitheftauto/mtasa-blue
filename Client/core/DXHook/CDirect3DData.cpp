/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDirect3DData.cpp
 *  PURPOSE:     Direct3D related data storage class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using namespace std;

template <>
CDirect3DData* CSingleton<CDirect3DData>::m_pSingleton = NULL;

CDirect3DData::CDirect3DData()
    : m_mViewMatrix{}
    , m_mProjMatrix{}
    , m_mWorldMatrix{}
    , m_hDeviceWindow(0)
    , m_dwViewportX(0)
    , m_dwViewportY(0)
    , m_dwViewportWidth(0)
    , m_dwViewportHeight(0)
{

}

CDirect3DData::~CDirect3DData()
{
}

void CDirect3DData::StoreTransform(D3DTRANSFORMSTATETYPE dwMatrixToStore, const D3DMATRIX* pMatrix)
{
    if (!pMatrix)
        return;

    // Use direct assignment instead of memcpy for better performance
    switch (dwMatrixToStore)
    {
        case D3DTS_VIEW:
            m_mViewMatrix = *pMatrix;
            break;
        case D3DTS_PROJECTION:
            m_mProjMatrix = *pMatrix;
            break;
        case D3DTS_WORLD:
            m_mWorldMatrix = *pMatrix;
            break;
        default:
            // Do nothing for unsupported transforms
            break;
    }
}

void CDirect3DData::GetTransform(D3DTRANSFORMSTATETYPE dwRequestedMatrix, D3DMATRIX* pMatrixOut)
{
    if (!pMatrixOut)
        return;

    // Use direct assignment instead of memcpy for better performance
    switch (dwRequestedMatrix)
    {
        case D3DTS_VIEW:
            *pMatrixOut = m_mViewMatrix;
            break;
        case D3DTS_PROJECTION:
            *pMatrixOut = m_mProjMatrix;
            break;
        case D3DTS_WORLD:
            *pMatrixOut = m_mWorldMatrix;
            break;
        default:
            // Zero out for unsupported transforms
            *pMatrixOut = {};
            break;
    }
}

const D3DMATRIX* CDirect3DData::GetTransformPtr(D3DTRANSFORMSTATETYPE dwRequestedMatrix) const
{
    // Return direct pointer to cached matrix - avoids copy overhead
    switch (dwRequestedMatrix)
    {
        case D3DTS_VIEW:
            return &m_mViewMatrix;
        case D3DTS_PROJECTION:
            return &m_mProjMatrix;
        case D3DTS_WORLD:
            return &m_mWorldMatrix;
        default:
            return nullptr;
    }
}

void CDirect3DData::StoreViewport(DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
    m_dwViewportX = dwX;
    m_dwViewportY = dwY;
    m_dwViewportWidth = dwWidth;
    m_dwViewportHeight = dwHeight;
}

DWORD CDirect3DData::GetViewportX()
{
    return m_dwViewportX;
}

DWORD CDirect3DData::GetViewportY()
{
    return m_dwViewportY;
}

DWORD CDirect3DData::GetViewportWidth()
{
    return m_dwViewportWidth;
}

DWORD CDirect3DData::GetViewportHeight()
{
    return m_dwViewportHeight;
}
