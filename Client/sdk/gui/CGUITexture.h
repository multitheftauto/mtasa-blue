/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"

class CTextureItem;

class CGUITexture : public CGUIElement
{
public:
    virtual ~CGUITexture(){};

    virtual IDirect3DBaseTexture9* GetD3DTexture() = 0;

    virtual void LoadFromFile(std::string path) = 0;
};
