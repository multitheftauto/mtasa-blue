/************************************************************************
	filename: 	CEGUIPushButton.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for PushButton widget
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
#ifndef _CEGUIPushButton_h_
#define _CEGUIPushButton_h_

#include "CEGUIBase.h"
#include "elements/CEGUIButtonBase.h"
#include "elements/CEGUIPushButtonProperties.h"
#include "CEGUIRenderableImage.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
    Base class to provide logic for push button type widgets.
*/
class CEGUIEXPORT PushButton : public ButtonBase
{
public:
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Event name constants
	*************************************************************************/
	// generated internally by Window
	static const String EventClicked;					//!< The button was clicked.

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for base PushButton class
	*/
	PushButton(const String& type, const String& name);


	/*!
	\brief
		Destructor for PushButton class
	*/
	virtual ~PushButton(void);

    /*************************************************************************
    	Common Public Interface
    *************************************************************************/
	/*!
	\brief
		return whether or not rendering of the standard imagery is enabled.

	\return
		true if the standard button imagery will be rendered, false if no standard rendering will be performed.
	*/
	bool	isStandardImageryEnabled(void) const;


	/*!
	\brief
		Return whether of not custom button image areas are auto-scaled to the size of the button.

	\return
		true if client specified custom image areas are re-sized when the button size changes.  false if image areas will
		remain unchanged when the button is sized.
	*/
	bool	isCustomImageryAutoSized(void) const;

    /*!
	\brief
		returns a pointer to a read-only RenderableImage object holding the details
        of the image set to render for the button in the normal state, or 0 if no
        image is currently set for this state.

	\return
		Pointer to a const RenderableImage object with all the details for the image.  
	*/
	const RenderableImage*	getNormalImage(void) const;

	/*!
	\brief
        returns a pointer to a read-only RenderableImage object holding the details
        of the image set to render for the button in the highlighted state, or 0 if no
        image is currently set for this state.

	\return
		Pointer to a const RenderableImage object with all the details for the image.  
	*/
	const RenderableImage*	getHoverImage(void) const;

	/*!
	\brief
        returns a pointer to a read-only RenderableImage object holding the details
        of the image set to render for the button in the pushed state, or 0 if no
        image is currently set for this state.

	\return
		Pointer to a const RenderableImage object with all the details for the image.  
	*/
	const RenderableImage*	getPushedImage(void) const;

	/*!
	\brief
        returns a pointer to a read-only RenderableImage object holding the details
        of the image set to render for the button in the disabled state, or 0 if no
        image is currently set for this state.

	\return
		Pointer to a const RenderableImage object with all the details for the image.  
	*/
	const RenderableImage*	getDisabledImage(void) const;

    /*!
    \brief
        get the offset that is used to shift of the text in the x-direction
        this is useful if the button-images are not symmetrical and the
        text shouldn't be completely centered

    \return
        the offset relative to the button-size
    */
    float   getTextXOffset(void) const;

	/*!
	\brief
		set whether or not to render the standard imagery for the button

	\param setting
		true to have the standard button imagery drawn, false to have no standard imagery drawn.

	\return
		Nothing.
	*/
	void	setStandardImageryEnabled(bool setting);

	/*!
	\brief
		Set whether to auto re-size custom image areas when the button is sized.

	\param setting
		true to have custom image areas auto-sized.  false to have system leave image areas untouched.

	\return
		Nothing.
	*/
	void	setCustomImageryAutoSized(bool setting);

	/*!
	\brief
		set the details of the image to render for the button in the normal state.

	\param image
		RenderableImage object with all the details for the image.  Note that an internal copy of the Renderable image is made and
		ownership of \a image remains with client code.  If this parameter is NULL, rendering of an image for this button state is
		disabled.

	\return
		Nothing.
	*/
	void	setNormalImage(const RenderableImage* image);

	/*!
	\brief
		set the details of the image to render for the button in the highlighted state.

	\param image
		RenderableImage object with all the details for the image.  Note that an internal copy of the Renderable image is made and
		ownership of \a image remains with client code.  If this parameter is NULL, rendering of an image for this button state is
		disabled.

	\return
		Nothing.
	*/
	void	setHoverImage(const RenderableImage* image);

	/*!
	\brief
		set the details of the image to render for the button in the pushed state.

	\param image
		RenderableImage object with all the details for the image.  Note that an internal copy of the Renderable image is made and
		ownership of \a image remains with client code.  If this parameter is NULL, rendering of an image for this button state is
		disabled.

	\return
		Nothing.
	*/
	void	setPushedImage(const RenderableImage* image);

	/*!
	\brief
		set the details of the image to render for the button in the disabled state.

	\param image
		RenderableImage object with all the details for the image.  Note that an internal copy of the Renderable image is made and
		ownership of \a image remains with client code.  If this parameter is NULL, rendering of an image for this button state is
		disabled.

	\return
		Nothing.
	*/
	void	setDisabledImage(const RenderableImage* image);

    /*!
    \brief
        set the offset to use for a shift of the text in the x-direction
        this is useful if the button-images are not symmetrical and the
        text shouldn't be completely centered

    \param offset
        The offset to use. This is relative to the button-width

    \return
        Nothing.
    */
    void   setTextXOffset(float offset);

protected:
	/*************************************************************************
		New Event Handlers
	*************************************************************************/
	/*!
	\brief
		handler invoked internally when the button is clicked.
	*/
	virtual void	onClicked(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event Handlers
	*************************************************************************/
	virtual void	onMouseButtonUp(MouseEventArgs& e);
    virtual void    onSized(WindowEventArgs& e);


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add button specific events
	*/
	void	addPushButtonEvents(void);


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
		if (class_name==(const utf8*)"PushButton")	return true;
		return ButtonBase::testClassName_impl(class_name);
	}


    /*************************************************************************
    	Data Fields
    *************************************************************************/
    // flags stating which imagery is to be rendered and how.
    bool	d_autoscaleImages;				//!< when true custom images will be scaled to the same size as the button
    bool	d_useStandardImagery;			//!< true if button standard imagery should be drawn.
    bool	d_useNormalImage;				//!< true if an image should be drawn for the normal state.
    bool	d_useHoverImage;				//!< true if an image should be drawn for the highlighted state.
    bool	d_usePushedImage;				//!< true if an image should be drawn for the pushed state.
    bool	d_useDisabledImage;				//!< true if an image should be drawn for the disabled state.

    // button RenderableImage objects
    RenderableImage		d_normalImage;		//!< RenderableImage used when rendering an image in the normal state.
    RenderableImage		d_hoverImage;		//!< RenderableImage used when rendering an image in the highlighted state.
    RenderableImage		d_pushedImage;		//!< RenderableImage used when rendering an image in the pushed state.
    RenderableImage		d_disabledImage;	//!< RenderableImage used when rendering an image in the disabled state.

    //text-offset
    float d_textXOffset;		//!< offset applied to the x co-ordinate of the text label.

private:
    /*************************************************************************
        Static Properties for this class
    *************************************************************************/
    static PushButtonProperties::NormalImage    d_normalImageProperty;
    static PushButtonProperties::PushedImage    d_pushedImageProperty;
    static PushButtonProperties::HoverImage     d_hoverImageProperty;
    static PushButtonProperties::DisabledImage  d_disabledImageProperty;
    static PushButtonProperties::UseStandardImagery d_useStandardImageryProperty;
    static PushButtonProperties::TextXOffset    d_textXOffsetProperty;

    /*************************************************************************
        Private methods
    *************************************************************************/
    void	addPushButtonProperties(void);
};


} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIPushButton_h_
