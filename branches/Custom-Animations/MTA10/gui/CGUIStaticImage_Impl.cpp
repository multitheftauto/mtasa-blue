/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIStaticImage_Impl.cpp
*  PURPOSE:     Static image widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define CGUISTATICIMAGE_NAME "CGUI/StaticImage"

CGUIStaticImage_Impl::CGUIStaticImage_Impl ( CGUI_Impl* pGUI, CGUIElement* pParent )
{
    // Initialize
    m_pImagesetManager  = pGUI->GetImageSetManager ();
    m_pImageset         = NULL;
    m_pImage            = NULL;
    m_pGUI              = pGUI;
    m_pManager = pGUI;
    m_pTexture = NULL;
    m_bCreatedTexture = false;

    // Get an unique identifier for CEGUI
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the control and set default properties
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUISTATICIMAGE_NAME, szUnique );
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
        SetParent ( NULL );
    }
}


CGUIStaticImage_Impl::~CGUIStaticImage_Impl ( void )
{
    // Clear the image
    Clear ();

    DestroyElement ();
}


bool CGUIStaticImage_Impl::LoadFromFile ( const char* szFilename, const char* szDirectory )
{
    std::string strPath = szDirectory ? szDirectory : m_pGUI->GetWorkingDirectory ();
    strPath += szFilename;

    // Load texture
    if ( !m_pTexture )
    {
        m_pTexture = new CGUITexture_Impl ( m_pGUI );
        m_bCreatedTexture = true;
    }

    if ( !m_pTexture->LoadFromFile ( strPath.c_str () ) )
        return false;

    // Load image
    return LoadFromTexture ( m_pTexture );
}


bool CGUIStaticImage_Impl::LoadFromTexture ( CGUITexture* pTexture )
{

    if ( m_pImageset || m_pImage )
        return false;

    if ( m_pTexture && pTexture != m_pTexture )
    {
        delete m_pTexture;
        m_bCreatedTexture = false;
    }
    
    m_pTexture = (CGUITexture_Impl *)pTexture;

    // Get CEGUI texture
    CEGUI::Texture* pCEGUITexture = m_pTexture->GetTexture ();

    // Get an unique identifier for CEGUI for the imageset
    char szUnique [CGUI_CHAR_SIZE];
    m_pGUI->GetUniqueName ( szUnique );

    // Create an imageset
    m_pImageset = m_pImagesetManager->createImageset ( szUnique, pCEGUITexture );

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


void CGUIStaticImage_Impl::Clear ( void )
{
    // Stop the control from using it
    reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow )->setImage ( NULL );

    // Kill the images
    if ( m_pImageset )
    {
        m_pImageset->undefineAllImages ();
        m_pImagesetManager->destroyImageset ( m_pImageset );
        if ( m_bCreatedTexture )
        {
            delete m_pTexture;
            m_pTexture = NULL;
        }
        m_pImage = NULL;
        m_pImageset = NULL;
    }
}


void CGUIStaticImage_Impl::SetFrameEnabled ( bool bFrameEnabled )
{
    reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) -> setFrameEnabled ( bFrameEnabled );
}


bool CGUIStaticImage_Impl::IsFrameEnabled ( void )
{
    return reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) -> isFrameEnabled ();
}


CEGUI::Image* CGUIStaticImage_Impl::GetDirectImage ( void )
{
    return const_cast < CEGUI::Image* > ( reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) ->getImage () );
}

void CGUIStaticImage_Impl::Render ( void )
{
    return reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) -> render ();
}
