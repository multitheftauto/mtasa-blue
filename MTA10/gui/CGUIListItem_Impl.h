/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUIListItem_Impl.h
*  PURPOSE:     List widget item class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUILISTITEM_IMPL_H
#define __CGUILISTITEM_IMPL_H

#include <gui/CGUIListItem.h>
#include <CEGUI.h>

class CGUIStaticImage;
class CGUIStaticImage_Impl;

class CGUIListItem_Impl : public CGUIListItem
{
public:
	enum Type
	{
		TextItem = 0,
		NumberItem = 1,
		ImageItem = 2
	};

                            CGUIListItem_Impl       ( const char* szText = "", unsigned int uiType = 0, CGUIStaticImage_Impl* pImage = NULL );
                            ~CGUIListItem_Impl      ( void );

    void                    SetText                 ( const char *Text );
    std::string             GetText                 ( void );

	void*					GetData					( void );
	void					SetData					( void* pData );

	void					SetDisabled				( bool bDisabled );
	void					SetFont					( const char *szFontName );
	void					SetImage				( CGUIStaticImage* Image );

    bool                    GetSelectedState        ( void );
    void                    SetSelectedState        ( bool bState );

    CEGUI::ListboxItem*		GetListItem             ( void );

	unsigned int			ItemType;

private:
    CEGUI::ListboxItem*		m_pListItem;
	void*					m_pData;
};

#endif
