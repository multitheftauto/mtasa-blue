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
#include "CProxyDirect3DIndexBuffer.h"


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DIndexBuffer::CProxyDirect3DIndexBuffer
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DIndexBuffer::CProxyDirect3DIndexBuffer ( IDirect3DDevice9* InD3DDevice9,IDirect3DIndexBuffer9* pOriginal, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool )
    : m_stats ( Usage & D3DUSAGE_DYNAMIC ? g_pDeviceState->MemoryState.DynamicIndexBuffer : g_pDeviceState->MemoryState.StaticIndexBuffer )
{
	m_pOriginal = pOriginal;
    m_iMemUsed = Length;
    m_dwUsage = Usage;
    m_format = Format;
    m_pool = Pool;

    m_stats.iCurrentCount++;
    m_stats.iCurrentBytes += m_iMemUsed;
    m_stats.iCreatedCount++;
    m_stats.iCreatedBytes += m_iMemUsed;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DIndexBuffer::~CProxyDirect3DIndexBuffer
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DIndexBuffer::~CProxyDirect3DIndexBuffer ( void )
{
    m_stats.iCurrentCount--;
    m_stats.iCurrentBytes -= m_iMemUsed;
    m_stats.iDestroyedCount++;
    m_stats.iDestroyedBytes += m_iMemUsed;
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DIndexBuffer::QueryInterface
//
// Used to find the pointer to the real interface
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DIndexBuffer::QueryInterface ( REFIID riid, void** ppvObj )
{
	*ppvObj = NULL;

    // Looking for me?
	if( riid == CProxyDirect3DIndexBuffer_GUID )
	{
		*ppvObj = this;
		return S_OK;
	}

	return m_pOriginal->QueryInterface ( riid, ppvObj ); 
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DIndexBuffer::Release
//
// Delete this object on final release of the original
//
/////////////////////////////////////////////////////////////
ULONG CProxyDirect3DIndexBuffer::Release ( void )
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
// CProxyDirect3DIndexBuffer::Lock
//
// For stats
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DIndexBuffer::Lock ( UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags )
{
    m_stats.iLockedCount++;

    *ppbData = NULL;
    HRESULT hr = m_pOriginal->Lock ( OffsetToLock, SizeToLock, ppbData, Flags);

    if( FAILED( hr ) )
    {
        SString strMessage( "Lock IndexBuffer fail: hr:%x Length:%x Usage:%x Format:%x Pool:%x OffsetToLock:%x SizeToLock:%x Flags:%x"
                                                        , hr, m_iMemUsed, m_dwUsage, m_format, m_pool, OffsetToLock, SizeToLock, Flags );
        WriteDebugEvent( strMessage );
        AddReportLog( 8625, strMessage );
        CCore::GetSingleton ().LogEvent ( 625, "Lock IndexBuffer", "", strMessage );
    }
    else
    if ( *ppbData == NULL )
    {
        SString strMessage( "Lock IndexBuffer result NULL: hr:%x Length:%x Usage:%x Format:%x Pool:%x OffsetToLock:%x SizeToLock:%x Flags:%x"
                                                        , hr, m_iMemUsed, m_dwUsage, m_format, m_pool, OffsetToLock, SizeToLock, Flags );
        WriteDebugEvent( strMessage );
        AddReportLog( 8626, strMessage );
        CCore::GetSingleton ().LogEvent ( 626, "Lock IndexBuffer NULL", "", strMessage );
    }
    return hr;
}
