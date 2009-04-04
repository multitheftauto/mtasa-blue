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


void CGUIListItem_Impl::SetText ( const char *Text )
{
	CEGUI::String strText;

	if ( Text ) strText.assign ( Text );
	m_pListItem->setText ( strText );
}


void CGUIListItem_Impl::SetImage ( CGUIStaticImage* pImage )
{
	if ( ItemType == Type::ImageItem )
    {
        CGUIStaticImage_Impl* pImageImpl = (CGUIStaticImage_Impl*) pImage;
		reinterpret_cast < CEGUI::ListboxImageItem* > ( m_pListItem ) -> setImage ( pImageImpl->GetDirectImage () );
    }
}


std::string CGUIListItem_Impl::GetText ( void )
{
    return m_pListItem->getText ().c_str ();
}


CEGUI::ListboxItem* CGUIListItem_Impl::GetListItem ( void )
{
    return m_pListItem;
}

void* CGUIListItem_Impl::GetData ( void )
{
	return m_pData;
}

void CGUIListItem_Impl::SetData ( void* pData )
{
	m_pData = pData;
}

bool CGUIListItem_Impl::GetSelectedState ( void )
{
    return m_pListItem->isSelected ();
}

void CGUIListItem_Impl::SetSelectedState ( bool bState )
{
    m_pListItem->setSelected ( bState );
}