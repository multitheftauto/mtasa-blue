/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVideoRenderer.h
*  PURPOSE:     Header file for DirectShow to Direct3D video renderer class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CVideoRenderer;

#ifndef __CVIDEORENDERER_H
#define __CVIDEORENDERER_H

#include "CGraphics.h"
#include "texturerenderer/ITextureRenderer.h"
#include "texturerenderer/IVideoRenderer.h"

#include <dshow.h>
#include <uuids.h>
#include <vmr9.h>
#include <vector>

class CVideoRenderer : public IVideoRenderer
{
    friend class CVideoManager;

public:
    // Our methods
    IDirect3DTexture9 *                     GetVideoTexture         ( void );
    void                                    SetVideoTexture         ( IDirect3DTexture9 * pTexture );

    void                                    Lock                    ( void ) { EnterCriticalSection ( &m_Lock ); }
    void                                    Unlock                  ( void ) { LeaveCriticalSection ( &m_Lock ); }

    LPCRITICAL_SECTION                      GetMutex                ( void );

    void                                    Play                    ( void );
    void                                    Stop                    ( void );

    void                                    Mute                    ( void );
    void                                    Unmute                  ( void );

    void                                    SetPosition             ( LONGLONG pCurrent );
    inline void                             GetPositions            ( LONGLONG& pCurrent, LONGLONG& pDuration ) { m_pMediaSeeking->GetPositions ( &pCurrent, &pDuration ); }

    void                                    OnLostDevice            ( void );
    void                                    OnResetDevice           ( IDirect3DDevice9* pDevice );

protected:
                                            CVideoRenderer          ( IDirect3DDevice9* pDevice );
    virtual                                 ~CVideoRenderer         ( void );

    bool                                    Initialize              ( const char *szFile );

private:
    IDirect3DDevice9 *              m_pDevice;
    IDirect3DTexture9 *             m_pVideoTexture;

    CRITICAL_SECTION                m_Lock;

    IGraphBuilder *                 m_pGraph;
    IBaseFilter *                   m_pFilter;
    IMediaControl *                 m_pMediaControl;
    IMediaSeeking *                 m_pMediaSeeking;
    IBasicAudio *                   m_pBasicAudio;

    long                            lDefaultVolume;
};

#endif
