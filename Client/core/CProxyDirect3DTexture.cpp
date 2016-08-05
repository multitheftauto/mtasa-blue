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
#include "CProxyDirect3DTexture.h"


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::CProxyDirect3DTexture
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DTexture::CProxyDirect3DTexture ( IDirect3DDevice9* InD3DDevice9,IDirect3DTexture9* pOriginal, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool )
    : m_stats ( Usage & D3DUSAGE_DYNAMIC ? g_pDeviceState->MemoryState.DynamicTexture : g_pDeviceState->MemoryState.StaticTexture )
{
	m_pOriginal = pOriginal;
    m_iMemUsed = CRenderItemManager::CalcD3DTextureMemoryKBUsage ( pOriginal ) * 1024;
    m_dwUsage = Usage;

    m_stats.iCurrentCount++;
    m_stats.iCurrentBytes += m_iMemUsed;
    m_stats.iCreatedCount++;
    m_stats.iCreatedBytes += m_iMemUsed;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::~CProxyDirect3DTexture
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DTexture::~CProxyDirect3DTexture ( void )
{
    m_stats.iCurrentCount--;
    m_stats.iCurrentBytes -= m_iMemUsed;
    m_stats.iDestroyedCount++;
    m_stats.iDestroyedBytes += m_iMemUsed;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::QueryInterface
//
// Used to find the pointer to the real interface
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DTexture::QueryInterface ( REFIID riid, void** ppvObj )
{
	*ppvObj = NULL;

    // Looking for me?
	if( riid == CProxyDirect3DTexture_GUID )
	{
		*ppvObj = this;
		return S_OK;
	}

	return m_pOriginal->QueryInterface ( riid, ppvObj ); 
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DTexture::Release
//
// Delete this object on final release of the original
//
/////////////////////////////////////////////////////////////
ULONG CProxyDirect3DTexture::Release ( void )
{
	// Call original function
	ULONG count = m_pOriginal->Release ();
		
    if ( count == 0 )
    {
		// now, the Original Object has deleted itself, so do we here
		delete this ;  // destructor will be called automatically
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
HRESULT CProxyDirect3DTexture::LockRect ( UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
    m_stats.iLockedCount++;
    return m_pOriginal->LockRect ( Level, pLockedRect, pRect, Flags );
}
