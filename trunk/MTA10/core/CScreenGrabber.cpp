/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CScreenGrabber.cpp
*  PURPOSE:
*
*****************************************************************************/

#include "StdInc.h"
#include "CCompressorJobQueue.h"

struct SScreenShotQueueItem
{
    uint uiSizeX;
    uint uiSizeY;
    uint uiQuality;
    uint uiTimeQueued;
    PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback;
};

///////////////////////////////////////////////////////////////
//
// CScreenGrabber
//
//
///////////////////////////////////////////////////////////////
class CScreenGrabber : public CScreenGrabberInterface
{
public:
    ZERO_ON_NEW
                                CScreenGrabber                  ( void );
                                ~CScreenGrabber                 ( void );

    // CScreenGrabberInterface
    virtual void                OnDeviceCreate                  ( IDirect3DDevice9* pDevice );
    virtual void                OnLostDevice                    ( void );
    virtual void                OnResetDevice                   ( void );
    virtual void                DoPulse                         ( void );
    virtual void                QueueScreenShot                 ( uint uiSizeX, uint uiSizeY, uint uiQuality, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback );
    virtual void                ClearScreenShotQueue            ( void );
    virtual bool                GetBackBufferPixels             ( uint uiSizeX, uint uiSizeY, CBuffer& buffer, SString& strOutError );

    // CScreenGrabber
    void                        ProcessScreenShotQueue          ( void );

protected:
    IDirect3DDevice9*                       m_pDevice;
    CCompressorJobQueue*                    m_pCompressorJobQueue;
    CCompressJobData*                       m_pCompressJobData;
    CRenderTargetItem*                      m_pScreenShotTemp;
    std::list < SScreenShotQueueItem >      m_ScreenShotQueue;
};


///////////////////////////////////////////////////////////////
// Object creation
///////////////////////////////////////////////////////////////
CScreenGrabberInterface* NewScreenGrabber ( void )
{
    return new CScreenGrabber ();
}



////////////////////////////////////////////////////////////////
//
// CScreenGrabber::CScreenGrabber
//
//
//
////////////////////////////////////////////////////////////////
CScreenGrabber::CScreenGrabber ( void )
{
    m_pCompressorJobQueue = NewCompressorJobQueue ();
}


////////////////////////////////////////////////////////////////
//
// CScreenGrabber::~CScreenGrabber
//
//
//
////////////////////////////////////////////////////////////////
CScreenGrabber::~CScreenGrabber ( void )
{
    SAFE_DELETE( m_pCompressorJobQueue );
}


////////////////////////////////////////////////////////////////
//
// CScreenGrabber::OnDeviceCreate
//
//
//
////////////////////////////////////////////////////////////////
void CScreenGrabber::OnDeviceCreate ( IDirect3DDevice9* pDevice )
{
    m_pDevice = pDevice;
}


////////////////////////////////////////////////////////////////
//
// CScreenGrabber::OnLostDevice
//
//
//
////////////////////////////////////////////////////////////////
void CScreenGrabber::OnLostDevice ( void )
{
    SAFE_RELEASE( m_pScreenShotTemp );
}


////////////////////////////////////////////////////////////////
//
// CScreenGrabber::OnResetDevice
//
//
//
////////////////////////////////////////////////////////////////
void CScreenGrabber::OnResetDevice ( void )
{
}


////////////////////////////////////////////////////////////////
//
// CScreenGrabber::DoPulse
//
//
//
////////////////////////////////////////////////////////////////
void CScreenGrabber::DoPulse ( void )
{
    m_pCompressorJobQueue->DoPulse ();
    ProcessScreenShotQueue ();
}


////////////////////////////////////////////////////////////////
//
// CScreenGrabber::QueueScreenShot
//
// Add a screen shot request
//
////////////////////////////////////////////////////////////////
void CScreenGrabber::QueueScreenShot ( uint uiSizeX, uint uiSizeY, uint uiQuality, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback )
{
    SScreenShotQueueItem item;
    item.uiSizeX = uiSizeX;
    item.uiSizeY = uiSizeY;
    item.uiQuality = uiQuality;
    item.uiTimeQueued = GetTickCount32 ();
    item.pfnScreenShotCallback = pfnScreenShotCallback;
    m_ScreenShotQueue.push_back ( item );
}


////////////////////////////////////////////////////////////////
//
// CScreenGrabber::ProcessScreenShotQueue
//
// Process queued requests
//
////////////////////////////////////////////////////////////////
void CScreenGrabber::ProcessScreenShotQueue ( void )
{
    // Check if busy
    if ( m_pCompressJobData )
    {
        // Previous job complete?
        if ( !m_pCompressorJobQueue->PollCommand ( m_pCompressJobData, 0 ) )
            return;

        // Compression done
        if ( m_pCompressJobData->HasCallback () )
            m_pCompressJobData->ProcessCallback ();
        m_pCompressJobData = NULL;
    }

    // Anything new?
    if ( m_ScreenShotQueue.empty ()  )
        return;

    // Get new args
    SScreenShotQueueItem item = m_ScreenShotQueue.front ();
    uint uiSizeX = item.uiSizeX;
    uint uiSizeY = item.uiSizeY;
    uint uiQuality = item.uiQuality;
    uint uiTimeSpentInQueue = GetTickCount32 () - item.uiTimeQueued;
    PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback = item.pfnScreenShotCallback;
    m_ScreenShotQueue.pop_front ();

    CBuffer buffer;
    SString strError;
    if ( GetBackBufferPixels ( uiSizeX, uiSizeY, buffer, strError ) )
    {
        // Start compression
        m_pCompressJobData = m_pCompressorJobQueue->AddCommand ( uiSizeX, uiSizeY, uiQuality, uiTimeSpentInQueue, pfnScreenShotCallback, buffer );
    }
    else
    {
        // Pass on error
        pfnScreenShotCallback( buffer, 0, strError );
    }
}


////////////////////////////////////////////////////////////////
//
// CScreenGrabber::GetBackBufferPixels
//
//
//
////////////////////////////////////////////////////////////////
bool CScreenGrabber::GetBackBufferPixels ( uint uiSizeX, uint uiSizeY, CBuffer& buffer, SString& strOutError )
{
    HRESULT hr;

    // Try to get the back buffer
    IDirect3DSurface9* pD3DBackBufferSurface = NULL;
    hr = m_pDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pD3DBackBufferSurface );
    if ( FAILED( hr ) )
    {
        strOutError = SString( "GetBackBuffer failed (0x%08x)", hr );
        return false;
    }

    // Adjust/create screenshot target size
    if ( !m_pScreenShotTemp || m_pScreenShotTemp->m_uiSizeX != uiSizeX || m_pScreenShotTemp->m_uiSizeY != uiSizeY )
    {
        // Delete old one if it exists
        SAFE_RELEASE( m_pScreenShotTemp );

        // Try to create new one if needed
        if ( uiSizeX > 0 )
            m_pScreenShotTemp = CGraphics::GetSingleton ().GetRenderItemManager ()->CreateRenderTarget ( uiSizeX, uiSizeY, false, true );
    }

    if ( !m_pScreenShotTemp )
    {
        strOutError = "No ScreenShotTemp";
        return false;
    }


    // Copy back buffer into our private render target
    D3DTEXTUREFILTERTYPE FilterType = D3DTEXF_LINEAR;
    hr = m_pDevice->StretchRect( pD3DBackBufferSurface, NULL, m_pScreenShotTemp->m_pD3DRenderTargetSurface, NULL, FilterType );
    if ( FAILED( hr ) )
    {
        strOutError = SString( "StretchRect failed (0x%08x)", hr );
        return false;
    }

    // Clean up
    SAFE_RELEASE( pD3DBackBufferSurface );

    if ( !m_pScreenShotTemp->ReadPixels ( buffer, strOutError ) )
    {
        dassert( !strOutError.empty() );
        return false;
    }

    return true;
}


////////////////////////////////////////////////////////////////
//
// CScreenGrabber::ClearScreenShotQueue
//
// Remove queued requests and finish current job
//
////////////////////////////////////////////////////////////////
void CScreenGrabber::ClearScreenShotQueue ( void )
{
    m_ScreenShotQueue.clear ();

    if ( m_pCompressJobData )
        m_pCompressorJobQueue->PollCommand ( m_pCompressJobData, -1 );
    m_pCompressJobData = NULL;
}
