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
#include "CProxyDirect3DVertexBuffer.h"
#include "CAdditionalVertexStreamManager.h"
#include "CVertexStreamBoundingBoxManager.h"


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::CProxyDirect3DVertexBuffer
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DVertexBuffer::CProxyDirect3DVertexBuffer ( IDirect3DDevice9* InD3DDevice9,IDirect3DVertexBuffer9* pOriginal, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool )
    : m_stats ( Usage & D3DUSAGE_DYNAMIC ? g_pDeviceState->MemoryState.DynamicVertexBuffer : g_pDeviceState->MemoryState.StaticVertexBuffer )
{
	m_pOriginal = pOriginal;
    m_iMemUsed = Length;
    m_dwUsage = Usage;

    m_stats.iCurrentCount++;
    m_stats.iCurrentBytes += m_iMemUsed;
    m_stats.iCreatedCount++;
    m_stats.iCreatedBytes += m_iMemUsed;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::~CProxyDirect3DVertexBuffer
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DVertexBuffer::~CProxyDirect3DVertexBuffer ( void )
{
    CAdditionalVertexStreamManager::GetSingleton ()->OnVertexBufferDestroy ( m_pOriginal );
    CVertexStreamBoundingBoxManager::GetSingleton ()->OnVertexBufferDestroy ( m_pOriginal );

    m_stats.iCurrentCount--;
    m_stats.iCurrentBytes -= m_iMemUsed;
    m_stats.iDestroyedCount++;
    m_stats.iDestroyedBytes += m_iMemUsed;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::QueryInterface
//
// Used to find the pointer to the real interface
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DVertexBuffer::QueryInterface ( REFIID riid, void** ppvObj )
{
	*ppvObj = NULL;

    // Looking for me?
	if( riid == CProxyDirect3DVertexBuffer_GUID )
	{
		*ppvObj = this;
		return S_OK;
	}

	return m_pOriginal->QueryInterface ( riid, ppvObj ); 
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexBuffer::Release
//
// Delete this object on final release of the original
//
/////////////////////////////////////////////////////////////
ULONG CProxyDirect3DVertexBuffer::Release ( void )
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
// CProxyDirect3DVertexBuffer::Lock
//
// If lock is writable, tell manager that range content will change
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DVertexBuffer::Lock ( UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags )
{
    m_stats.iLockedCount++;

    if ( ( Flags & D3DLOCK_READONLY ) == 0 )
    {
        CAdditionalVertexStreamManager::GetSingleton ()->OnVertexBufferRangeInvalidated ( m_pOriginal, OffsetToLock, SizeToLock );
        CVertexStreamBoundingBoxManager::GetSingleton ()->OnVertexBufferRangeInvalidated ( m_pOriginal, OffsetToLock, SizeToLock );
    }
    return m_pOriginal->Lock ( OffsetToLock, SizeToLock, ppbData, Flags );
}
