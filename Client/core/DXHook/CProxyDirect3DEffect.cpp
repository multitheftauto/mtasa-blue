/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CProxyDirect3DEffect.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProxyDirect3DEffect.h"

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DEffect::CProxyDirect3DEffect
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DEffect::CProxyDirect3DEffect(IDirect3DDevice9* InD3DDevice9, ID3DXEffect* pOriginal) : m_stats(g_pDeviceState->MemoryState.Effect)
{
    m_pOriginal = pOriginal;
    m_stats.iCurrentCount++;
    m_stats.iCreatedCount++;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DEffect::~CProxyDirect3DEffect
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DEffect::~CProxyDirect3DEffect()
{
    m_stats.iCurrentCount--;
    m_stats.iDestroyedCount++;
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DEffect::QueryInterface
//
// Used to find the pointer to the real interface
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DEffect::QueryInterface(REFIID riid, void** ppvObj)
{
    *ppvObj = NULL;

    // Looking for me?
    if (riid == CProxyDirect3DEffect_GUID)
    {
        *ppvObj = this;
        return S_OK;
    }

    return m_pOriginal->QueryInterface(riid, ppvObj);
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DEffect::Release
//
// Delete this object on final release of the original
//
/////////////////////////////////////////////////////////////
ULONG CProxyDirect3DEffect::Release()
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
// CProxyDirect3DEffect::CloneEffect
//
//
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DEffect::CloneEffect(LPDIRECT3DDEVICE9 pDevice, LPD3DXEFFECT* ppEffect)
{
    HRESULT hr = m_pOriginal->CloneEffect(pDevice, ppEffect);
    if (SUCCEEDED(hr))
    {
        // Create proxy so we can track when it's finished with
        *ppEffect = new CProxyDirect3DEffect(pDevice, *ppEffect);
    }
    return hr;
}

/////////////////////////////////////////////////////////////
//
// MyD3DXCreateEffectFromFile
//
// Wrap result of orignal function
//
/////////////////////////////////////////////////////////////
HRESULT WINAPI MyD3DXCreateEffectFromFile(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags,
                                          LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
    HRESULT hr = D3DXCreateEffectFromFile(pDevice, pSrcFile, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
    if (SUCCEEDED(hr))
    {
        // Create proxy so we can track when it's finished with
        *ppEffect = new CProxyDirect3DEffect(pDevice, *ppEffect);
    }
    return hr;
}

/////////////////////////////////////////////////////////////
//
// D3DXCreateEffect
//
// Wrap result of orignal function
//
/////////////////////////////////////////////////////////////
HRESULT WINAPI MyD3DXCreateEffect(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD Flags,
                                  LPD3DXEFFECTPOOL pPool, LPD3DXEFFECT* ppEffect, LPD3DXBUFFER* ppCompilationErrors)
{
    HRESULT hr = D3DXCreateEffect(pDevice, pSrcData, SrcDataLen, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
    if (SUCCEEDED(hr))
    {
        // Create proxy so we can track when it's finished with
        *ppEffect = new CProxyDirect3DEffect(pDevice, *ppEffect);
    }
    return hr;
}
