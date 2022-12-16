/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientGifDisplay.h"
#include "CClientGif.h"

CClientGifDisplay::CClientGifDisplay(CClientDisplayManager* pDisplayManager, CClientGif* pGif, int ID)
    : CClientDisplay(pDisplayManager, ID)
{
    m_pGif = pGif;
    m_bVisible = true;
    m_bIsCleared = false;
}

void CClientGifDisplay::Render(){
    if (!m_pGif || m_pGif->IsDestoryed()){
        return;
    }
    if (!m_bVisible){
        if (!IsCleared()){
            ClearTexture();
        }
        return;
    }else if (IsCleared()){
        m_bIsCleared = false;
    }
    if (m_pGif->IsPlaying()) {
        if ((double)GetTickCount64_() >= m_pGif->GetTick() + m_pGif->GetFrameDelay(-1)) {
            m_pGif->Next();
            m_pGif->UpdateTick();
            UpdateTexture();
        }
    }
}

void CClientGifDisplay::UpdateTexture(){
    if (!m_pGif || m_pGif->IsDestoryed()){
        return;
    }
    CGifItem* pGifItem = m_pGif->GetRenderItem();
    if (!pGifItem){
        return;
    }
    IDirect3DSurface9* surface = pGifItem->m_pD3DRenderTargetSurface;
    if (!surface){
        return;
    }
    IDirect3DDevice9* device = pGifItem->m_pDevice;
    // Lock Surface
    D3DLOCKED_RECT LockedRect;
    ZeroMemory(&LockedRect, sizeof(D3DLOCKED_RECT));
    surface->LockRect(&LockedRect, nullptr, 0);
    // Unlock Surface
    unsigned char* frame = m_pGif->GetFrame();
    if (frame){
        uint stride = m_pGif->GetStride();
        auto surfaceData = static_cast<byte*>(LockedRect.pBits);
        auto data = static_cast<const byte*>(frame);
        for (int i = 0; i < pGifItem->m_uiSizeY; i++) {
            memcpy(surfaceData,data,stride);
            data += stride;
            surfaceData += LockedRect.Pitch;
        }
    }
    surface->UnlockRect();
}

void CClientGifDisplay::ClearTexture(){
    if (!m_pGif || m_pGif->IsDestoryed()){
        return;
    }
    CGifItem* pGif = m_pGif->GetRenderItem();
    if (!pGif){
        return;
    }
    IDirect3DSurface9* surface = pGif->m_pD3DRenderTargetSurface;
    if (!surface){
        return;
    }
    IDirect3DDevice9* device = pGif->m_pDevice;
    // Lock Surface
    D3DLOCKED_RECT LockedRect;
    surface->LockRect(&LockedRect, nullptr, 0);
    device->ColorFill(surface, nullptr, D3DCOLOR_RGBA(0, 0, 0, 0));
    // Unlock Surface
    surface->UnlockRect();
    m_bIsCleared = true;
}
