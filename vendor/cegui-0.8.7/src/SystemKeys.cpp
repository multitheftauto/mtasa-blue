/***********************************************************************
    created:    Fri Feb 17 2012
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/SystemKeys.h"

namespace CEGUI
{
//----------------------------------------------------------------------------//
SystemKeys::SystemKeys() :
    d_current(0),
    d_leftShift(false),
    d_rightShift(false),
    d_leftCtrl(false),
    d_rightCtrl(false),
    d_leftAlt(false),
    d_rightAlt(false)
{
}

//----------------------------------------------------------------------------//
void SystemKeys::reset()
{
    *this = SystemKeys();
}

//----------------------------------------------------------------------------//
uint SystemKeys::get() const
{
    return d_current;
}

//----------------------------------------------------------------------------//
bool SystemKeys::isPressed(SystemKey key) const
{
    return (d_current & key) || (!key && !d_current);
}

//----------------------------------------------------------------------------//
void SystemKeys::keyPressed(Key::Scan key)
{
    updatePressedStateForKey(key, true);
    updateSystemKeyState(keyCodeToSystemKey(key));
}

//----------------------------------------------------------------------------//
void SystemKeys::keyReleased(Key::Scan key)
{
    updatePressedStateForKey(key, false);
    updateSystemKeyState(keyCodeToSystemKey(key));
}

//----------------------------------------------------------------------------//
SystemKeys::SystemKey SystemKeys::keyCodeToSystemKey(Key::Scan key)
{
    switch (key)
    {
    case Key::RightShift:
    case Key::LeftShift:
        return Shift;

    case Key::LeftControl:
    case Key::RightControl:
        return Control;

    case Key::LeftAlt:
    case Key::RightAlt:
        return Alt;

    default:
        return None;
    }
}
    
//----------------------------------------------------------------------------//
void SystemKeys::updateSystemKeyState(SystemKey syskey)
{
    switch (syskey)
    {
    case Shift:
        (d_leftShift || d_rightShift) ? d_current |= Shift : d_current &= ~Shift;
        break;

    case Control:
        (d_leftCtrl || d_rightCtrl) ? d_current |= Control : d_current &= ~Control;
        break;

    case Alt:
        (d_leftAlt || d_rightAlt) ? d_current |= Alt : d_current &= ~Alt;
        break;

    default:
        break;
    }
}

//----------------------------------------------------------------------------//
void SystemKeys::updatePressedStateForKey(Key::Scan key, bool state)
{
    switch (key)
    {
    case Key::LeftShift:
        d_leftShift = state;
        break;

    case Key::RightShift:
        d_rightShift = state;
        break;

    case Key::LeftControl:
        d_leftCtrl = state;
        break;

    case Key::RightControl:
        d_rightCtrl = state;
        break;

    case Key::LeftAlt:
        d_leftAlt = state;
        break;

    case Key::RightAlt:
        d_rightAlt = state;
        break;

    default:
        break;
    }
}

//----------------------------------------------------------------------------//
void SystemKeys::mouseButtonPressed(MouseButton button)
{
    d_current |= mouseButtonToSystemKey(button);
}

//----------------------------------------------------------------------------//
void SystemKeys::mouseButtonReleased(MouseButton button)
{
    d_current &= ~mouseButtonToSystemKey(button);
}

//----------------------------------------------------------------------------//
SystemKeys::SystemKey SystemKeys::mouseButtonToSystemKey(MouseButton button)
{
	switch (button)
	{
	case LeftButton:
		return LeftMouse;

	case RightButton:
		return RightMouse;

	case MiddleButton:
		return MiddleMouse;

	case X1Button:
		return X1Mouse;

	case X2Button:
		return X2Mouse;

	default:
		return None; 
	}
}
//----------------------------------------------------------------------------//
}

