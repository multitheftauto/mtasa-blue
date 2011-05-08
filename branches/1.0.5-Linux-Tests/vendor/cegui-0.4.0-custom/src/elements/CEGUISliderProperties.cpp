/************************************************************************
	filename: 	CEGUISliderProperties.cpp
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Implements properties for slider class
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
#include "elements/CEGUISliderProperties.h"
#include "elements/CEGUISlider.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of SliderProperties namespace section
namespace SliderProperties
{
String	CurrentValue::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const Slider*>(receiver)->getCurrentValue());
}


void	CurrentValue::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Slider*>(receiver)->setCurrentValue(PropertyHelper::stringToFloat(value));
}


String	MaximumValue::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const Slider*>(receiver)->getMaxValue());
}


void	MaximumValue::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Slider*>(receiver)->setMaxValue(PropertyHelper::stringToFloat(value));
}


String	ClickStepSize::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::floatToString(static_cast<const Slider*>(receiver)->getClickStep());
}


void	ClickStepSize::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<Slider*>(receiver)->setClickStep(PropertyHelper::stringToFloat(value));
}

} // End of  SliderProperties namespace section

} // End of  CEGUI namespace section
