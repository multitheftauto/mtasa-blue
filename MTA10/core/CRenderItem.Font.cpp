/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.Font.cpp
*  PURPOSE:
*  DEVELOPERS:  xidiot
*
*****************************************************************************/

#include "StdInc.h"
#include "utils/XFont.h"


////////////////////////////////////////////////////////////////
//
// CFontItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CFontItem::PostConstruct ( CRenderItemManager* pManager, const SString& strFullFilePath, const SString& strFontName, uint uiSize, bool bBold )
{
    Super::PostConstruct ( pManager );
    m_strFullFilePath = strFullFilePath;

    // Initial creation of d3d data
    CreateUnderlyingData ( strFontName, uiSize, bBold );
}


////////////////////////////////////////////////////////////////
//
// CFontItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CFontItem::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CFontItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CFontItem::IsValid ( void )
{
    return m_pFntNormal && m_pFntBig && m_pFntCEGUI;
}


////////////////////////////////////////////////////////////////
//
// CFontItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CFontItem::OnLostDevice ( void )
{
    m_pFntNormal->OnLostDevice ();
    m_pFntBig->OnLostDevice ();
}


////////////////////////////////////////////////////////////////
//
// CFontItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CFontItem::OnResetDevice ( void )
{
    m_pFntNormal->OnResetDevice ();
    m_pFntBig->OnResetDevice ();
}


////////////////////////////////////////////////////////////////
//
// CFontItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CFontItem::CreateUnderlyingData ( const SString& strFontName, uint uiSize, bool bBold )
{
    assert ( !m_pFntNormal );
    assert ( !m_pFntBig );
    assert ( !m_pFntCEGUI );

    uiSize = ( uiSize < 5 ) ? 5 : ( ( uiSize > 150 ) ? 150 : uiSize );

    // Find unused font name
    int iCounter = 0;
    do
        m_strCEGUIFontName = SString ( "%s*%d*%d_%d", *strFontName, uiSize, bBold, iCounter++ );
    while ( CCore::GetSingleton ().GetGUI ()->IsFontPresent ( m_strCEGUIFontName ) );

    // Create the CEGUI font
    try
    {
        m_pFntCEGUI = CCore::GetSingleton ().GetGUI ()->CreateFnt ( m_strCEGUIFontName, m_strFullFilePath, uiSize );
    }
    catch (...)
    {
        // Catch any font creation problems
    }

    if ( !m_pFntCEGUI )
        return;

    // Create ths DX fonts
    FONT_PROPERTIES sFontProps;
    if ( GetFontProperties ( LPCTSTR ( m_strFullFilePath.c_str () ), &sFontProps ) )
        CCore::GetSingleton ().GetGraphics()->LoadAdditionalDXFont ( m_strFullFilePath, sFontProps.csName, uiSize, bBold, &m_pFntNormal, &m_pFntBig );

    if ( !m_pFntNormal )
    {
        SAFE_DELETE( m_pFntCEGUI );
        return;
    }
}


////////////////////////////////////////////////////////////////
//
// CFontItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CFontItem::ReleaseUnderlyingData ( void )
{
    SAFE_DELETE( m_pFntCEGUI );

    // Release the DX font data
    CCore::GetSingleton ().GetGraphics()->DestroyAdditionalDXFont ( m_strFullFilePath, m_pFntBig, m_pFntNormal );
}
