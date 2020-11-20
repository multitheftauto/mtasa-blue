/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIStaticImage_Impl.cpp
 *  PURPOSE:     Static image widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUISTATICIMAGE_NAME "StaticImage"

using namespace GUINew;

CGUIStaticImage_Impl::CGUIStaticImage_Impl(CGUI_Impl* pGUI, CGUIElement* pParent)
{
    // Initialize
    m_pImagesetManager = pGUI->GetImageManager();
    m_pImage = NULL;
    m_pTexture = NULL;
    m_pGUI = pGUI;
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the control and set default properties
    m_pWindow = pGUI->GetWindowManager()->createWindow(pGUI->GetElementPrefix() + "/" + CGUISTATICIMAGE_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);
    m_pWindow->setArea(CEGUI::Rect(CEGUI::UDim(0.0f, 0), CEGUI::UDim(0.0f, 0), CEGUI::UDim(1.0f, 0), CEGUI::UDim(1.0f, 0)));
    m_pWindow->setProperty("BackgroundEnabled", "False");

    // Make sure frame is disabled
    SetFrameEnabled(false);

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    AddEvents();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if (pParent)
    {
        SetParent(pParent);
    }
    else
    {
        pGUI->AddChild(this);
        SetParent(NULL);
    }
}

CGUIStaticImage_Impl::~CGUIStaticImage_Impl()
{
    // Clear the image
    Clear();

    DestroyElement();
}

bool CGUIStaticImage_Impl::LoadFromFile(const char* szFilename, const char* szResourceGroup)
{
    // Ensure we don't already have an image
    Clear();

    // Check if the image already exists and load it
    if (m_pImagesetManager->isDefined(szFilename))
        m_pImage = (CEGUI::BasicImage*)&m_pImagesetManager->get(szFilename);
    else
    {
        // Define a new image in the ImageManager
        m_pImagesetManager->addFromImageFile(szFilename, szFilename, szResourceGroup);

        // Failed to create image
        if (!m_pImagesetManager->isDefined(szFilename))
            return false;

        // Get the image from the image manager and cast to BasicImage
        m_pImage = (CEGUI::BasicImage*)&m_pImagesetManager->get(szFilename);
    }

    // Set image to window
    m_pWindow->setProperty("Image", szFilename);

    return true;
}

bool CGUIStaticImage_Impl::LoadFromTexture(CGUITexture* pTexture)
{
    // Ensure we don't already have an image
    Clear();

    if (m_pTexture && pTexture != m_pTexture)
    {
        delete m_pTexture;
        m_pTexture = NULL;
    }

    m_pTexture = (CGUITexture_Impl*)pTexture;

    // Get CEGUI texture
    CEGUI::Texture* pCEGUITexture = m_pTexture->GetTexture();

    // Get an unique identifier for CEGUI for the image
    char szUnique[CGUI_CHAR_SIZE];
    m_pGUI->GetUniqueName(szUnique);

    // Define a new image in the ImageManager
    m_pImage = (CEGUI::BasicImage*)(&m_pImagesetManager->create("BasicImage", szUnique));

    // Set the texture of the created image
    m_pImage->setTexture(pCEGUITexture);

    // Set image to window
    m_pWindow->setProperty("Image", szUnique);

    // Success
    return true;
}

void CGUIStaticImage_Impl::Clear()
{
    if (!m_pImage)
        return;

    //m_pImagesetManager->destroy(m_pImage->getName());
}

bool CGUIStaticImage_Impl::GetNativeSize(CVector2D& vecSize)
{
    if (!m_pTexture && !m_pImage)
        return false;

    const CEGUI::Sizef* size = m_pTexture ? &m_pTexture->GetTexture()->getSize() : &m_pImage->getRenderedSize();

    vecSize.fX = size->d_width;
    vecSize.fY = size->d_height;

    return true;
}

void CGUIStaticImage_Impl::SetFrameEnabled(bool bFrameEnabled)
{
    m_pWindow->setProperty("FrameEnabled", bFrameEnabled ? "True" : "False");
}

bool CGUIStaticImage_Impl::IsFrameEnabled()
{
    return m_pWindow->getProperty("FrameEnabled") == "True" ? true : false;
}

CEGUI::Image* CGUIStaticImage_Impl::GetDirectImage()
{
    return m_pImage;
}

/* Unused? */
void CGUIStaticImage_Impl::Render()
{
    return m_pWindow->render();
}
