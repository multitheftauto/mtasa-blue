/************************************************************************
	filename: 	CEGUIPushButtonProperties.cpp
	created:	22/2/2005
	author:		Paul D Turner
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
#include "CEGUIImage.h"
#include "CEGUIImageset.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIExceptions.h"

#include "elements/CEGUIPushButton.h"
#include "elements/CEGUIPushButtonProperties.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{

// Start of PushButtonProperties namespace section
namespace PushButtonProperties
{
    String NormalImage::get(const PropertyReceiver* receiver) const
    {
        const RenderableImage* img = static_cast<const PushButton*>(receiver)->getNormalImage();
        return img ? PropertyHelper::imageToString(img->getImage()) : String("");
    }

    void NormalImage::set(PropertyReceiver* receiver, const String &value)
    {
        RenderableImage image;
        image.setImage(PropertyHelper::stringToImage(value));
        image.setHorzFormatting(RenderableImage::HorzStretched);
        image.setVertFormatting(RenderableImage::VertStretched);
        static_cast<PushButton*>(receiver)->setNormalImage(&image);
    }

    String PushedImage::get(const PropertyReceiver* receiver) const
    {
        const RenderableImage* img = static_cast<const PushButton*>(receiver)->getPushedImage();
        return img ? PropertyHelper::imageToString(img->getImage()) : String("");
    }

    void PushedImage::set(PropertyReceiver* receiver, const String &value)
    {
        RenderableImage image;
        image.setImage(PropertyHelper::stringToImage(value));
        image.setHorzFormatting(RenderableImage::HorzStretched);
        image.setVertFormatting(RenderableImage::VertStretched);
        static_cast<PushButton*>(receiver)->setPushedImage(&image);
    }

    String HoverImage::get(const PropertyReceiver* receiver) const
    {
        const RenderableImage* img = static_cast<const PushButton*>(receiver)->getHoverImage();
        return img ? PropertyHelper::imageToString(img->getImage()) : String("");
    }

    void HoverImage::set(PropertyReceiver* receiver, const String &value)
    {
        RenderableImage image;
        image.setImage(PropertyHelper::stringToImage(value));
        image.setHorzFormatting(RenderableImage::HorzStretched);
        image.setVertFormatting(RenderableImage::VertStretched);
        static_cast<PushButton*>(receiver)->setHoverImage(&image);
    }

    String DisabledImage::get(const PropertyReceiver* receiver) const
    {
        const RenderableImage* img = static_cast<const PushButton*>(receiver)->getDisabledImage();
        return img ? PropertyHelper::imageToString(img->getImage()) : String("");
    }

    void DisabledImage::set(PropertyReceiver* receiver, const String &value)
    {
        RenderableImage image;
        image.setImage(PropertyHelper::stringToImage(value));
        image.setHorzFormatting(RenderableImage::HorzStretched);
        image.setVertFormatting(RenderableImage::VertStretched);
        static_cast<PushButton*>(receiver)->setDisabledImage(&image);
    }

    String UseStandardImagery::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::boolToString(static_cast<const PushButton*>(receiver)->isStandardImageryEnabled());
    }

    void UseStandardImagery::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<PushButton*>(receiver)->setStandardImageryEnabled(PropertyHelper::stringToBool(value));
    }

    String TextXOffset::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const PushButton*>(receiver)->getTextXOffset());
    }

    void TextXOffset::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<PushButton*>(receiver)->setTextXOffset(PropertyHelper::stringToFloat(value));
    }

}

}
