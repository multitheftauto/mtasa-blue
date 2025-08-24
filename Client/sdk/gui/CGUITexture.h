/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUITexture.h
 *  PURPOSE:     Texture handling interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <d3d9.h>

struct IDirect3DTexture9;

class CGUITexture
{
public:
    virtual ~CGUITexture(){};

    virtual bool LoadFromFile(const char* szFilename) = 0;
    virtual void LoadFromMemory(const void* pBuffer, unsigned int uiWidth, unsigned int uiHeight) = 0;
    virtual void Clear() = 0;

    virtual IDirect3DTexture9* GetD3DTexture() = 0;
    virtual void               CreateTexture(unsigned int width, unsigned int height) = 0;
};
