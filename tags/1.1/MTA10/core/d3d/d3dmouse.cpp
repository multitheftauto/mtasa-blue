/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/d3d/d3dmouse.cpp
*  PURPOSE:     Direct3D mouse handler
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "include/d3dmouse.h"


CD3DMouse::CD3DMouse()
{
    m_meLastEvent = D3DMEVENT_NONE;
    m_pRenderer = NULL;
    m_pMouseTexture = NULL;
    m_usXPosition = 0;
    m_usYPosition = 0;
    m_uiViewWidth = 0;
    m_uiViewHeight = 0;
    m_dwClearColor = D3DCOLOR_ARGB(255,255,255,255);
}

CD3DMouse::~CD3DMouse()
{
    SAFE_RELEASE(m_pMouseTexture)
    m_meLastEvent = D3DMEVENT_NONE;
}

HRESULT CD3DMouse::ProcessWindowMessages(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
        OnMouseLButtonEvent(true);
        return true;
    case WM_LBUTTONUP:
        OnMouseLButtonEvent(false);
        return true;
    case WM_RBUTTONDOWN:
        OnMouseRButtonEvent(true);
        return true;
    case WM_RBUTTONUP:
        OnMouseRButtonEvent(false);
        return true;
    case WM_MOUSEMOVE: 
        OnMouseMoveEvent(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
        return true;
    default:
        return false;
    }
}

D3DM_ERROR CD3DMouse::RenderMouse()
{
    D3DSURFACE_DESC desc;
    D3DXVECTOR2 vectScale, vectTrans;


    if (m_pMouseTexture == NULL || m_pRenderer == NULL) 
    {
        return D3DMERROR_RENDER;
    }


    m_pMouseTexture->GetLevelDesc(0,&desc);

    vectScale.x = m_uiViewWidth/640.0f/2;
    vectScale.y = m_uiViewHeight/480.0f/2;
    vectTrans.x = (float)m_usXPosition;
    vectTrans.y = (float)m_usYPosition;

    m_pRenderer->Render2DSprite(m_pMouseTexture,&vectScale,&vectTrans,m_dwClearColor);

    return D3DMERROR_NONE;
}

VOID CD3DMouse::OnLostDevice()
{
    if (m_pRenderer) m_pRenderer->OnInvalidateDevice();
}

VOID CD3DMouse::OnRestoreDevice()
{
    if (m_pRenderer) m_pRenderer->OnRestoreDevice();
}

D3DM_ERROR CD3DMouse::Initialize(IDirect3DDevice8 *pd3dptr, UINT uiWidth, UINT uiHeight, PBYTE pbFileAddress, DWORD dwSize)
{
    m_uiViewWidth = uiWidth;
    m_uiViewHeight = uiHeight;

    if ((m_pD3DDevice = pd3dptr) == NULL)
    {
        return D3DMERROR_INVALIDDEVICE;
    }

    if ((m_pRenderer = new CD3DMGEng(m_pD3DDevice)) == NULL)
    {
        return D3DMERROR_SPRITEFAILED;
    }

    if (D3DXCreateTextureFromFileInMemory(m_pD3DDevice, pbFileAddress, dwSize, &m_pMouseTexture) != D3D_OK)
    {
        return D3DMERROR_TEXTUREFAILED;
    }

    return D3DMERROR_NONE;
}

D3DM_ERROR CD3DMouse::Initialize(IDirect3DDevice8 *pd3dptr, UINT uiWidth, UINT uiHeight, LPCTSTR wszFileName)
{
    m_uiViewWidth = uiWidth;
    m_uiViewHeight = uiHeight;

    if ((m_pD3DDevice = pd3dptr) == NULL)
    {
        return D3DMERROR_INVALIDDEVICE;
    }

    if ((m_pRenderer = new CD3DMGEng(m_pD3DDevice)) == NULL)
    {
        return D3DMERROR_SPRITEFAILED;
    }

    if (D3DXCreateTextureFromFile(m_pD3DDevice, wszFileName, &m_pMouseTexture) != D3D_OK)
    {
        return D3DMERROR_TEXTUREFAILED;
    }

    return D3DMERROR_NONE;
}

VOID CD3DMouse::OnMouseLButtonEvent(BOOL bButtonDown)
{
    if (bButtonDown) //Lbutton down event
    {
        // I don't like it changing colour? Does the windows cursor do that? No.
    //  m_dwClearColor = D3DCOLOR_ARGB(220,255,182,193);
        SetLastEvent(D3DMEVENT_LBUTTONDN);
    }
    else  //Lbutton up event
    {
    //  m_dwClearColor = D3DCOLOR_ARGB(255,255,255,255);
        SetLastEvent(D3DMEVENT_LBUTTONUP);
    }
}

VOID CD3DMouse::OnMouseRButtonEvent(BOOL bButtonDown)
{
    if (bButtonDown)  //Rbutton down event
    {
        SetLastEvent(D3DMEVENT_RBUTTONDN);
    }
    else  //Rbutton up event
    {
        SetLastEvent(D3DMEVENT_RBUTTONUP);
    }
}


