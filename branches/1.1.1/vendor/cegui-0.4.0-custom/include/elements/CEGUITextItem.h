/************************************************************************
	filename: 	CEGUITextItem.h
	created:	31/3/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Interface to base class for TextItem widget
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
#ifndef _CEGUITextItem_h_
#define _CEGUITextItem_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIItemEntry.h"
#include "elements/CEGUITextItemProperties.h"
#include "CEGUIFont.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	Base class for all the TextItem widgets.
*/
class CEGUIEXPORT TextItem : public ItemEntry
{
public:
	/*************************************************************************
		Constants
	*************************************************************************/
	static const colour		DefaultTextColour;		//!< Default colour used when rendering text.


	/*************************************************************************
		Accessors
	*************************************************************************/
	/*!
	\brief
		Return the TextFormatting currently used for rendering the text.

	\return
		TextFormatting value describing the currently used text formatting option.
	*/
	TextFormatting getTextFormatting() const		{return d_textFormatting;}


	/*!
    \brief
        get the offset that is used to shift of the text in the x-direction
        this is useful if the button-images are not symmetrical and the
        text shouldn't be completely centered

    \return
        the offset in pixels
    */
    float   getTextXOffset(void) const				{return d_textXOffset;}


	/*!
    \brief
        Get the current text colour of the item.

    \return
        The current text colour.
    */
	colour getTextColour(void) const			{return d_textColour;}


	/*************************************************************************
		Manipulators
	*************************************************************************/
	/*!
	\brief
		Set the TextFormatting to be used when rendering the text.
	*/
	void setTextFormatting(TextFormatting format)	{d_textFormatting=format;}


	/*!
    \brief
        Set the current text colour of the item.

    \param col
        The colour to set as the current text colour.
    */
	void setTextColour(const colour& col)			{d_textColour=col;}


	/*!
    \brief
        Set the offset to use for a shift of the text in the x-direction
        this is useful if the button-images are not symmetrical and the
        text shouldn't be completely centered

    \param offset
        The offset to use - in pixels.

    \return
        Nothing.
    */
    void   setTextXOffset(float offset)				{d_textXOffset=offset;}


	/*************************************************************************
		Pure functions from ItemEntry
	*************************************************************************/
	/*!
	\brief
		Return the "optimal" size for the item
	
	\return
		Size describing the size in pixel that this TextItem's content requires
		for non-clipped rendering
	*/
	virtual Size getItemPixelSize(void);


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for TextItem objects
	*/
	TextItem(const String& type, const String& name);


	/*!
	\brief
		Destructor for TextItem objects
	*/
	virtual ~TextItem(void);


protected:
	/*************************************************************************
		Abstract Implementation Functions (must be provided by derived class)
	*************************************************************************/

	/*************************************************************************
		Overridden event handlers
	*************************************************************************/
	/*!
	\brief
		Handler called when the window's text is changed.

	\param e
		WindowEventArgs object whose 'window' pointer field is set to the window that triggered the event.  For this
		event the trigger window is always 'this'.
	*/
	virtual void	onTextChanged(WindowEventArgs& e);


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
    virtual void populateRenderCache();


	/*!
	\brief
		Return whether this window was inherited from the given class name at some point in the inheritance heirarchy.

	\param class_name
		The class name that is to be checked.

	\return
		true if this window was inherited from \a class_name. false if not.
	*/
	virtual bool	testClassName_impl(const String& class_name) const
	{
		if (class_name==(const utf8*)"TextItem")	return true;
		return ItemEntry::testClassName_impl(class_name);
	}


	/*************************************************************************
		Implementation Rendering Functions
	*************************************************************************/

	/*************************************************************************
		Implementation Data
	*************************************************************************/
	colour d_textColour;				//!< the current text colour
	TextFormatting d_textFormatting;	//!< the current text formatting

	//text-offset
    float d_textXOffset;		//!< offset applied to the x co-ordinate of the text rendered.


private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static TextItemProperties::TextColour		d_textColourProperty;
	static TextItemProperties::TextFormatting	d_textFormattingProperty;
	static TextItemProperties::TextXOffset		d_textXOffsetProperty;

	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addTextItemProperties( bool bCommon = true );
protected:
	void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addTextItemProperties(false); }
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif


#endif	// end of guard _CEGUITextItem_h_
