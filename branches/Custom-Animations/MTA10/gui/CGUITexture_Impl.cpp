/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUITexture_Impl.cpp
*  PURPOSE:     Texture handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Jax <>
*               Oli <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CGUITexture_Impl::CGUITexture_Impl ( CGUI_Impl* pGUI )
{
    // Save the renderer
    m_pRenderer = pGUI->GetRenderer ();

    // Create the texture
    m_pTexture = m_pRenderer->createTexture ();
}


CGUITexture_Impl::~CGUITexture_Impl ( void )
{
    if ( m_pTexture )
        m_pRenderer->destroyTexture ( m_pTexture );
}


bool CGUITexture_Impl::LoadFromFile ( const char* szFilename )
{
    // Verify that the file exists
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile ( szFilename, &FindFileData );
    if ( hFind == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    // Close the find
    FindClose ( hFind );

    // Try to load it
    try
    {
        m_pTexture->loadFromFile ( szFilename, "" );
    }
    catch ( CEGUI::Exception )
    {
        return false;
    }
    return true;
}


void CGUITexture_Impl::LoadFromMemory ( const void* pBuffer, unsigned int uiWidth, unsigned int uiHeight )
{
    m_pTexture->loadFromMemory ( pBuffer, uiWidth, uiHeight );
}


void CGUITexture_Impl::Clear ( void )
{
    // Destroy the previous texture and recreate it (empty)
    m_pRenderer->destroyTexture ( m_pTexture );
    m_pTexture = m_pRenderer->createTexture ();
}


CEGUI::Texture* CGUITexture_Impl::GetTexture ( void )
{
    return m_pTexture;
}

void CGUITexture_Impl::SetTexture ( CEGUI::Texture* pTexture )
{
    m_pTexture = pTexture;
}

LPDIRECT3DTEXTURE9 CGUITexture_Impl::GetD3DTexture ( void )
{
    return reinterpret_cast < CEGUI::DirectX9Texture* > ( m_pTexture ) -> getD3DTexture ();
}


void CGUITexture_Impl::CreateTexture ( unsigned int width, unsigned int height )
{
    return reinterpret_cast < CEGUI::DirectX9Texture* > ( m_pTexture ) -> createRenderTarget ( width, height );
}
