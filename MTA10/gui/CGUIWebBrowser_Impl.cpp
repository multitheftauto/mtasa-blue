/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIWebBrowser_Impl.cpp
*  PURPOSE:     WebBrowser widget class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"
#include <core/CWebViewInterface.h>

// Use StaticImage here as we'd have to add the same definition twice to the Falagard definition file otherwise
#define CGUIWEBBROWSER_NAME "CGUI/StaticImage"

CGUIWebBrowser_Impl::CGUIWebBrowser_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent )
{
    // Initialize
    m_pImagesetManager  = pGUI->GetImageSetManager ();
    m_pImageset         = nullptr;
    m_pImage            = nullptr;
    m_pGUI              = pGUI;
    m_pManager          = pGUI;
    m_pWebView          = nullptr;

    // Get an unique identifier for CEGUI
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the control and set default properties
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUIWEBBROWSER_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
    m_pWindow->setRect ( CEGUI::Relative, CEGUI::Rect ( 0.0f, 0.0f, 1.0f, 1.0f ) );
    reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) -> setBackgroundEnabled ( false );

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData ( reinterpret_cast < void* > ( this ) );

    AddEvents ();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if ( pParent )
    {
        SetParent ( pParent );
    }
    else
    {
        pGUI->AddChild ( this );
        SetParent ( nullptr );
    }
}

CGUIWebBrowser_Impl::~CGUIWebBrowser_Impl ( void )
{
    // Clear the image
    Clear ();

    DestroyElement ();
}

bool CGUIWebBrowser_Impl::LoadFromTexture ( IDirect3DTexture9* pTexture )
{
    if ( m_pImageset && m_pImage )
    {
        m_pImageset->undefineAllImages ();
    }

    CGUIWebBrowserTexture* pCEGUITexture = new CGUIWebBrowserTexture ( m_pGUI->GetRenderer (), pTexture );

    // Get an unique identifier for CEGUI for the imageset
    char szUnique [CGUI_CHAR_SIZE];
    m_pGUI->GetUniqueName ( szUnique );

    // Create an imageset
    if ( !m_pImageset )
    {
        while ( m_pImagesetManager->isImagesetPresent( szUnique ) )
            m_pGUI->GetUniqueName ( szUnique );
        m_pImageset = m_pImagesetManager->createImageset ( szUnique, pCEGUITexture, true );
    }

    // Get an unique identifier for CEGUI for the image
    m_pGUI->GetUniqueName ( szUnique );
    
    // Define an image and get its pointer
    m_pImageset->defineImage ( szUnique, CEGUI::Point ( 0, 0 ), CEGUI::Size ( pCEGUITexture->getWidth (), pCEGUITexture->getHeight () ), CEGUI::Point ( 0, 0 ) );
    m_pImage = &m_pImageset->getImage ( szUnique );

    // Set the image just loaded as the image to be drawn for the widget
    reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow )->setImage ( m_pImage );

    // Success
    return true;
}

void CGUIWebBrowser_Impl::LoadFromWebView ( CWebViewInterface* pWebView )
{
    m_pWebView = pWebView;

    // Load webview texture
    LoadFromTexture ( pWebView->GetTexture () );
}

void CGUIWebBrowser_Impl::Clear ( void )
{
    // Stop the control from using it
    reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow )->setImage ( nullptr );

    // Kill the images
    if ( m_pImageset )
    {
        m_pImageset->undefineAllImages ();
        m_pImagesetManager->destroyImageset ( m_pImageset );
        m_pImage = nullptr;
        m_pImageset = nullptr;
    }
}

bool CGUIWebBrowser_Impl::GetNativeSize ( CVector2D& vecSize )
{
    auto pTexture = m_pWebView->GetTexture ();
    D3DSURFACE_DESC SurfaceDesc;
    if ( pTexture->GetLevelDesc ( 0, &SurfaceDesc ) == ERROR_SUCCESS )
    {
        vecSize.fX = (float)SurfaceDesc.Width;
        vecSize.fY = (float)SurfaceDesc.Height;
        return true;
    }
    
    return false;    
}

void CGUIWebBrowser_Impl::SetFrameEnabled ( bool bFrameEnabled )
{
    reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) -> setFrameEnabled ( bFrameEnabled );
}


bool CGUIWebBrowser_Impl::IsFrameEnabled ( void )
{
    return reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) -> isFrameEnabled ();
}


CEGUI::Image* CGUIWebBrowser_Impl::GetDirectImage ( void )
{
    return const_cast < CEGUI::Image* > ( reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) ->getImage () );
}

void CGUIWebBrowser_Impl::Render ( void )
{
    return reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) -> render ();
}




CGUIWebBrowserTexture::CGUIWebBrowserTexture ( CEGUI::Renderer* pOwner, IDirect3DTexture9* pTexture ) : CEGUI::DirectX9Texture(pOwner)
{
    m_pTexture = pTexture;

    // Get width and height
    D3DSURFACE_DESC SurfaceDesc;
    if ( pTexture->GetLevelDesc ( 0, &SurfaceDesc ) == ERROR_SUCCESS )
    {
        m_Width = SurfaceDesc.Width;
        m_Height = SurfaceDesc.Height;
    }
}
