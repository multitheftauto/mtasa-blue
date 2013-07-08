#include "../Common.h"
#include "CVideoRenderer.h"
#include "dshowutil.h"

#include <dshow.h>
#include <uuids.h>
#include <vmr9.h>
#include <vector>

CVideoRenderer::CVideoRenderer ( IDirect3DDevice9* pDevice )
{
	InitializeCriticalSection ( &m_Lock );
	Lock ();

	m_pGraph = NULL;
	m_pFilter = NULL;
	m_pMediaControl = NULL;
	m_pMediaSeeking = NULL;
	m_pVideoTexture = NULL;
	m_dwGraphRegister = 0;
	m_pDevice = pDevice;
	m_lVideoWidth = m_lVideoHeight = 0;

	Unlock ();
}

CVideoRenderer::~CVideoRenderer ()
{
	// --- stop the video playback
	Stop ();

#ifdef _DEBUG
	// --- unregister the graph
    if( m_dwGraphRegister )
    {
        RemoveGraphFromRot( m_dwGraphRegister );
    }
#endif

	Lock ();

	// --- release our resources
	SAFE_RELEASE( m_pBasicAudio );
	SAFE_RELEASE( m_pMediaControl );
	SAFE_RELEASE( m_pMediaSeeking );

	SAFE_RELEASE ( m_pGraph );
	SAFE_RELEASE ( m_pFilter );

	// ACHTUNG: free codec instances

	Unlock ();

	DeleteCriticalSection ( &m_Lock );
}

IDirect3DTexture9 * CVideoRenderer::GetVideoTexture ( void )
{
	if ( m_pVideoTexture ) {
		m_pVideoTexture->AddRef ();
		return m_pVideoTexture;
	}
	return NULL;
}

void CVideoRenderer::GetVideoFormat( long & lWidth, long & lHeight )
{
	lWidth = m_lVideoWidth;
	lHeight = m_lVideoHeight;
}

void CVideoRenderer::SetVideoFormat( const long lWidth, const long lHeight )
{
	m_lVideoWidth = lWidth;
	m_lVideoHeight = lHeight;
}

bool CVideoRenderer::Initialize ( const WCHAR * fileName )
{
	HRESULT hr;
	#define EIF( _x ) if( FAILED( hr=_x ) ) goto error

	// --- check for a valid device
	if ( !m_pDevice ) return false;

	Lock ();

	// --- create the filter graph
	EIF( CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGraph ) );
	m_pFilter = CreateTextureRenderer ( &hr, m_pDevice, this );
	EIF( hr );

	// --- add our texture filter and set up the graph to render to this filter
	m_pGraph->AddFilter( m_pFilter,	L"Texture renderer" );

	EIF( RenderFileToVideoRenderer( m_pGraph, fileName, FALSE ) );

#ifdef _DEBUG
	// --- register this graph for use with graphedt
	if( FAILED( hr = AddGraphToRot( m_pGraph, &m_dwGraphRegister ) ) ) {
		// ACHTUNG: failure, report and move on
	}
#endif

	// --- get some interfaces
	m_pGraph->QueryInterface( IID_IMediaSeeking, (void**)&m_pMediaSeeking );
	m_pGraph->QueryInterface( IID_IMediaControl, (void**)&m_pMediaControl );
	m_pGraph->QueryInterface( IID_IBasicAudio, (void**)&m_pBasicAudio );

	m_pBasicAudio->get_Volume ( &lDefaultVolume );

	// --- clean up
	m_pGraph->Release ();
	m_pGraph = NULL;

	// --- unlock the mutex
	Unlock ();

	return true;

error:
	// --- failure occurred
	hr = hr;

	return false;
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
	// --- wait until this control has stopped the video, if any
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
	LONGLONG Duration = 0L;
	if ( m_pMediaSeeking )
		HRESULT hr = m_pMediaSeeking->SetPositions ( &pCurrent, AM_SEEKING_AbsolutePositioning, &Duration, AM_SEEKING_NoPositioning );
}
