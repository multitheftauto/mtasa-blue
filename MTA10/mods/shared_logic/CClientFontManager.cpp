/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientFontManager.cpp
*  PURPOSE:     Custom font bucket bucket
*  DEVELOPERS:  qwerty
*
*****************************************************************************/

#include "StdInc.h"
#include "utils/XFont.h"


////////////////////////////////////////////////////////////////
//
// CClientFontManager::CClientFontManager
//
//
////////////////////////////////////////////////////////////////
CClientFontManager::CClientFontManager ( CClientManager* pClientManager )
{
    m_pClientManager = pClientManager;
}


////////////////////////////////////////////////////////////////
//
// CClientFontManager::~CClientFontManager
//
//
////////////////////////////////////////////////////////////////
CClientFontManager::~CClientFontManager ( void )
{
    // Remove any existing GUI font data
    for ( std::map < SString, SGUIFontInfo >::iterator iter = m_GUIFontInfoMap.begin () ; iter != m_GUIFontInfoMap.end () ; ++iter )
    {
        SGUIFontInfo& info = iter->second;
        delete info.pFntCEGUI;
    }

    // Remove any existing DX font data
    for ( std::map < SString, SDXFontInfo >::iterator iter = m_DXFontInfoMap.begin () ; iter != m_DXFontInfoMap.end () ; ++iter )
    {
        SDXFontInfo& info = iter->second;
        g_pCore->GetGraphics()->DestroyAdditionalDXFont ( info.strFullFilePath, info.pFntBig, info.pFntNormal );
    }
}


////////////////////////////////////////////////////////////////
//
// CClientFontManager::CreateFont
//
// Creates a brand spanking new font element.
// Actual underlying font may be shared to due techniques
//
////////////////////////////////////////////////////////////////
CClientFont* CClientFontManager::CreateFont ( const SString& strFullFilePath, uint uiSize, bool bBold, const SString& strMetaPath, CResource* pResource )
{
    uiSize = ( uiSize < 5 ) ? 5 : ( ( uiSize > 150 ) ? 150 : uiSize );

    // Unique font name for this file
    SString strFontName = SString ( "%s/%s", pResource->GetName (), strMetaPath.c_str () ).Replace ( "\\", "/" );

    // Find/create the underlying fonts
    SGUIFontInfo* pGUIFontInfo = GetGUIFontInfo ( strFontName, strFullFilePath, uiSize );
    if ( !pGUIFontInfo )
        return NULL;
    SDXFontInfo* pDXFontInfo = GetDXFontInfo ( strFontName, strFullFilePath, Round < uint > ( uiSize * 1.75f ), bBold );

    // Create the element
    CClientFont* pFontElement = new CClientFont ( m_pClientManager, INVALID_ELEMENT_ID, pGUIFontInfo->strCEGUIFontName, pDXFontInfo->pFntNormal, pDXFontInfo->pFntBig );

    // Add refs to the infos
    MapInsert ( pGUIFontInfo->fontElementUsers, pFontElement );
    MapInsert ( pDXFontInfo->fontElementUsers, pFontElement );

    pFontElement->SetParent ( pResource->GetResourceDynamicEntity() );
    return pFontElement;
}


////////////////////////////////////////////////////////////////
//
// CClientFontManager::Remove
//
// Called when a font element is being deleted.
//
////////////////////////////////////////////////////////////////
void CClientFontManager::Remove ( CClientFont* pFontElement )
{
    // Remove element ref from GUI font infos
    for ( std::map < SString, SGUIFontInfo >::iterator iter = m_GUIFontInfoMap.begin () ; iter != m_GUIFontInfoMap.end () ; )
    {
        SGUIFontInfo& info = iter->second;
        MapRemove ( info.fontElementUsers, pFontElement );
        if ( info.fontElementUsers.empty () )
        {
            // Destroy the CEGUI font data
            delete info.pFntCEGUI;
            m_GUIFontInfoMap.erase ( iter++ );
        }
        else
            ++iter;
    }

    // Remove element ref from DX font infos
    for ( std::map < SString, SDXFontInfo >::iterator iter = m_DXFontInfoMap.begin () ; iter != m_DXFontInfoMap.end () ; )
    {
        SDXFontInfo& info = iter->second;
        MapRemove ( info.fontElementUsers, pFontElement );
        if ( info.fontElementUsers.empty () )
        {
            // Destroy the DX font data
            g_pCore->GetGraphics()->DestroyAdditionalDXFont ( info.strFullFilePath, info.pFntBig, info.pFntNormal );
            m_DXFontInfoMap.erase ( iter++ );
        }
        else
            ++iter;
    }
}


////////////////////////////////////////////////////////////////
//
// CClientFontManager::GetGUIFontInfo
//
// Get a GUIFont which matches specification.
//
////////////////////////////////////////////////////////////////
SGUIFontInfo* CClientFontManager::GetGUIFontInfo ( const SString& strFontName, const SString& strFullFilePath, uint uiSize )
{
    // Find existing
    SString strKey ( "%s*%d", *strFontName, uiSize );
    SGUIFontInfo* pGUIFontInfo = MapFind ( m_GUIFontInfoMap, strKey );
    if ( !pGUIFontInfo )
    {
        // Try to create if required
        if ( CGUIFont* fntCEGUI = g_pCore->GetGUI ()->CreateFnt ( strKey, strFullFilePath, uiSize ) )
        {
            // Add new info
            SGUIFontInfo newInfo;
            newInfo.pFntCEGUI = fntCEGUI;
            newInfo.strCEGUIFontName = strKey;
            MapSet ( m_GUIFontInfoMap, strKey, newInfo );

            pGUIFontInfo = MapFind ( m_GUIFontInfoMap, strKey );
        }
    }
    return pGUIFontInfo;
}


////////////////////////////////////////////////////////////////
//
// CClientFontManager::GetDXFontInfo
//
// Get a DXFont which matches specification.
//
////////////////////////////////////////////////////////////////
SDXFontInfo* CClientFontManager::GetDXFontInfo ( const SString& strFontName, const SString& strFullFilePath, uint uiSize, bool bBold )
{
    // Find existing
    SString strKey ( "%s*%d*%d", *strFontName, uiSize, bBold );
    SDXFontInfo* pDXFontInfo = MapFind ( m_DXFontInfoMap, strKey );
    if ( !pDXFontInfo )
    {
        // Add new info
        SDXFontInfo newInfo;
        newInfo.pFntNormal = NULL;
        newInfo.pFntBig = NULL;

        // Let's grab the face name
        FONT_PROPERTIES sFontProps;
        if ( GetFontProperties ( LPCTSTR ( strFullFilePath.c_str () ), &sFontProps ) )
        {
            newInfo.strFullFilePath = strFullFilePath;

            ID3DXFont *pFntNormal = NULL,*pFntBig = NULL;
            if ( g_pCore->GetGraphics()->LoadAdditionalDXFont ( strFullFilePath, sFontProps.csName, uiSize, bBold, &pFntNormal, &pFntBig ) )
            {
                newInfo.pFntNormal = pFntNormal;
                newInfo.pFntBig = pFntBig;
            }
        }

        MapSet ( m_DXFontInfoMap, strKey, newInfo );
        pDXFontInfo = MapFind ( m_DXFontInfoMap, strKey );
    }
    return pDXFontInfo;
}
