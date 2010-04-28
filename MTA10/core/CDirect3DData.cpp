/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DData.cpp
*  PURPOSE:     Direct3D related data storage class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

template<> CDirect3DData * CSingleton< CDirect3DData >::m_pSingleton = NULL;

CDirect3DData::CDirect3DData ( void ) 
{
    // Zero out our matricies.
    memset ( &m_mViewMatrix, 0, sizeof ( D3DMATRIX ) );
    memset ( &m_mProjMatrix, 0, sizeof ( D3DMATRIX ) );
    memset ( &m_mWorldMatrix, 0, sizeof ( D3DMATRIX ) );

    m_hDeviceWindow     = 0;
    m_dwViewportX       = 0;
    m_dwViewportY       = 0;
    m_dwViewportWidth   = 0;
    m_dwViewportHeight  = 0;
}

CDirect3DData::~CDirect3DData ( void ) 
{

}


void CDirect3DData::StoreTransform ( D3DTRANSFORMSTATETYPE dwMatrixToStore, const D3DMATRIX* pMatrix ) 
{
    switch ( dwMatrixToStore )
    {
        case D3DTS_VIEW:
            // Copy the real view matrix.
            memcpy ( &m_mViewMatrix, pMatrix, sizeof ( D3DMATRIX ) );
            break;
        case D3DTS_PROJECTION:
            // Copy the real projection marix.
            memcpy ( &m_mProjMatrix, pMatrix, sizeof ( D3DMATRIX ) );
            break;
        case D3DTS_WORLD:
            // Copy the real world matrix.
            memcpy ( &m_mWorldMatrix, pMatrix, sizeof ( D3DMATRIX ) );
            break;
        default:
            // Do nothing.
            break;
    }
}

void CDirect3DData::GetTransform ( D3DTRANSFORMSTATETYPE dwRequestedMatrix, D3DMATRIX * pMatrixOut ) 
{
    switch ( dwRequestedMatrix )
    {
        case D3DTS_VIEW:
            // Copy the stored view matrix.
            memcpy ( pMatrixOut, &m_mViewMatrix, sizeof ( D3DMATRIX ) );
            break;
        case D3DTS_PROJECTION:
            // Copy the stored projection matrix.
            memcpy ( pMatrixOut, &m_mProjMatrix, sizeof ( D3DMATRIX ) );
            break;
        case D3DTS_WORLD:
            // Copy the stored world matrix.
            memcpy ( pMatrixOut, &m_mWorldMatrix, sizeof ( D3DMATRIX ) );
            break;
        default:
            // Zero out the structure for the user.
            memcpy ( pMatrixOut, 0, sizeof ( D3DMATRIX ) );
            break;
    }

    //assert ( 0 );   // Too expensive to be used because SetTransform is used too often
}


void CDirect3DData::StoreViewport ( DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight ) 
{
    m_dwViewportX       = dwX;
    m_dwViewportY       = dwY;
    m_dwViewportWidth   = dwWidth;
    m_dwViewportHeight  = dwHeight;
}


DWORD CDirect3DData::GetViewportX ( void ) 
{
    return m_dwViewportX;
}


DWORD CDirect3DData::GetViewportY ( void ) 
{
    return m_dwViewportY;
}


DWORD CDirect3DData::GetViewportWidth ( void ) 
{
    return m_dwViewportWidth;
}


DWORD CDirect3DData::GetViewportHeight ( void ) 
{
    return m_dwViewportHeight;
}

