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
    m_bLoaded           = false;
	m_pData = NULL;
	m_pManager = pGUI;

    // Create the texture
    m_pTexture = new CGUITexture_Impl ( pGUI );

    // Get an unique identifier for CEGUI
    char szUnique [CGUI_CHAR_SIZE];
    pGUI->GetUniqueName ( szUnique );

    // Create the control and set default properties
    m_pWindow = pGUI->GetWindowManager ()->createWindow ( CGUISTATICIMAGE_NAME, szUnique );
    m_pWindow->setDestroyedByParent ( false );
    m_pWindow->setRect ( CEGUI::Relative, CEGUI::Rect (0, 0, 1.0, 1.0 ) );
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

    m_pManager->RemoveFromRedrawQueue ( reinterpret_cast < CGUIElement* > ( ( m_pWindow )->getUserData () ) );

    // Destroy the control
    m_pWindow->destroy ();

	// Destroy the properties list
	EmptyProperties ();
}


bool CGUIStaticImage_Impl::LoadFromFile ( const char* szFilename, const char* szDirectory )
{
	char szPath [ MAX_PATH + 1 ] = {0};

	unsigned int uiFilenameLen = ( unsigned int ) strlen ( szFilename );
	strncpy ( szPath, szDirectory ? szDirectory : m_pGUI->GetWorkingDirectory (), MAX_PATH );
	
	// Check if the path fits in the buffer
	if ( strlen ( szPath ) + uiFilenameLen > MAX_PATH ) return false;

	// Tail the filename to the path
	strncat ( szPath, szFilename, MAX_PATH );

    // Load texture
    if ( !m_pTexture->LoadFromFile ( szPath ) ) return false;

    // Load image
    return LoadFromTexture ( m_pTexture );
}


bool CGUIStaticImage_Impl::LoadFromTexture ( CGUITexture* pTexture )
{
    // Get CEGUI texture
    CEGUI::Texture* pCEGUITexture = ((CGUITexture_Impl*)pTexture)->GetTexture ();

    // Get an unique identifier for CEGUI for the imageset and the image
    char szUnique [CGUI_CHAR_SIZE];
    m_pGUI->GetUniqueName ( szUnique );

    // Create an imageset
    try
    {
        m_pImageset = m_pImagesetManager->createImageset ( szUnique, pCEGUITexture );
    }
    catch ( CEGUI::Exception& )
    {
        return false;
    };

    // Get an unique identifier for CEGUI for the image
    m_pGUI->GetUniqueName ( szUnique );
    
    // TODO: Fix memory leaks
    try
    {
        // Define an image and get it's pointer
        m_pImageset->defineImage ( szUnique, CEGUI::Point ( 0, 0 ), CEGUI::Size ( pCEGUITexture->getWidth (), pCEGUITexture->getHeight () ), CEGUI::Point ( 0, 0 ) );
        m_pImage = &m_pImageset->getImage ( szUnique );

        // Set the image just loaded as the image to be drawn for the widget
        reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) -> setImage ( m_pImage );
    }
    catch ( CEGUI::Exception& )
    {
        return false;
    };

    // Success
    m_bLoaded = true;
    return true;
}


void CGUIStaticImage_Impl::Clear ( void )
{
    // Stop the control from using it
    reinterpret_cast < CEGUI::StaticImage* > ( m_pWindow ) -> setImage ( NULL);

    // Kill the images
    if ( m_pImageset )
    {
        m_pImageset->undefineAllImages ();
    }

    // FIXME: We have a memoryleak here... can't figure out why this cause a crash:
    //m_pImagesetManager->destroyImageset ( m_pImageset );

    // Unload the texture
    m_pTexture->Clear ();

    // Update the loaded status boolean
    m_bLoaded = false;
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