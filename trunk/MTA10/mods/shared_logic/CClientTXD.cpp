/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTXD.cpp
*  PURPOSE:     TXD manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include <StdInc.h>

CClientTXD::CClientTXD ( class CClientManager* pManager, ElementID ID ) : CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pTexture = NULL;
}


CClientTXD::~CClientTXD ( void )
{
    // Remove us from all the models
    RemoveAll ();

    // Unload the texture
    UnloadTXD ();
}


bool CClientTXD::LoadTXD ( const char* szFile )
{
    // Are we not already loaded?
    if ( !m_pTexture )
    {
        // Try to load it
        m_pTexture = g_pGame->GetRenderWare ()->ReadTXD ( szFile );

        // Return whether we succeeded
        return m_pTexture != NULL;
	}

    // Failed
    return false;
}


void CClientTXD::UnloadTXD ( void )
{
    // Are we loaded?
	if ( m_pTexture )
    {
        // Destroy the texture
		g_pGame->GetRenderWare ()->DestroyTXD ( m_pTexture );
        m_pTexture = NULL;
    }
}


bool CClientTXD::Import ( unsigned short usModel )
{
    // Have we got a texture loaded and not already imported into this model?
    if ( m_pTexture && !IsImported ( usModel ) )
    {
        // Do the importing
        g_pGame->GetRenderWare ()->ModelInfoImportTXD ( m_pTexture, usModel );

        // Add the id to the list over which models we've imported ourselves into.
        m_Imported.push_back ( usModel );

        // Succeeded
        return true;
    }

    // Failed
    return false;
}


bool CClientTXD::IsImported ( unsigned short usModel )
{
    // Loop through our imported ids
    std::list < unsigned short > ::iterator iter = m_Imported.begin ();
    for ( ; iter != m_Imported.end (); iter++ )
    {
        // Same id as given?
        if ( *iter == usModel )
        {
            // We've imported us into this model
            return true;
        }
    }

    // We haven't
    return false;
}


void CClientTXD::Remove ( unsigned short usModel )
{
    // Loaded texture?
    if ( m_pTexture )
    {
        // Remove the model
        InternalRemove ( usModel );

        // Remove it from the list
        m_Imported.remove ( usModel );
    }
}


void CClientTXD::RemoveAll ( void )
{
    // Loaded texture?
    if ( m_pTexture )
    {
        // Loop through our imported ids
        std::list < unsigned short > ::iterator iter = m_Imported.begin ();
        for ( ; iter != m_Imported.end (); iter++ )
        {
            // Remove each
            InternalRemove ( *iter );
        }
    }

    // Clear the list
    m_Imported.clear ();
}


bool CClientTXD::IsImportableModel ( unsigned short usModel )
{
    // Currently we work on vehicles and objects
    return CClientObjectManager::IsValidModel ( usModel ) ||
           CClientVehicleManager::IsValidModel ( usModel );
}


void CClientTXD::InternalRemove ( unsigned short usModel )
{
    // Are we loaded?
	if ( m_pTexture )
    {
        // Remove the TXD from the model info
		g_pGame->GetRenderWare ()->ModelInfoRemoveTXD ( m_pTexture, usModel );
    }
}
