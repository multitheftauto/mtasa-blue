/************************************************************************
	filename: 	CEGUISpinnerProperties.h
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
#ifndef _CEGUISpinnerProperties_h_
#define _CEGUISpinnerProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{
// Start of SpinnerProperties namespace section
namespace SpinnerProperties
{
    /*!
    \brief
	    Property to access the current value of the spinner.

	    \par Usage:
		    - Name: CurrentValue
		    - Format: "[float]".

	    \par Where:
		    - [float] represents the current value of the Spinner widget.
    */
    class CurrentValue : public Property
    {
    public:
	    CurrentValue() : Property(
		    "CurrentValue",
		    "Property to get/set the current value of the spinner.  Value is a float.",
		    "0.000000")
	    {}

	    String get(const PropertyReceiver* receiver) const;
	    void set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
	    Property to access the step size of the spinner.

	    \par Usage:
		    - Name: StepSize
		    - Format: "[float]".

	    \par Where:
		    - [float] represents the current value of the Spinner widget.
    */
    class StepSize : public Property
    {
    public:
	    StepSize() : Property(
		    "StepSize",
		    "Property to get/set the step size of the spinner.  Value is a float.",
		    "1.000000")
	    {}

	    String get(const PropertyReceiver* receiver) const;
	    void set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
	    Property to access the minimum value setting of the spinner.

	    \par Usage:
		    - Name: MinimumValue
		    - Format: "[float]".

	    \par Where:
		    - [float] represents the current minimum value of the Spinner widget.
    */
    class MinimumValue : public Property
    {
    public:
	    MinimumValue() : Property(
		    "MinimumValue",
		    "Property to get/set the minimum value setting of the spinner.  Value is a float.",
		    "-32768.000000")
	    {}

	    String get(const PropertyReceiver* receiver) const;
	    void set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
	    Property to access the maximum value setting of the spinner.

	    \par Usage:
		    - Name: MaximumValue
		    - Format: "[float]".

	    \par Where:
		    - [float] represents the current maximum value of the Spinner widget.
    */
    class MaximumValue : public Property
    {
    public:
	    MaximumValue() : Property(
		    "MaximumValue",
		    "Property to get/set the maximum value setting of the spinner.  Value is a float.",
		    "32767.000000")
	    {}

	    String get(const PropertyReceiver* receiver) const;
	    void set(PropertyReceiver* receiver, const String& value);
    };

    /*!
    \brief
	    Property to access the TextInputMode setting.

	    This property offers access the text display and input mode for the spinner.

	    \par Usage:
		    - Name: TextInputMode
		    - Format: "[text]".

	    \par Where [text] is:
		    - "FloatingPoint" for floating point decimal numbers.
		    - "Integer" for integer decimal numbers.
            - "Hexadecimal" for hexadecimal numbers.
            - "Octal" for octal numbers.
    */
    class TextInputMode : public Property
    {
    public:
	    TextInputMode() : Property(
		    "TextInputMode",
		    "Property to get/set the TextInputMode setting for the spinner.  Value is \"FloatingPoint\", \"Integer\", \"Hexadecimal\", or \"Octal\".",
		    "Integer")
	    {}

	    String	get(const PropertyReceiver* receiver) const;
	    void	set(PropertyReceiver* receiver, const String& value);
    };

} // End of  SpinnerProperties namespace section
} // End of  CEGUI namespace section


#endif	// end of guard _CEGUISpinnerProperties_h_
