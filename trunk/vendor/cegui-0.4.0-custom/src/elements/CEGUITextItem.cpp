/************************************************************************
	filename: 	CEGUITextItem.cpp
	created:	31/3/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Implementation of TextEntry widget base class
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "StdInc.h"
#include "elements/CEGUITextItem.h"
#include "elements/CEGUIItemListBase.h"
#include "CEGUIFont.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
Definition of Properties for this class
*************************************************************************/
TextItemProperties::TextXOffset		TextItem::d_textXOffsetProperty;
TextItemProperties::TextColour		TextItem::d_textColourProperty;
TextItemProperties::TextFormatting	TextItem::d_textFormattingProperty;

/*************************************************************************
Constants
*************************************************************************/
const colour	TextItem::DefaultTextColour	= 0xFFFFFFFF;

/*************************************************************************
	Constructor for TextItem base class.
*************************************************************************/
TextItem::TextItem(const String& type, const String& name) :
	ItemEntry(type, name),
	d_textColour(DefaultTextColour),
	d_textFormatting(LeftAligned),
	d_textXOffset(0.0f)
{
	addTextItemProperties();
}


/*************************************************************************
	Destructor for TextItem base class.
*************************************************************************/
TextItem::~TextItem(void)
{
}


/*************************************************************************
	Handler called when text is changed.
*************************************************************************/
void TextItem::onTextChanged(WindowEventArgs& e)
{
	Window::onTextChanged(e);
	
	// if we are attached to a ItemListBase, we inform the list of the change
	Window* parent = getParent();
	ItemListBase* ilb = (ItemListBase*)parent;
	if (parent!=NULL && parent->testClassName("ItemListBase") && ilb->isItemInList(this))
	{
		ilb->handleUpdatedItemData();
	}
	
	requestRedraw();
	e.handled = true;
}


/*************************************************************************
	Perform the actual rendering for this Window.
*************************************************************************/
void TextItem::populateRenderCache()
{
	Rect absrect(0,0,d_pixelSize.d_width, d_pixelSize.d_height);
	
	ColourRect colours(d_textColour);
	colours.modulateAlpha(getEffectiveAlpha());
		
	//
	// Draw label text
	//
	absrect.d_top += PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) / 2);
    absrect.d_left += PixelAligned(d_textXOffset);

    float zBase = System::getSingleton().getRenderer()->getZLayer(2) - System::getSingleton().getRenderer()->getCurrentZ();

    d_renderCache.cacheText(getText(), getFont(), d_textFormatting, absrect, zBase, colours);
}


/*************************************************************************
	Get the optimal pixelsize for this TextItem
*************************************************************************/
Size TextItem::getItemPixelSize()
{
	const Font *f = getFont();
	// we abs the x offset to to ensure negative offsets do not shrink the item.
	return Size(f->getTextExtent(getText())+(d_textXOffset<0?-d_textXOffset:d_textXOffset), f->getLineSpacing()+1);
}


/*************************************************************************
	Add TextItem specific properties
*************************************************************************/
void TextItem::addTextItemProperties( bool bCommon )
{
	if ( bCommon == false )
    {
        addProperty(&d_textColourProperty);
	    addProperty(&d_textFormattingProperty);
	    addProperty(&d_textXOffsetProperty);
    }
}

} // End of  CEGUI namespace section
