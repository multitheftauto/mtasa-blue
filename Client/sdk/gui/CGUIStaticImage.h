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

class CGUITexture;

class CGUIStaticImage : public CGUIElement
{
public:
    virtual ~CGUIStaticImage(){};

    virtual CGUITexture* GetTexture() = 0;

    virtual bool LoadFromFile(std::string path) = 0;
    virtual void LoadFromTexture(CGUITexture* texture) = 0;
};
