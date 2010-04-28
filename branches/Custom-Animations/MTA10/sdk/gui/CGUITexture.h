/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUITexture.h
*  PURPOSE:     Texture handling interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUITEXTURE_H
#define __CGUITEXTURE_H

#include <d3d9.h>

struct IDirect3DTexture9;

class CGUITexture
{
public:
    virtual                     ~CGUITexture            ( void ) {};

    virtual bool                LoadFromFile            ( const char* szFilename ) = 0;
    virtual void                LoadFromMemory          ( const void* pBuffer, unsigned int uiWidth, unsigned int uiHeight ) = 0;
    virtual void                Clear                   ( void ) = 0;

    virtual IDirect3DTexture9*  GetD3DTexture           ( void ) = 0;
    virtual void                CreateTexture           ( unsigned int width, unsigned int height ) = 0;
};

#endif
