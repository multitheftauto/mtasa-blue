/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDirect3DData.h
 *  PURPOSE:     Header file for Direct3D data class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <d3d9.h>
#include "CSingleton.h"

class CDirect3DData : public CSingleton<CDirect3DData>
{
public:
    CDirect3DData();
    ~CDirect3DData();

    void StoreTransform(D3DTRANSFORMSTATETYPE dwMatrixToStore, const D3DMATRIX* pMatrix);
    void GetTransform(D3DTRANSFORMSTATETYPE dwRequestedMatrix, D3DMATRIX* pMatrixOut);

    HWND GetDeviceWindow() { return m_hDeviceWindow; };
    void StoreDeviceWindow(HWND hDeviceWindow) { m_hDeviceWindow = hDeviceWindow; };

    void StoreViewport(DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);

    DWORD GetViewportX();
    DWORD GetViewportY();
    DWORD GetViewportWidth();
    DWORD GetViewportHeight();

private:
    D3DMATRIX m_mViewMatrix;
    D3DMATRIX m_mProjMatrix;
    D3DMATRIX m_mWorldMatrix;

    HWND m_hDeviceWindow;

    DWORD m_dwViewportX;
    DWORD m_dwViewportY;
    DWORD m_dwViewportWidth;
    DWORD m_dwViewportHeight;
};
