/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIStaticImage.h
*  PURPOSE:     Static image widget interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUISTATICIMAGE_H
#define __CGUISTATICIMAGE_H

#include "CGUIElement.h"
#include "CGUITexture.h"

class CGUIStaticImage : public CGUIElement
{
public:
    virtual                             ~CGUIStaticImage        ( void ) {};

    virtual bool                        LoadFromFile            ( const char* szFilename ) = 0;
    virtual bool                        LoadFromTexture         ( CGUITexture* pTexture ) = 0;
    virtual void                        Clear                   ( void ) = 0;
    virtual bool                        GetNativeSize           ( CVector2D &vecSize ) = 0;

    virtual void                        SetFrameEnabled         ( bool bFrameEnabled ) = 0;
    virtual bool                        IsFrameEnabled          ( void ) = 0;

    virtual void                        Render                  ( void ) = 0;
};

#endif
