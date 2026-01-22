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
#include "CProxyDirect3DTexture.h"

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::CProxyDirect3DTexture
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DTexture::CProxyDirect3DTexture(IDirect3DDevice9* InD3DDevice9, IDirect3DTexture9* pOriginal, UINT Width, UINT Height, UINT Levels, DWORD Usage,
                                             D3DFORMAT Format, D3DPOOL Pool)
    : m_pStats(Usage & D3DUSAGE_DYNAMIC ? &g_StaticMemoryState.DynamicTexture : &g_StaticMemoryState.StaticTexture)
{
    m_pOriginal = pOriginal;
    m_iMemUsed = CRenderItemManager::CalcD3DTextureMemoryKBUsage(pOriginal) * 1024;
    m_dwUsage = Usage;

    m_pStats->iCurrentCount++;
    m_pStats->iCurrentBytes += m_iMemUsed;
    m_pStats->iCreatedCount++;
    m_pStats->iCreatedBytes += m_iMemUsed;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::~CProxyDirect3DTexture
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DTexture::~CProxyDirect3DTexture()
{
    if (m_pStats)
    {
        m_pStats->iCurrentCount--;
        m_pStats->iCurrentBytes -= m_iMemUsed;
        m_pStats->iDestroyedCount++;
        m_pStats->iDestroyedBytes += m_iMemUsed;
    }
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::QueryInterface
//
// Used to find the pointer to the real interface
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DTexture::QueryInterface(REFIID riid, void** ppvObj)
{
    *ppvObj = NULL;

    // Looking for me?
    if (riid == CProxyDirect3DTexture_GUID)
    {
        *ppvObj = this;
        AddRef();
        return S_OK;
    }

    return m_pOriginal->QueryInterface(riid, ppvObj);
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::Release
//
// Delete this object on final release of the original
//
/////////////////////////////////////////////////////////////
ULONG CProxyDirect3DTexture::Release()
{
    // Call original function
    ULONG count = m_pOriginal->Release();

    if (count == 0)
    {
        // now, the Original Object has deleted itself, so do we here
        delete this;            // destructor will be called automatically
    }

    return count;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::LockRect
//
// For stats
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DTexture::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
    if (m_pStats)
        m_pStats->iLockedCount++;
    return m_pOriginal->LockRect(Level, pLockedRect, pRect, Flags);
}
