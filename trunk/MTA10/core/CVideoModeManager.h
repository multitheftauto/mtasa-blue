/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVideoModeManager.h
*  PURPOSE:     Header file for the video mode manager class
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVIDEOMODEMANAGER_H
#define __CVIDEOMODEMANAGER_H

enum
{
    VIDEO_MODE_FULLSCREEN = -1,
    VIDEO_MODE_WINDOWED = 0,
};

class CVideoModeManagerInterface
{
public:
    virtual             ~CVideoModeManagerInterface ( void ) {}

    virtual void        PreCreateDevice             ( D3DPRESENT_PARAMETERS* pp ) = 0;
    virtual void        PostCreateDevice            ( IDirect3DDevice9* pD3DDevice, D3DPRESENT_PARAMETERS* pp ) = 0;
    virtual void        PreReset                    ( D3DPRESENT_PARAMETERS* pp ) = 0;
    virtual void        PostReset                   ( D3DPRESENT_PARAMETERS* pp ) = 0;
    virtual int         GetLastFullScreenVideoMode  ( void ) = 0;
    virtual void        ChangeVideoMode             ( int iMode ) = 0;
};

CVideoModeManagerInterface* GetVideoModeManager ( void );

#endif

