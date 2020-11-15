/***********************************************************************
    created:    29/12/2010
    author:     Lukas E Meindl

    purpose:    Implementation of ColourPicker base class
*************************************************************************/
/***************************************************************************
*   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/widgets/PushButton.h"
#include "CEGUI/widgets/Editbox.h"

#include "CEGUI/MouseCursor.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/CoordConverter.h"

#include "CEGUI/CommonDialogs/ColourPicker/ColourPicker.h"
#include "CEGUI/CommonDialogs/ColourPicker/Conversions.h"

#include "CEGUI/TextureTarget.h"
#include "CEGUI/Texture.h"
#include "CEGUI/PropertyHelper.h"

namespace CEGUI
{
//----------------------------------------------------------------------------//
//Constants
const String ColourPicker::EventNamespace("ColourPicker");
const String ColourPicker::WidgetTypeName("CEGUI/ColourPicker");
const String ColourPicker::EventOpenedPicker("OpenedPicker");
const String ColourPicker::EventClosedPicker("ClosedPicker");
const String ColourPicker::EventAcceptedColour("AcceptedColour");
//----------------------------------------------------------------------------//
// Child Widget names
const String ColourPicker::ColourRectName("__auto_colourrect__");
//----------------------------------------------------------------------------//
std::map<Window*, int> ColourPicker::s_colourPickerWindows;

//----------------------------------------------------------------------------//
ColourPicker::ColourPicker(const String& type, const String& name)
    : Window(type, name),
      d_shareColourPickerControlsWindow(false),
      d_colourPickerControlsWindow(0)
{
}

//----------------------------------------------------------------------------//
ColourPicker::~ColourPicker(void)
{
    if (d_colourPickerControlsWindow != 0)
    {
        if (d_shareColourPickerControlsWindow)
        {
            std::map<Window*, int>::iterator iter =
                s_colourPickerWindows.find(d_colourPickerControlsWindow);

            if (iter != s_colourPickerWindows.end())
            {
                if (iter->second <= 0)
                    WindowManager::getSingleton().destroyWindow(iter->first);
            }
        }
        else
            WindowManager::getSingleton().
            destroyWindow(d_colourPickerControlsWindow);
    }
}

//----------------------------------------------------------------------------//
void ColourPicker::initialiseComponents(void)
{
    // get component windows
    Window* colourRect = getColourRect();

    // bind handler to close button 'Click' event
    colourRect->subscribeEvent(Window::EventMouseClick,
        Event::Subscriber(&ColourPicker::colourRect_ColourRectClickedHandler,
                          this));

    performChildWindowLayout();

    d_selectedColour =
        getProperty<ColourRect>("Colour").d_top_left;

    initialiseColourPickerControlsWindow();
}

//----------------------------------------------------------------------------//
void ColourPicker::setColour(const Colour& newColour)
{
    d_selectedColour = newColour;

    WindowEventArgs e(this);
    fireEvent(EventAcceptedColour, e, EventNamespace);

    Colour selectedColourNoAlpha = d_selectedColour;
    selectedColourNoAlpha.setAlpha(1.0f);
    String colourRectString(PropertyHelper<ColourRect>::toString(
                                ColourRect(selectedColourNoAlpha)));

    this->setProperty("Colour", colourRectString);
}

//----------------------------------------------------------------------------//
Colour ColourPicker::getColour()
{
    return d_selectedColour;
}

//----------------------------------------------------------------------------//
Window* ColourPicker::getColourRect(void)
{
    return getChild(ColourRectName);
}

//----------------------------------------------------------------------------//
void ColourPicker::initialiseColourPickerControlsWindow()
{
    String colourPickerControlsStyle =
        this->getProperty("ColourPickerControlsStyle");

    d_shareColourPickerControlsWindow = this->getProperty<bool>("ShareColourPickerControlsWidget");

    if (d_shareColourPickerControlsWindow)
    {
        std::map<Window*, int>::iterator iter = s_colourPickerWindows.begin();

        while (iter != s_colourPickerWindows.end() &&
                iter->first->getType() == colourPickerControlsStyle)
        {
            ++iter;
        }

        if (iter != s_colourPickerWindows.end())
        {
            ++(iter->second);
        }
        else
        {
            createColourPickerControlsWindow(colourPickerControlsStyle);
            s_colourPickerWindows[d_colourPickerControlsWindow] = 1;
        }
    }
    else
        createColourPickerControlsWindow(colourPickerControlsStyle);


}

//----------------------------------------------------------------------------//
void ColourPicker::createColourPickerControlsWindow(
    const String& colourPickerControlsStyle)
{
    d_colourPickerControlsWindow = &dynamic_cast<ColourPickerControls&>(
        *WindowManager::getSingleton().createWindow(colourPickerControlsStyle));
}

//----------------------------------------------------------------------------//
bool ColourPicker::colourRect_ColourRectClickedHandler(const EventArgs&)
{
    WindowEventArgs args(this);
    onColourRectClicked(args);

    return true;
}

//----------------------------------------------------------------------------//
void ColourPicker::onColourRectClicked(WindowEventArgs& e)
{
    if (d_colourPickerControlsWindow)
    {
        if (d_colourPickerControlsWindow->getParent() == 0)
        {
            getGUIContext().getRootWindow()->
                addChild(d_colourPickerControlsWindow);

            d_colourPickerControlsWindow->setCallingColourPicker(this);
            d_colourPickerControlsWindow->setColours(d_selectedColour);
            d_colourPickerControlsWindow->setPreviousColour(d_selectedColour);
            d_colourPickerControlsWindow->refreshAllElements();
            fireEvent(EventOpenedPicker, e, EventNamespace);
        }
        else
        {
            if (d_colourPickerControlsWindow->getParent() != 0)
            {
                d_colourPickerControlsWindow->getParent()->
                removeChild(d_colourPickerControlsWindow);

                d_colourPickerControlsWindow->setCallingColourPicker(0);
                fireEvent(EventClosedPicker, e, EventNamespace);
            }
        }
    }
}

//----------------------------------------------------------------------------//

}

