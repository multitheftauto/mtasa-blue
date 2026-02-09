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
#include "ComPtrValidation.h"

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DEffect::CProxyDirect3DEffect
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DEffect::CProxyDirect3DEffect(IDirect3DDevice9* InD3DDevice9, ID3DXEffect* pOriginal)
    : m_pStats(&g_StaticMemoryState.Effect), m_pOriginal(pOriginal), m_lRefCount(1)
{
    m_pStats->iCurrentCount++;
    m_pStats->iCreatedCount++;
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
    if (m_pStats)
    {
        m_pStats->iCurrentCount--;
        m_pStats->iDestroyedCount++;
    }
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
        AddRef();
        return S_OK;
    }

    if (!IsOriginalValid())
        return E_NOINTERFACE;

    return m_pOriginal->QueryInterface(riid, ppvObj);
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DEffect::IsOriginalValid
//
// Check if the underlying ID3DXEffect pointer is still valid.
// Uses COM pointer validation to detect freed/invalid objects.
//
/////////////////////////////////////////////////////////////
bool CProxyDirect3DEffect::IsOriginalValid() const
{
    if (!m_pOriginal)
        return false;

    return IsValidComInterfacePointer(m_pOriginal);
}

/////////////////////////////////////////////////////////////
//
// CProxyDirect3DEffect::AddRef
//
// Increment reference count. Validates the original pointer
// before forwarding to avoid crashes on invalid COM objects.
//
/////////////////////////////////////////////////////////////
ULONG CProxyDirect3DEffect::AddRef()
{
    LONG newRefCount = m_lRefCount.fetch_add(1, std::memory_order_relaxed) + 1;

    if (IsOriginalValid())
    {
        m_pOriginal->AddRef();
    }
    else
    {
        // Original is invalid (e.g., d3d9on12 crash) - null it to prevent
        // dangling pointer access from inline methods
        m_pOriginal = nullptr;
    }

    return static_cast<ULONG>(newRefCount);
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
    LONG newRefCount = m_lRefCount.fetch_sub(1, std::memory_order_acq_rel) - 1;

    // Handle underflow
    if (newRefCount < 0)
    {
        m_lRefCount.store(0, std::memory_order_release);
        newRefCount = 0;
    }

    // Release the original if valid
    if (IsOriginalValid())
    {
        m_pOriginal->Release();
    }
    else
    {
        // Original is invalid (e.g., d3d9on12 crash) - null it to prevent
        // dangling pointer access from inline methods
        m_pOriginal = nullptr;
    }

    // Destroy proxy when refcount reaches 0
    if (newRefCount == 0)
    {
        delete this;
    }

    return static_cast<ULONG>(newRefCount);
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
    if (!IsOriginalValid())
        return E_FAIL;

    HRESULT hr = m_pOriginal->CloneEffect(pDevice, ppEffect);
    if (SUCCEEDED(hr) && ppEffect && *ppEffect)
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
