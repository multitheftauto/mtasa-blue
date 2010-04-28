/************************************************************************
	filename: 	CEGUISpinnerProperties.cpp
	created:	7/2/2005
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
#include "elements/CEGUISpinnerProperties.h"
#include "elements/CEGUISpinner.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{
// Start of SpinnerProperties namespace section
namespace SpinnerProperties
{
    String CurrentValue::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const Spinner*>(receiver)->getCurrentValue());
    }

    void CurrentValue::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<Spinner*>(receiver)->setCurrentValue(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String StepSize::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const Spinner*>(receiver)->getStepSize());
    }

    void StepSize::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<Spinner*>(receiver)->setStepSize(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String MinimumValue::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const Spinner*>(receiver)->getMinimumValue());
    }

    void MinimumValue::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<Spinner*>(receiver)->setMinimumValue(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////

    String MaximumValue::get(const PropertyReceiver* receiver) const
    {
        return PropertyHelper::floatToString(static_cast<const Spinner*>(receiver)->getMaximumValue());
    }

    void MaximumValue::set(PropertyReceiver* receiver, const String& value)
    {
        static_cast<Spinner*>(receiver)->setMaximumValue(PropertyHelper::stringToFloat(value));
    }

    //////////////////////////////////////////////////////////////////////////
    
    String TextInputMode::get(const PropertyReceiver* receiver) const
    {
        switch(static_cast<const Spinner*>(receiver)->getTextInputMode())
        {
        case Spinner::FloatingPoint:
            return String("FloatingPoint");
            break;
        case Spinner::Hexadecimal:
            return String("Hexadecimal");
            break;
        case Spinner::Octal:
            return String("Octal");
            break;
        default:
            return String("Integer");
        }
    }

    void TextInputMode::set(PropertyReceiver* receiver, const String& value)
    {
        Spinner::TextInputMode mode;

        if (value == "FloatingPoint")
        {
            mode = Spinner::FloatingPoint;
        }
        else if (value == "Hexadecimal")
        {
            mode = Spinner::Hexadecimal;
        }
        else if (value == "Octal")
        {
            mode = Spinner::Octal;
        }
        else
        {
            mode = Spinner::Integer;
        }

        static_cast<Spinner*>(receiver)->setTextInputMode(mode);
    }

} // End of  SpinnerProperties namespace section
} // End of  CEGUI namespace section
