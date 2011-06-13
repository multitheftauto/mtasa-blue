/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRenderElementManager.cpp
*  PURPOSE:
*  DEVELOPERS:  idiot
*
*****************************************************************************/

#include "StdInc.h"


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CClientRenderElementManager
//
//
////////////////////////////////////////////////////////////////
CClientRenderElementManager::CClientRenderElementManager ( CClientManager* pClientManager )
{
    m_pClientManager = pClientManager;
    m_pRenderItemManager = g_pCore->GetGraphics ()->GetRenderItemManager ();
    m_uiStatsFontCount = 0;
    m_uiStatsTextureCount = 0;
    m_uiStatsShaderCount = 0;
    m_uiStatsRenderTargetCount = 0;
    m_uiStatsScreenSourceCount = 0;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::~CClientRenderElementManager
//
//
////////////////////////////////////////////////////////////////
CClientRenderElementManager::~CClientRenderElementManager ( void )
{
    // Remove any existing
    while ( m_ItemElementMap.size () )
        Remove ( m_ItemElementMap.begin ()->second );
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateFont
//
//
//
////////////////////////////////////////////////////////////////
CClientFont* CClientRenderElementManager::CreateFont ( const SString& strFullFilePath, const SString& strUniqueName, uint uiSize, bool bBold )
{
    // Create the item
    SFontItem* pFontItem = m_pRenderItemManager->CreateFont ( strFullFilePath, strUniqueName, uiSize, bBold );

    // Check create worked
    if ( !pFontItem )
        return NULL;

    // Create the element
    CClientFont* pFontElement = new CClientFont ( m_pClientManager, INVALID_ELEMENT_ID, pFontItem );

    // Add to this manager's list
    MapSet ( m_ItemElementMap, pFontItem, pFontElement );

    // Update stats
    m_uiStatsFontCount++;

    return pFontElement;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateTexture
//
//
//
////////////////////////////////////////////////////////////////
CClientTexture* CClientRenderElementManager::CreateTexture ( const SString& strFullFilePath )
{
    // Create the item
    STextureItem* pTextureItem = m_pRenderItemManager->CreateTexture ( strFullFilePath );

    // Check create worked
    if ( !pTextureItem )
        return NULL;

    // Create the element
    CClientTexture* pTextureElement = new CClientTexture ( m_pClientManager, INVALID_ELEMENT_ID, pTextureItem );

    // Add to this manager's list
    MapSet ( m_ItemElementMap, pTextureItem, pTextureElement );

    // Update stats
    m_uiStatsTextureCount++;

    return pTextureElement;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateShader
//
//
//
////////////////////////////////////////////////////////////////
CClientShader* CClientRenderElementManager::CreateShader ( const SString& strFullFilePath, SString& strOutStatus )
{
    // Create the item
    SShaderItem* pShaderItem = m_pRenderItemManager->CreateShader ( strFullFilePath, strOutStatus );

    // Check create worked
    if ( !pShaderItem )
        return NULL;

    // Create the element
    CClientShader* pShaderElement = new CClientShader ( m_pClientManager, INVALID_ELEMENT_ID, pShaderItem );

    // Add to this manager's list
    MapSet ( m_ItemElementMap, pShaderItem, pShaderElement );

    // Update stats
    m_uiStatsShaderCount++;

    return pShaderElement;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateRenderTarget
//
//
//
////////////////////////////////////////////////////////////////
CClientRenderTarget* CClientRenderElementManager::CreateRenderTarget ( uint uiSizeX, uint uiSizeY )
{
    // Create the item
    SRenderTargetItem* pRenderTargetItem = m_pRenderItemManager->CreateRenderTarget ( uiSizeX, uiSizeY );

    // Check create worked
    if ( !pRenderTargetItem )
        return NULL;

    // Create the element
    CClientRenderTarget* pRenderTargetElement = new CClientRenderTarget ( m_pClientManager, INVALID_ELEMENT_ID, pRenderTargetItem );

    // Add to this manager's list
    MapSet ( m_ItemElementMap, pRenderTargetItem, pRenderTargetElement );

    // Update stats
    m_uiStatsRenderTargetCount++;

    return pRenderTargetElement;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateScreenSource
//
//
//
////////////////////////////////////////////////////////////////
CClientScreenSource* CClientRenderElementManager::CreateScreenSource ( uint uiSizeX, uint uiSizeY )
{
    // Create the item
    SScreenSourceItem* pScreenSourceItem = m_pRenderItemManager->CreateScreenSource ( uiSizeX, uiSizeY );

    // Check create worked
    if ( !pScreenSourceItem )
        return NULL;

    // Create the element
    CClientScreenSource* pScreenSourceElement = new CClientScreenSource ( m_pClientManager, INVALID_ELEMENT_ID, pScreenSourceItem );

    // Add to this manager's list
    MapSet ( m_ItemElementMap, pScreenSourceItem, pScreenSourceElement );

    // Update stats
    m_uiStatsScreenSourceCount++;

    return pScreenSourceElement;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::FindAutoTexture
//
// Find texture by unique name. Create if not found.
//
////////////////////////////////////////////////////////////////
CClientTexture* CClientRenderElementManager::FindAutoTexture ( const SString& strFullFilePath, const SString& strUniqueName )
{
    // Check if we've already done this file
    CClientTexture** ppTextureElement = MapFind ( m_AutoTextureMap, strUniqueName );
    if ( !ppTextureElement )
    {
        // Try to create
        CClientTexture* pNewTextureElement = CreateTexture ( strFullFilePath );
        if ( !pNewTextureElement )
            return NULL;

        // Add to automap if created
        MapSet ( m_AutoTextureMap, strUniqueName, pNewTextureElement );
        ppTextureElement = MapFind ( m_AutoTextureMap, strUniqueName );
    }

    return *ppTextureElement;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::Remove
//
// Called when an element is being deleted.
// Remove from lists and release render item
//
////////////////////////////////////////////////////////////////
void CClientRenderElementManager::Remove ( CClientRenderElement* pElement )
{
    // Validate
    assert ( pElement == *MapFind( m_ItemElementMap, pElement->GetRenderItem () ) );

    // Remove from this managers list
    MapRemove ( m_ItemElementMap, pElement->GetRenderItem () );

    // Remove from auto texture map
    if ( pElement->IsA ( CClientTexture::GetClassId () ) )
    {
        for ( std::map < SString, CClientTexture* >::iterator iter = m_AutoTextureMap.begin () ; iter != m_AutoTextureMap.end () ; ++iter )
        {
            if ( iter->second == pElement )
            {
                m_AutoTextureMap.erase ( iter );
                break;
            }
        }
    }

    // Update stats
    if ( pElement->IsA ( CClientFont::GetClassId () ) )
        m_uiStatsFontCount--;
    else
    if ( pElement->IsA ( CClientShader::GetClassId () ) )
        m_uiStatsShaderCount--;
    else
    if ( pElement->IsA ( CClientRenderTarget::GetClassId () ) )
        m_uiStatsRenderTargetCount--;
    else
    if ( pElement->IsA ( CClientScreenSource::GetClassId () ) )
        m_uiStatsScreenSourceCount--;
    else
    if ( pElement->IsA ( CClientTexture::GetClassId () ) )
        m_uiStatsTextureCount--;

    // Release render item
    m_pRenderItemManager->ReleaseRenderItem ( pElement->GetRenderItem () );
}
