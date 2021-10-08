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

class CGUIStaticImage : public CGUIElement
{
public:
    virtual ~CGUIStaticImage(){};

    virtual CTextureItem* GetTexture() = 0;

    virtual bool LoadFromFile(std::string path) = 0;
};
