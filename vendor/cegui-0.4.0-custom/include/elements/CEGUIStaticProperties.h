/************************************************************************
	filename: 	CEGUIStaticProperties.h
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to properties for Static (base class) widget.
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
#ifndef _CEGUIStaticProperties_h_
#define _CEGUIStaticProperties_h_

#include "CEGUIProperty.h"

// Start of CEGUI namespace section
namespace CEGUI
{

// Start of StaticProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the Static (base) class
*/
namespace StaticProperties
{
/*!
\brief
	Property to access the state of the frame enabled setting for the Static widget.

	\par Usage:
		- Name: FrameEnabled
		- Format: "[text]".

	\par Where [Text] is:
		- "True" to indicate that the frame is enabled.
		- "False" to indicate that the frame is disabled.
*/
class FrameEnabled : public Property
{
public:
	FrameEnabled() : Property(
		"FrameEnabled",
		"Property to get/set the state of the frame enabled setting for the Static widget.  Value is either \"True\" or \"False\".",
		"True")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the state of the frame background setting for the Static widget.

	\par Usage:
		- Name: BackgroundEnabled
		- Format: "[text]".

	\par Where [Text] is:
		- "True" to indicate that the background is enabled.
		- "False" to indicate that the background is disabled.
*/
class BackgroundEnabled : public Property
{
public:
	BackgroundEnabled() : Property(
		"BackgroundEnabled",
		"Property to get/set the state of the frame background setting for the Static widget.  Value is either \"True\" or \"False\".",
		"True")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the frame colours for the Static widget.

	\par Usage:
		- Name: FrameColours
		- Format: "tl:[aarrggbb] tr:[aarrggbb] bl:[aarrggbb] br:[aarrggbb]".

	\par Where:
		- tl:[aarrggbb] is the top-left colour value specified as ARGB (hex).
		- tr:[aarrggbb] is the top-right colour value specified as ARGB (hex).
		- bl:[aarrggbb] is the bottom-left colour value specified as ARGB (hex).
		- br:[aarrggbb] is the bottom-right colour value specified as ARGB (hex).
*/
class FrameColours : public Property
{
public:
	FrameColours() : Property(
		"FrameColours",
		"Property to get/set the frame colours for the Static widget.  Value is \"tl:[aarrggbb] tr:[aarrggbb] bl:[aarrggbb] br:[aarrggbb]\".",
		"tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the background colours for the Static widget.

	\par Usage:
		- Name: BackgroundColours
		- Format: "tl:[aarrggbb] tr:[aarrggbb] bl:[aarrggbb] br:[aarrggbb]".

	\par Where:
		- tl:[aarrggbb] is the top-left colour value specified as ARGB (hex).
		- tr:[aarrggbb] is the top-right colour value specified as ARGB (hex).
		- bl:[aarrggbb] is the bottom-left colour value specified as ARGB (hex).
		- br:[aarrggbb] is the bottom-right colour value specified as ARGB (hex).
*/
class BackgroundColours : public Property
{
public:
	BackgroundColours() : Property(
		"BackgroundColours",
		"Property to get/set the background colours for the Static widget.  Value is \"tl:[aarrggbb] tr:[aarrggbb] bl:[aarrggbb] br:[aarrggbb]\".",
		"tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the background image for the Static widget.

	\par Usage:
		- Name: BackgroundImage
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class BackgroundImage : public Property
{
public:
	BackgroundImage() : Property(
		"BackgroundImage",
		"Property to get/set the background image for the Static widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the top-left image for the frame of the Static widget.

	\par Usage:
		- Name: TopLeftFrameImage
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class TopLeftFrameImage : public Property
{
public:
	TopLeftFrameImage() : Property(
		"TopLeftFrameImage",
		"Property to get/set the top-left image for the frame of the Static widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the top-right image for the frame of the Static widget.

	\par Usage:
		- Name: TopRightFrameImage
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class TopRightFrameImage : public Property
{
public:
	TopRightFrameImage() : Property(
		"TopRightFrameImage",
		"Property to get/set the top-right image for the frame of the Static widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the bottom-left image for the frame of the Static widget.

	\par Usage:
		- Name: BottomLeftFrameImage
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class BottomLeftFrameImage : public Property
{
public:
	BottomLeftFrameImage() : Property(
		"BottomLeftFrameImage",
		"Property to get/set the bottom-left image for the frame of the Static widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the bottom-right image for the frame of the Static widget.

	\par Usage:
		- Name: BottomRightFrameImage
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class BottomRightFrameImage : public Property
{
public:
	BottomRightFrameImage() : Property(
		"BottomRightFrameImage",
		"Property to get/set the bottom-right image for the frame of the Static widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the left edge image for the frame of the Static widget.

	\par Usage:
		- Name: LeftFrameImage
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class LeftFrameImage : public Property
{
public:
	LeftFrameImage() : Property(
		"LeftFrameImage",
		"Property to get/set the left edge image for the frame of the Static widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the right edge image for the frame of the Static widget.

	\par Usage:
		- Name: RightFrameImage
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class RightFrameImage : public Property
{
public:
	RightFrameImage() : Property(
		"RightFrameImage",
		"Property to get/set the right edge image for the frame of the Static widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the top edge image for the frame of the Static widget.

	\par Usage:
		- Name: TopFrameImage
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class TopFrameImage : public Property
{
public:
	TopFrameImage() : Property(
		"TopFrameImage",
		"Property to get/set the top edge image for the frame of the Static widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the bottom edge image for the frame of the Static widget.

	\par Usage:
		- Name: BottomFrameImage
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class BottomFrameImage : public Property
{
public:
	BottomFrameImage() : Property(
		"BottomFrameImage",
		"Property to get/set the bottom edge image for the frame of the Static widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};




} // End of  StaticProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIStaticProperties_h_
