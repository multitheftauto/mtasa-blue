/************************************************************************
	filename: 	CEGUITabControlProperties.cpp
	created:	08/08/2004
	author:		Steve Streeting
	
	purpose:	Implements Listbox Property classes
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
#include "elements/CEGUITabControlProperties.h"
#include "elements/CEGUITabControl.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of TabControlProperties namespace section
namespace TabControlProperties
{
String	AbsoluteTabHeight::get(const PropertyReceiver* receiver) const
{
    return PropertyHelper::floatToString(static_cast<const TabControl*>(receiver)->getAbsoluteTabHeight());
}


void	AbsoluteTabHeight::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<TabControl*>(receiver)->setAbsoluteTabHeight(PropertyHelper::stringToFloat(value));
}

String	RelativeTabHeight::get(const PropertyReceiver* receiver) const
{
    return PropertyHelper::floatToString(static_cast<const TabControl*>(receiver)->getRelativeTabHeight());
}

bool	AbsoluteTabHeight::isDefault(const PropertyReceiver* receiver) const
{
	return static_cast<const TabControl*>(receiver)->getRelativeTabHeight() == 0.05f;
}

String	AbsoluteTabHeight::getDefault(const PropertyReceiver* receiver) const
{
	float defval = 0;
	Window* par = static_cast<const TabControl*>(receiver)->getParent();

	if (par != 0)
	{
		defval = par->relativeToAbsoluteY(0.05f);
	}

	return PropertyHelper::floatToString(defval);
}


void	RelativeTabHeight::set(PropertyReceiver* receiver, const String& value)
{
    static_cast<TabControl*>(receiver)->setRelativeTabHeight(PropertyHelper::stringToFloat(value));
}


String	TabHeight::get(const PropertyReceiver* receiver) const
{
    return PropertyHelper::floatToString(static_cast<const TabControl*>(receiver)->getTabHeight());
}


void	TabHeight::set(PropertyReceiver* receiver, const String& value)
{
    static_cast<TabControl*>(receiver)->setTabHeight(PropertyHelper::stringToFloat(value));
}

bool TabHeight::isDefault(const PropertyReceiver* receiver) const
{
	if (static_cast<const TabControl*>(receiver)->getMetricsMode() == Relative)
	{
		return static_cast<const TabControl*>(receiver)->isPropertyDefault("RelativeTabHeight");
	}
	else
	{
		return static_cast<const TabControl*>(receiver)->isPropertyDefault("AbsoluteTabHeight");
	}
}

String TabHeight::getDefault(const PropertyReceiver* receiver) const
{
	if (static_cast<const TabControl*>(receiver)->getMetricsMode() == Relative)
	{
		return static_cast<const TabControl*>(receiver)->getPropertyDefault("RelativeTabHeight");
	}
	else
	{
		return static_cast<const TabControl*>(receiver)->getPropertyDefault("AbsoluteTabHeight");
	}
}



String	AbsoluteTabTextPadding::get(const PropertyReceiver* receiver) const
{
    return PropertyHelper::floatToString(static_cast<const TabControl*>(receiver)->getAbsoluteTabTextPadding());
}


void	AbsoluteTabTextPadding::set(PropertyReceiver* receiver, const String& value)
{
    static_cast<TabControl*>(receiver)->setAbsoluteTabTextPadding(PropertyHelper::stringToFloat(value));
}

String	RelativeTabTextPadding::get(const PropertyReceiver* receiver) const
{
    return PropertyHelper::floatToString(static_cast<const TabControl*>(receiver)->getRelativeTabTextPadding());
}


void	RelativeTabTextPadding::set(PropertyReceiver* receiver, const String& value)
{
    static_cast<TabControl*>(receiver)->setRelativeTabTextPadding(PropertyHelper::stringToFloat(value));
}

bool	RelativeTabTextPadding::isDefault(const PropertyReceiver* receiver) const
{
	return static_cast<const TabControl*>(receiver)->getAbsoluteTabTextPadding() == 5.0f;
}

String	RelativeTabTextPadding::getDefault(const PropertyReceiver* receiver) const
{
	float defval = 0;
	Window* par = static_cast<const TabControl*>(receiver)->getParent();

	if (par != 0)
	{
		defval = par->absoluteToRelativeY(5.0f);
	}

	return PropertyHelper::floatToString(defval);
}



String	TabTextPadding::get(const PropertyReceiver* receiver) const
{
    return PropertyHelper::floatToString(static_cast<const TabControl*>(receiver)->getTabTextPadding());
}


void	TabTextPadding::set(PropertyReceiver* receiver, const String& value)
{
    static_cast<TabControl*>(receiver)->setTabTextPadding(PropertyHelper::stringToFloat(value));
}

bool	TabTextPadding::isDefault(const PropertyReceiver* receiver) const
{
	if (static_cast<const TabControl*>(receiver)->getMetricsMode() == Relative)
	{
		return static_cast<const TabControl*>(receiver)->isPropertyDefault("RelativeTabTextPadding");
	}
	else
	{
		return static_cast<const TabControl*>(receiver)->isPropertyDefault("AbsoluteTabTextPadding");
	}
}

String	TabTextPadding::getDefault(const PropertyReceiver* receiver) const
{
	if (static_cast<const TabControl*>(receiver)->getMetricsMode() == Relative)
	{
		return static_cast<const TabControl*>(receiver)->getPropertyDefault("RelativeTabTextPadding");
	}
	else
	{
		return static_cast<const TabControl*>(receiver)->getPropertyDefault("AbsoluteTabTextPadding");
	}
}





} // End of  TabControlProperties namespace section

} // End of  CEGUI namespace section
