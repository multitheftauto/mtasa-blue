/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.GuiFont.cpp
*  PURPOSE:
*  DEVELOPERS:  xidiot
*
*****************************************************************************/

#include "StdInc.h"


////////////////////////////////////////////////////////////////
//
// CGuiFontItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CGuiFontItem::PostConstruct ( CRenderItemManager* pManager, const SString& strFullFilePath, const SString& strFontName, uint uiSize )
{
    Super::PostConstruct ( pManager );

    // Initial creation of cegui data
    CreateUnderlyingData ( strFullFilePath, strFontName, uiSize );
}


////////////////////////////////////////////////////////////////
//
// CGuiFontItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CGuiFontItem::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CGuiFontItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CGuiFontItem::IsValid ( void )
{
    return m_pFntCEGUI != NULL;
}


////////////////////////////////////////////////////////////////
//
// CGuiFontItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CGuiFontItem::OnLostDevice ( void )
{
    // Nothing required for CGuiFontItem
}


////////////////////////////////////////////////////////////////
//
// CGuiFontItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CGuiFontItem::OnResetDevice ( void )
{
    // Nothing required for CGuiFontItem
}


////////////////////////////////////////////////////////////////
//
// CGuiFontItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CGuiFontItem::CreateUnderlyingData ( const SString& strFullFilePath, const SString& strFontName, uint uiSize )
{
    assert ( !m_pFntCEGUI );

    uiSize = ( uiSize < 5 ) ? 5 : ( ( uiSize > 150 ) ? 150 : uiSize );

    // Find unused font name
    int iCounter = 0;
    do
        m_strCEGUIFontName = SString ( "%s*%d*%d", *strFontName, uiSize, iCounter++ );
    while ( CCore::GetSingleton ().GetGUI ()->IsFontPresent ( m_strCEGUIFontName ) );

    // Create the CEGUI font
    try
    {
        m_pFntCEGUI = CCore::GetSingleton ().GetGUI ()->CreateFnt ( m_strCEGUIFontName, strFullFilePath, uiSize );
    }
    catch (...)
    {
        // Catch any font creation problems
        // TODO - Cleanup open file handle
    }

    if ( !m_pFntCEGUI )
        return;

    // Memory usage - complete guess
    int iCharHeight = m_pFntCEGUI->GetFontHeight ( 1 );
    int iCharWidth = m_pFntCEGUI->GetTextExtent ( "A", 1 );
    int iNumChars = 256;
    int iBodgeFactor = 4;
    int iBPP = 32;
    int iMemoryUsed = iCharHeight * iCharWidth * iBPP / 8 * iNumChars * iBodgeFactor;
    m_iMemoryKBUsed = iMemoryUsed / 1024;
}


////////////////////////////////////////////////////////////////
//
// CGuiFontItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CGuiFontItem::ReleaseUnderlyingData ( void )
{
    SAFE_DELETE( m_pFntCEGUI );
}
