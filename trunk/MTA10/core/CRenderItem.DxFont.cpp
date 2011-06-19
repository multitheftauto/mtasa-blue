/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.DxFont.cpp
*  PURPOSE:
*  DEVELOPERS:  xidiot
*
*****************************************************************************/

#include "StdInc.h"
#include "utils/XFont.h"


////////////////////////////////////////////////////////////////
//
// CDxFontItem::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CDxFontItem::PostConstruct ( CRenderItemManager* pManager, const SString& strFullFilePath, uint uiSize, bool bBold )
{
    Super::PostConstruct ( pManager );
    m_strFullFilePath = strFullFilePath;

    // Initial creation of d3d data
    CreateUnderlyingData ( uiSize, bBold );
}


////////////////////////////////////////////////////////////////
//
// CDxFontItem::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CDxFontItem::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CDxFontItem::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CDxFontItem::IsValid ( void )
{
    return m_pFntNormal && m_pFntBig;
}


////////////////////////////////////////////////////////////////
//
// CDxFontItem::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CDxFontItem::OnLostDevice ( void )
{
    m_pFntNormal->OnLostDevice ();
    m_pFntBig->OnLostDevice ();
}


////////////////////////////////////////////////////////////////
//
// CDxFontItem::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CDxFontItem::OnResetDevice ( void )
{
    m_pFntNormal->OnResetDevice ();
    m_pFntBig->OnResetDevice ();
}


////////////////////////////////////////////////////////////////
//
// CDxFontItem::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CDxFontItem::CreateUnderlyingData ( uint uiSize, bool bBold )
{
    assert ( !m_pFntNormal );
    assert ( !m_pFntBig );

    uiSize = ( uiSize < 5 ) ? 5 : ( ( uiSize > 150 ) ? 150 : uiSize );

    // Create the D3DX fonts
    FONT_PROPERTIES sFontProps;
    if ( GetFontProperties ( LPCTSTR ( m_strFullFilePath.c_str () ), &sFontProps ) )
        CCore::GetSingleton ().GetGraphics()->LoadAdditionalDXFont ( m_strFullFilePath, sFontProps.csName, static_cast < int > ( std::floor ( uiSize * 1.75f ) ), bBold, &m_pFntNormal, &m_pFntBig );
}


////////////////////////////////////////////////////////////////
//
// CDxFontItem::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CDxFontItem::ReleaseUnderlyingData ( void )
{
    // Release the D3DX font data
    CCore::GetSingleton ().GetGraphics()->DestroyAdditionalDXFont ( m_strFullFilePath, m_pFntBig, m_pFntNormal );
}
