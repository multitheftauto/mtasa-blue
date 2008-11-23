/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVideoRenderer.cpp
*  PURPOSE:     DirectShow to Direct3D render target video renderer
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CVideoUtils.h"

CVideoRenderer::CVideoRenderer ( IDirect3DDevice9* pDevice )
{
	InitializeCriticalSection ( &m_Lock );
	Lock ();

	// Copy the Direct3D9 device
	m_pDevice = pDevice;

	// Reset the pointers
	m_pGraph = NULL;
	m_pFilter = NULL;
	m_pMediaControl = NULL;
	m_pMediaSeeking = NULL;
	m_pVideoTexture = NULL;

	Unlock ();
}


CVideoRenderer::~CVideoRenderer ()
{
	// Stop the video playback
	Stop ();

	Lock ();

	// Release our resources
	SAFE_RELEASE ( m_pGraph );
	SAFE_RELEASE ( m_pFilter );

	Unlock ();

	DeleteCriticalSection ( &m_Lock );
}


IDirect3DTexture9 * CVideoRenderer::GetVideoTexture ( void )
{
	// Check for a valid video texture
	if ( m_pVideoTexture ) {
		// Add a reference
		m_pVideoTexture->AddRef ();

		return m_pVideoTexture;
	}
	return NULL;
}


bool CVideoRenderer::Initialize ( const char * szFile )
{
	IBaseFilter * pDSound, * pXVID, * pVorbis;
	IBaseFilter * pSource;
	IFileSourceFilter * pFileSource;
	HRESULT hr;

	// Get the codecs
	CVideoManager *pManager = CVideoManager::GetSingletonPtr ();
	if ( pManager->CreateCodecSource ( &pSource ) != S_OK ) return false;
	if ( pManager->CreateCodecVorbis ( &pVorbis ) != S_OK ) return false;
	if ( pManager->CreateCodecXVID ( &pXVID ) != S_OK ) return false;

	// Check for a valid device
	if ( !m_pDevice ) return false;

	// Lock so we don't fuck up
	Lock ();

	CCore::GetSingleton ().GetConsole ()->Printf ( "Creating DirectShow graph instance" );

	// Initialize the graph builder
	CoCreateInstance ( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, reinterpret_cast < void** > ( &m_pGraph ) );
	if ( m_pGraph == NULL ) return false;

	CCore::GetSingleton ().GetConsole ()->Printf ( "Creating DirectSound renderer instance" );

	// Initialize the DirectSound filter
	CoCreateInstance ( CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, reinterpret_cast < void** > ( &pDSound ) );
	if ( pDSound == NULL ) return false;

#ifdef MTA_DEBUG
	CCore::GetSingleton ().GetConsole ()->Printf ( "Adding ROT for debug stuff" );

	// Enable GraphView debugging
	AddToROT(m_pGraph);
#endif

	CCore::GetSingleton ().GetConsole ()->Printf ( "Creating video renderer instance" );

	// Create an instance of the texture renderer and add it to the graph
	m_pFilter = CreateTextureRenderer ( &hr, m_pDevice, this );
	if ( hr != S_OK ) return false;

	// Add the source file filter to the grap h
	int iBufferSize = MultiByteToWideChar ( CP_ACP, 0, szFile, -1, NULL, 0 );
	wchar_t *wszFile = new wchar_t[iBufferSize];
	MultiByteToWideChar ( CP_ACP, 0, szFile, -1, wszFile, iBufferSize );

	CCore::GetSingleton ().GetConsole ()->Printf ( "Registering filter (Matroska)" );

	// Add the filters to the graph
	m_pGraph->AddFilter			( pSource,		L"[MTA] MKV source" );
	
	CCore::GetSingleton ().GetConsole ()->Printf ( "Loading video file" );

	pSource->QueryInterface ( IID_IFileSourceFilter, reinterpret_cast < void** > ( &pFileSource ) );
	if ( pFileSource->Load ( wszFile, NULL ) != S_OK ) return false;

	CCore::GetSingleton ().GetConsole ()->Printf ( "Registering filter (Output)" );

	m_pGraph->AddFilter			( m_pFilter,	L"[MTA] Texture renderer" );
	CCore::GetSingleton ().GetConsole ()->Printf ( "Registering filter (Vorbis)" );
	m_pGraph->AddFilter			( pVorbis,		L"[MTA] Vorbis decoder" );
	CCore::GetSingleton ().GetConsole ()->Printf ( "Registering filter (XVID)" );
	m_pGraph->AddFilter			( pXVID,		L"[MTA] XVID codec" );
	CCore::GetSingleton ().GetConsole ()->Printf ( "Registering filter (DirectSound)" );
	m_pGraph->AddFilter			( pDSound,		L"[MTA] DirectSound renderer" );

	CCore::GetSingleton ().GetConsole ()->Printf ( "Connecting video renderer" );

	// Connect the video pins
	IPin *pOut, *pSourceOut;
	hr = ConnectFilters ( m_pGraph, pSource, pXVID, &pSourceOut );		// MKV Source		-> XVID
	assert ( hr == S_OK );
	hr = ConnectFilters ( m_pGraph, pXVID, m_pFilter, &pOut );			// XVID				-> Texture Renderer
	assert ( hr == S_OK );

	// Connect the audio pins (not necessary)
	hr = ConnectFilters ( m_pGraph, pSource, pVorbis, &pOut );			// MKV Source		-> Vorbis Decoder
	hr = ConnectFilters ( m_pGraph, pVorbis, pDSound, &pOut );			// Vorbis Decoder	-> DirectSound renderer

	m_pGraph->QueryInterface ( IID_IMediaSeeking, reinterpret_cast < void** > ( &m_pMediaSeeking ) );
	assert ( m_pMediaSeeking != NULL );
    m_pGraph->QueryInterface ( IID_IMediaControl, reinterpret_cast < void** > ( &m_pMediaControl ) );
	if ( m_pMediaControl == NULL || m_pMediaSeeking == NULL ) return false;

	m_pGraph->QueryInterface ( IID_IBasicAudio, reinterpret_cast < void** > ( &m_pBasicAudio ) );
	if ( m_pBasicAudio == NULL ) return false;

	CCore::GetSingleton ().GetConsole ()->Printf ( "Successfully loaded video renderer" );

	m_pBasicAudio->get_Volume ( &lDefaultVolume );

	// Clean up
	delete [] wszFile;
//	m_pGraph->Release ();

	// Unlock the mutex
	Unlock ();

	return true;
}

void CVideoRenderer::Mute ( void )
{
	if ( m_pBasicAudio )
		m_pBasicAudio->put_Volume ( -10000 );
}

void CVideoRenderer::Unmute ( void )
{
	if ( m_pBasicAudio )
		m_pBasicAudio->put_Volume ( lDefaultVolume );
}

void CVideoRenderer::Play ( void )
{
    #ifdef MTA_DEBUG
	OutputDebugString("CVideoRenderer::Play");
    #endif

	Lock ();

	if ( m_pMediaControl != NULL )
	{
	    m_pMediaControl->Run ();

        OAFilterState state;
        do {
            m_pMediaControl->GetState(0, & state );
        } while( state != State_Running ) ;
	}

	Unlock ();
}

void CVideoRenderer::Stop ( void )
{
    #ifdef MTA_DEBUG
	OutputDebugString("CVideoRenderer::Stop");
    #endif

	// Wait until this control has stopped the video, if any
    if( m_pMediaControl != NULL ) 
    {
        OAFilterState state;
        do {
            m_pMediaControl->Stop();
            m_pMediaControl->GetState(0, & state );
        } while( state != State_Stopped ) ;
    }
}

void CVideoRenderer::OnLostDevice ( void )
{
	return;
}

void CVideoRenderer::OnResetDevice ( IDirect3DDevice9 * pDevice )
{
	return;
}

void CVideoRenderer::SetVideoTexture ( IDirect3DTexture9 * pTexture )
{
	m_pVideoTexture = pTexture;
}

LPCRITICAL_SECTION CVideoRenderer::GetMutex ( void )
{
	return &m_Lock;
}

void CVideoRenderer::SetPosition ( LONGLONG pCurrent )
{
    #ifdef MTA_DEBUG
	OutputDebugString("CVideoRenderer::SetPosition");
    #endif

	LONGLONG Duration = 0L;
	if ( m_pMediaSeeking )
		HRESULT hr = m_pMediaSeeking->SetPositions ( &pCurrent, AM_SEEKING_AbsolutePositioning, &Duration, AM_SEEKING_NoPositioning );
}
