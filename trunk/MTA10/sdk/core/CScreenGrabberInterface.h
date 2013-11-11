/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CScreenGrabber.h
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

typedef void (*PFN_SCREENSHOT_CALLBACK) ( const CBuffer& buffer, uint uiTimeSpentInQueue, const SString& strError );

//
// CScreenGrabberInterface
//
class CScreenGrabberInterface
{
public:
    ZERO_ON_NEW

    virtual                     ~CScreenGrabberInterface        ( void ) {}

    // CScreenGrabberInterface
    virtual void                OnDeviceCreate                  ( IDirect3DDevice9* pDevice ) = 0;
    virtual void                OnLostDevice                    ( void ) = 0;
    virtual void                OnResetDevice                   ( void ) = 0;
    virtual void                DoPulse                         ( void ) = 0;
    virtual void                QueueScreenShot                 ( uint uiSizeX, uint uiSizeY, uint uiQuality, PFN_SCREENSHOT_CALLBACK pfnScreenShotCallback ) = 0;
    virtual void                ClearScreenShotQueue            ( void ) = 0;
    virtual bool                GetBackBufferPixels             ( uint uiSizeX, uint uiSizeY, CBuffer& buffer, SString& strOutError ) = 0;
};

CScreenGrabberInterface* NewScreenGrabber ( void );
