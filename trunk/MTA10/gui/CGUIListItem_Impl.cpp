/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIListItem_Impl.cpp
*  PURPOSE:     List widget item class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CGUIListItem_Impl::CGUIListItem_Impl ( const char* szText, unsigned int uiType, CGUIStaticImage_Impl* pImage )
{
	ItemType = uiType;

	// Create the requested list item type
	switch ( uiType )
	{
		case Type::TextItem:
			m_pListItem = new CEGUI::ListboxTextItem ( CEGUI::String ( szText ) );
			break;
		case Type::ImageItem:
			m_pListItem = new CEGUI::ListboxImageItem ( *pImage->GetDirectImage () );
			break;
		case Type::NumberItem:
			m_pListItem = new CEGUI::ListboxNumberItem ( CEGUI::String ( szText ) );
			break;
	}

    if ( m_pListItem )
    {
	    // Set flags and properties
        m_pListItem->setAutoDeleted ( false );
        m_pListItem->setSelectionBrushImage ( "CGUI-Images", "ListboxSelectionBrush" );
    }

	m_pData = NULL;
}


CGUIListItem_Impl::~CGUIListItem_Impl ( void )
{
    delete m_pListItem;
}


void CGUIListItem_Impl::SetDisabled ( bool bDisabled )
{
	reinterpret_cast < CEGUI::ListboxItem* > ( m_pListItem )->setDisabled ( bDisabled );
}


void CGUIListItem_Impl::SetFont ( const char *szFontName )
{
	if ( szFontName )
		reinterpret_cast < CEGUI::ListboxTextItem* > ( m_pListItem )->setFont ( CEGUI::String ( szFontName ) );
}


void CGUIListItem_Impl::SetText ( const char *pszText )
{
	CEGUI::String strText;

	if ( pszText )
        strText.assign ( pszText );
	m_pListItem->setText ( strText );
}


void CGUIListItem_Impl::SetData ( const char* pszData )
{
    if ( pszData )
    {
        m_strData = pszData;
        m_pData = (void *)m_strData.c_str ();
    }
    else
    {
        m_pData = NULL;
    }
}


void CGUIListItem_Impl::SetImage ( CGUIStaticImage* pImage )
{
	if ( ItemType == Type::ImageItem )
    {
        CGUIStaticImage_Impl* pImageImpl = (CGUIStaticImage_Impl*) pImage;
		reinterpret_cast < CEGUI::ListboxImageItem* > ( m_pListItem ) -> setImage ( pImageImpl->GetDirectImage () );
    }
}


std::string CGUIListItem_Impl::GetText ( void ) const
{
    return m_pListItem->getText ().c_str ();
}


CEGUI::ListboxItem* CGUIListItem_Impl::GetListItem ( void )
{
    return m_pListItem;
}

bool CGUIListItem_Impl::GetSelectedState ( void )
{
    return m_pListItem->isSelected ();
}

void CGUIListItem_Impl::SetSelectedState ( bool bState )
{
    m_pListItem->setSelected ( bState );
}

void CGUIListItem_Impl::SetColor ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha )
{
    if ( ItemType == Type::TextItem )
    {
        reinterpret_cast < CEGUI::ListboxTextItem* > ( m_pListItem )->setTextColours( CEGUI::colour( ( float ) ucRed / 255.0f, ( float ) ucGreen / 255.0f, ( float ) ucBlue / 255.0f, ( float ) ucAlpha / 255.0f ) );
    }
    else if ( ItemType == Type::NumberItem )
    {
        reinterpret_cast < CEGUI::ListboxNumberItem* > ( m_pListItem )->setTextColours( CEGUI::colour( ( float ) ucRed / 255.0f, ( float ) ucGreen / 255.0f, ( float ) ucBlue / 255.0f, ( float ) ucAlpha / 255.0f ) );
    }
}

bool CGUIListItem_Impl::GetColor ( unsigned char & ucRed, unsigned char & ucGreen, unsigned char & ucBlue, unsigned char & ucAlpha )
{
    if ( ItemType == Type::TextItem )
    {
        CEGUI::colour color = reinterpret_cast < CEGUI::ListboxTextItem* > ( m_pListItem )->getTextColours ().d_top_left;
        ucRed = color.getRed () * 255;
        ucGreen = color.getGreen () * 255;
        ucBlue = color.getBlue () * 255;
        ucAlpha = color.getAlpha () * 255;
        return true;
    }
    else if ( ItemType == Type::NumberItem )
    {
        CEGUI::colour color = reinterpret_cast < CEGUI::ListboxNumberItem* > ( m_pListItem )->getTextColours ().d_top_left;
        ucRed = color.getRed () * 255;
        ucGreen = color.getGreen () * 255;
        ucBlue = color.getBlue () * 255;
        ucAlpha = color.getAlpha () * 255;
        return true;
    }
    return false;
}