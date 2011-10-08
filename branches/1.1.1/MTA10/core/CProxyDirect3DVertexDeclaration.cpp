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
#include "CProxyDirect3DVertexDeclaration.h"


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexDeclaration::CProxyDirect3DVertexDeclaration
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DVertexDeclaration::CProxyDirect3DVertexDeclaration ( IDirect3DDevice9* InD3DDevice9,IDirect3DVertexDeclaration9* pOriginal, CONST D3DVERTEXELEMENT9* pVertexElements )
{
	m_pOriginal = pOriginal;

    CProxyDirect3DDevice9::SD3DVertexDeclState info;

    // Calc and cache info
    for ( uint i = 0 ; i < MAXD3DDECLLENGTH ; i++ )
    {
        const D3DVERTEXELEMENT9& element = pVertexElements[ i ];
        if ( element.Stream == 0xFF )
            break;

        switch ( element.Stream)
        {
            case 0:        info.bUsesStreamAtIndex[0] = true;  break;
            case 1:        info.bUsesStreamAtIndex[1] = true;  break;
        }

        switch ( element.Usage + element.UsageIndex * 16 )
        {
            case D3DDECLUSAGE_POSITION:         info.Position = 1;   break;
            case D3DDECLUSAGE_POSITIONT:        info.PositionT = 1;  break;
            case D3DDECLUSAGE_NORMAL:           info.Normal = 1;     break;
            case D3DDECLUSAGE_COLOR:            info.Color0 = 1;     break;
            case D3DDECLUSAGE_COLOR + 16:       info.Color1 = 1;     break;
            case D3DDECLUSAGE_TEXCOORD:         info.TexCoord0 = 1;  break;
            case D3DDECLUSAGE_TEXCOORD + 16:    info.TexCoord1 = 1;  break;
        }
    }

    // Add to cached info map
    MapSet ( g_pProxyDevice->m_VertexDeclMap, this, info );
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexDeclaration::~CProxyDirect3DVertexDeclaration
//
//
//
/////////////////////////////////////////////////////////////
CProxyDirect3DVertexDeclaration::~CProxyDirect3DVertexDeclaration ( void )
{
    // Remove from cached info map
    MapRemove ( g_pProxyDevice->m_VertexDeclMap, this );
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexDeclaration::QueryInterface
//
// Used to find the pointer to the real vertex buffer when setting streams
//
/////////////////////////////////////////////////////////////
HRESULT CProxyDirect3DVertexDeclaration::QueryInterface ( REFIID riid, void** ppvObj )
{
	*ppvObj = NULL;

    // Looking for me?
	if( riid == CProxyDirect3DVertexDeclaration_GUID )
	{
		*ppvObj = this;
		return S_OK;
	}

	return m_pOriginal->QueryInterface ( riid, ppvObj ); 
}


/////////////////////////////////////////////////////////////
//
// CProxyDirect3DVertexDeclaration::Release
//
// Delete this object on final release of the original
//
/////////////////////////////////////////////////////////////
ULONG CProxyDirect3DVertexDeclaration::Release ( void )
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
