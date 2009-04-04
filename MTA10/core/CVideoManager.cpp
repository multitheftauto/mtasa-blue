/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVideoManager.cpp
*  PURPOSE:     DirectShow video renderer manager
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::string;
using std::list;

template<> CVideoManager * CSingleton< CVideoManager >::m_pSingleton = NULL;

// Codec paths
#define CORE_MTA_CODEC_XVID		"xv.ax"
#define CORE_MTA_CODEC_VORBIS	"vorbis.ax"
#define CORE_MTA_SPLITTER_MKV	"matroska.ax"

// DllGetClassObject function typedef
typedef HRESULT ( WINAPI * pfnDllGetClassObject ) ( REFCLSID rclsid, REFIID riid, LPVOID * ppv );

CVideoManager::CVideoManager ( void )
{
}

CVideoManager::~CVideoManager ( void )
{
	DestroyRenderers ();
	CoUninitialize ();
}

bool bOnce = false;
bool CVideoManager::CreateRenderer ( CVideoRenderer** ppRenderer, const char *szFile )
{
	if ( !bOnce ) {
		CCore::GetSingleton ().GetConsole ()->Printf ( "Loading codecs" );

		bOnce = true;

		// Load the matroska splitter
		if ( LoadCodec ( CLSID_SOURCE, CORE_MTA_SPLITTER_MKV, &m_pFactorySource ) != S_OK ) return false;

		// Load the audio codec
		if ( LoadCodec ( CLSID_VORBIS, CORE_MTA_CODEC_VORBIS, &m_pFactoryVorbis ) != S_OK ) return false;

		// Load the video codec
		if ( LoadCodec ( CLSID_XVID, CORE_MTA_CODEC_XVID, &m_pFactoryXVID ) != S_OK ) return false;
	}

	// Get the appropriate Direct3D device
	IDirect3DDevice9 * pDevice = CGraphics::GetSingleton ().GetDevice ();

	// Create the new renderer
	CVideoRenderer * pRenderer = new CVideoRenderer ( pDevice );

	// Initialize the renderer
	if ( pRenderer->Initialize ( szFile ) ) {
		// And add it if the initialisation was successful
		m_List.push_back ( pRenderer );

		(*ppRenderer) = pRenderer;
		return true;
	}
	(*ppRenderer) = NULL;
	return false;
}

void CVideoManager::DestroyRenderer ( CVideoRenderer* pRenderer )
{
	m_List.remove ( pRenderer );
	delete pRenderer;
}

void CVideoManager::DestroyRenderers ( void )
{
    list < CVideoRenderer* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
		delete *iter;
	}
	m_List.clear ();
}

void CVideoManager::OnLostDevice ( void )
{
    list < CVideoRenderer* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
		(*iter)->OnLostDevice ();
	}
}

void CVideoManager::OnResetDevice ( IDirect3DDevice9* pDevice )
{
    list < CVideoRenderer* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
		(*iter)->OnResetDevice ( pDevice );
	}
}

HRESULT CVideoManager::LoadCodec ( REFCLSID ClassID, const char * szPath, IClassFactory ** ppFactory )
{
	IClassFactory *pFactory = NULL;

	HINSTANCE hAx = LoadLibrary ( szPath );
	if ( hAx == NULL ) return E_FAIL;

	// Get the load function proc address
	pfnDllGetClassObject DllGetClassObject = (pfnDllGetClassObject)GetProcAddress ( hAx, "DllGetClassObject" );
	if ( DllGetClassObject == NULL ) return E_FAIL;

	// Get the class object
	if ( DllGetClassObject ( ClassID, IID_IClassFactory, (LPVOID*) ppFactory ) != S_OK ) return E_FAIL;
	
	return S_OK;
}

HRESULT CVideoManager::CreateCodec ( REFCLSID ClassID, IClassFactory * pFactory, IBaseFilter ** ppFilter )
{
	if ( pFactory == NULL ) return E_FAIL;

	// Create an instance
	if ( pFactory->CreateInstance ( NULL, IID_IBaseFilter, (LPVOID*) ppFilter ) != S_OK ) return E_FAIL;
	if ( (*ppFilter) == NULL ) return E_FAIL;

	return S_OK;
}
