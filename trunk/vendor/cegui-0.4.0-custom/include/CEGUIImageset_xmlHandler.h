/************************************************************************
filename: 	CEGUIImageset_xmlHandler.h
created:	21/2/2004
author:		Paul D Turner

purpose:	Defines the interface for the Imageset class
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
#ifndef _CEGUIImageset_xmlHandler_h_
#define _CEGUIImageset_xmlHandler_h_

#include "CEGUIImageset.h"
#include "CEGUIXMLHandler.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
Implementation Classes
*************************************************************************/
/*!
\brief
Handler class used to parse the Imageset XML files using SAX2
*/
class Imageset_xmlHandler : public XMLHandler
{
public:
	/*************************************************************************
	Construction & Destruction
	*************************************************************************/
	/*!
	\brief
	Constructor for Imageset::xmlHandler objects

	\param imageset
	Pointer to the Imageset object creating this xmlHandler object
	*/
	Imageset_xmlHandler(Imageset* imageset) : d_imageset(imageset) {}

	/*!
	\brief
	Destructor for Imageset::xmlHandler objects
	*/
	virtual ~Imageset_xmlHandler(void) {}

	/*************************************************************************
	SAX2 Handler overrides
	*************************************************************************/ 
	/*!
	\brief
	document processing (only care about elements, schema validates format)
	*/
    virtual void elementStart(const String& element, const XMLAttributes& attributes);
    virtual void elementEnd(const String& element);

	/*************************************************************************
	Functions used by our implementation
	*************************************************************************/
	Imageset*	getImageset(void) const				{return d_imageset;}

private:
	/*************************************************************************
	Implementation Constants
	*************************************************************************/
	static const String ImagesetElement;				//!< Tag name for Imageset elements.
	static const String ImageElement;					//!< Tag name for Image elements.
	static const char	ImagesetNameAttribute[];		//!< Attribute name that stores the name of the Imageset
	static const char	ImagesetImageFileAttribute[];	//!< Attribute name that stores the filename for the image file.
    static const char	ImagesetResourceGroupAttribute[];   //!< Attribute name that stores the resource group identifier used when loading image file.
	static const char	ImagesetNativeHorzResAttribute[];	//!< Optional attribute that stores 'native' horizontal resolution for the Imageset.
	static const char	ImagesetNativeVertResAttribute[];	//!< Optional attribute that stores 'native' vertical resolution for the Imageset.
	static const char	ImagesetAutoScaledAttribute[];	//!< Optional attribute that specifies whether the Imageset should be auto-scaled.
	static const char	ImageNameAttribute[];			//!< Attribute name that stores the name of the new Image.
	static const char	ImageXPosAttribute[];			//!< Attribute name that stores the x position of the new Image.
	static const char	ImageYPosAttribute[];			//!< Attribute name that stores the y position of the new Image.
	static const char	ImageWidthAttribute[];			//!< Attribute name that stores the width of the new Image.
	static const char	ImageHeightAttribute[];			//!< Attribute name that stores the height of the new Image.
	static const char	ImageXOffsetAttribute[];		//!< Attribute name that stores the x rendering offset of the new Image.
	static const char	ImageYOffsetAttribute[];		//!< Attribute name that stores the y rendering offset of the new Image.

	/*************************************************************************
	Implementation Data
	*************************************************************************/
	Imageset*	d_imageset;			//!< Holds a pointer to the Imageset that created the handler object
};

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIImageset_xmlHandler_h_
