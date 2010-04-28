/************************************************************************
	filename: 	CEGUIListboxImageItem.cpp
	created:	25/2/2006
	author:		C E Etheredge
	
	purpose:	Implementation of List box image items
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
#include "elements/CEGUIListboxImageItem.h"
#include "CEGUIWindow.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImage.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constructor
*************************************************************************/
ListboxImageItem::ListboxImageItem(const Image& image, uint item_id, void* item_data, bool disabled, bool auto_delete) :
	ListboxItem("", item_id, item_data, disabled, auto_delete),
	d_image(&image)
{
}


/*************************************************************************
	Return a pointer to the image being used by this ListboxImageItem
*************************************************************************/
const Image* ListboxImageItem::getImage(void) const
{
	if (d_image != NULL)
	{
		return d_image;
	}
	return d_image;
}


/*************************************************************************
	Return the rendered pixel size of this list box item.
*************************************************************************/
Size ListboxImageItem::getPixelSize(void) const
{
	Size tmp(0,0);

	if (d_image != NULL)
	{
		tmp.d_height	= PixelAligned(d_image->getHeight());
		tmp.d_width		= PixelAligned(d_image->getWidth());
	}

	return tmp;
}


/*************************************************************************
	Draw the list box item in its current state.
*************************************************************************/
void ListboxImageItem::draw(const Vector3& position, float alpha, const Rect& clipper) const
{
	if (d_selected && (d_selectBrush != NULL))
	{
		d_selectBrush->draw(clipper, position.d_z, clipper, getModulateAlphaColourRect(d_selectCols, alpha));
	}

	if (d_image != NULL)
	{
		d_image->draw(position, clipper);
	}
}

void ListboxImageItem::draw(RenderCache& cache,const Rect& targetRect, float zBase, float alpha, const Rect* clipper) const
{
    if (d_selected && d_selectBrush != 0)
    {
        cache.cacheImage(*d_selectBrush, targetRect, zBase, getModulateAlphaColourRect(d_selectCols, alpha), clipper);
    }

	if (d_image != NULL)
	{
		cache.cacheImage(*d_image, targetRect, zBase, ColourRect(colour(1,1,1,alpha)), clipper);
	}
}


bool ListboxImageItem::operator < ( const ListboxItem& rhs ) const
{
	// Compare the pointers
	ListboxImageItem* item = ( ListboxImageItem* ) &rhs;
	return d_image < item->getImage();
}


bool ListboxImageItem::operator > ( const ListboxItem& rhs ) const
{
	// Compare the pointers
	ListboxImageItem* item = ( ListboxImageItem* ) &rhs;
	return d_image > item->getImage();
}


} // End of  CEGUI namespace section