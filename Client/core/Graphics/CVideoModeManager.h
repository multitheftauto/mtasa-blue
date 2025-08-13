/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CVideoModeManager.h
 *  PURPOSE:     Header file for the video mode manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

enum
{
    VIDEO_MODE_FULLSCREEN = -1,
    VIDEO_MODE_WINDOWED = 0,
};

class CVideoModeManagerInterface
{
public:
    virtual ~CVideoModeManagerInterface() {}

    virtual void PreCreateDevice(D3DPRESENT_PARAMETERS* pp) = 0;
    virtual void PostCreateDevice(IDirect3DDevice9* pD3DDevice, D3DPRESENT_PARAMETERS* pp) = 0;
    virtual void PreReset(D3DPRESENT_PARAMETERS* pp) = 0;
    virtual void PostReset(D3DPRESENT_PARAMETERS* pp) = 0;
    virtual void GetNextVideoMode(int& iOutNextVideoMode, bool& bOutNextWindowed, bool& bOutNextFullScreenMinimize, int& iNextFullscreenStyle) = 0;
    virtual bool SetVideoMode(int nextVideoMode, bool bNextWindowed, bool bNextFullScreenMinimize, int iNextFullscreenStyle) = 0;
    virtual bool IsWindowed() = 0;
    virtual bool IsMultiMonitor() = 0;
    virtual bool IsMinimizeEnabled() = 0;
    virtual void OnGainFocus() = 0;
    virtual void OnLoseFocus() = 0;
    virtual void OnPaint() = 0;
    virtual bool GetRequiredDisplayResolution(int& iOutWidth, int& iOutHeight, int& iOutColorBits, int& iOutAdapterIndex) = 0;
    virtual int  GetFullScreenStyle() = 0;
    virtual bool IsDisplayModeWindowed() = 0;
};

CVideoModeManagerInterface* GetVideoModeManager();
