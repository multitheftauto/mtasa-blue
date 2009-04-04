/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIStaticImage_Impl.h
*  PURPOSE:     Static image widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUISTATICIMAGE_IMPL_H
#define __CGUISTATICIMAGE_IMPL_H

#include <gui/CGUIStaticImage.h>
#include "CGUIElement_Impl.h"

class CGUITexture;
class CGUITexture_Impl;

class CGUIStaticImage_Impl : public CGUIStaticImage, public CGUIElement_Impl
{
public:
                                CGUIStaticImage_Impl    ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL );
                                ~CGUIStaticImage_Impl   ( void );

    bool                        LoadFromFile            ( const char* szFilename, const char* szDirectory = NULL );
    bool                        LoadFromTexture         ( CGUITexture* pTexture );
    void                        Clear                   ( void );

    void                        SetFrameEnabled         ( bool bFrameEnabled );
    bool                        IsFrameEnabled          ( void );

    CEGUI::Image*               GetDirectImage			( void );

    void                        Render                  ( void );

	eCGUIType					GetType					( void ) { return CGUI_STATICIMAGE; };

private:
    class CGUI_Impl*            m_pGUI;
    CGUITexture_Impl*           m_pTexture;
    CEGUI::ImagesetManager*     m_pImagesetManager;
    CEGUI::Imageset*            m_pImageset;
    const CEGUI::Image*         m_pImage;

    bool                        m_bLoaded;

    #include "CGUIElement_Inc.h"
};

#endif
