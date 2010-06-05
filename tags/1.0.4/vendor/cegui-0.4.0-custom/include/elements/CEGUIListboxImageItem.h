/************************************************************************
	filename: 	CEGUIListboxImageItem.h
	created:	25/2/2006
	author:		C E Etheredge
	
	purpose:	Interface for list box image items
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
#ifndef _CEGUIListboxImageItem_h_
#define _CEGUIListboxImageItem_h_
#include "elements/CEGUIListboxItem.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Class used for textual items in a list box.
*/
class CEGUIEXPORT ListboxImageItem : public ListboxItem
{
public:
	/*************************************************************************
		Constants
	*************************************************************************/
	static const colour	DefaultTextColour;			//!< Default text colour.


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		base class constructor
	*/
	ListboxImageItem(const Image& image, uint item_id = 0, void* item_data = NULL, bool disabled = false, bool auto_delete = true);


	/*!
	\brief
		base class destructor
	*/
	virtual ~ListboxImageItem(void) {}


	/*************************************************************************
		Accessor methods
	*************************************************************************/
	/*!
	\brief
		Return a pointer to the image being used by this ListboxImageItem

	\return
		Image to be used for rendering this item
	*/
	const Image*	getImage(void) const;


	/*************************************************************************
		Manipulator methods
	*************************************************************************/
	/*!
	\brief
		Set the image to be used by this ListboxImageItem

	\param font
		Image to be used for rendering this item

	\return
		Nothing
	*/
	void	setImage(const Image* image)		{d_image = image;}


	/*************************************************************************
		Operators
	*************************************************************************/
	/*!
	\brief
		Less-than operator, compares item texts.
	*/
	virtual	bool	operator<(const ListboxItem& rhs) const;


	/*!
	\brief
		Greater-than operator, compares item texts.
	*/
	virtual	bool	operator>(const ListboxItem& rhs) const;


	/*************************************************************************
		Required implementations of pure virtuals from the base class.
	*************************************************************************/
    Size getPixelSize(void) const;
    void draw(const Vector3& position, float alpha, const Rect& clipper) const;
    void draw(RenderCache& cache,const Rect& targetRect, float zBase,  float alpha, const Rect* clipper) const;

protected:
	/*************************************************************************
		Implementation Data
	*************************************************************************/
	const Image*	d_image;			//!< Rendered image;
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIListboxImageItem_h_
