/************************************************************************
    filename:   FalProgressBar.cpp
    created:    Sat Jul 2 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#include "FalProgressBar.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardProgressBar::WidgetTypeName[] = "Falagard/ProgressBar";
    FalagardProgressBarProperties::VerticalProgress FalagardProgressBar::d_verticalProperty;
    FalagardProgressBarProperties::ReversedProgress FalagardProgressBar::d_reversedProperty;


    FalagardProgressBar::FalagardProgressBar(const String& type, const String& name) :
        ProgressBar(type, name),
        d_vertical(false),
        d_reversed(false)
    {
        addProperty(&d_verticalProperty);
        addProperty(&d_reversedProperty);
    }

    FalagardProgressBar::~FalagardProgressBar()
    {
    }

    void FalagardProgressBar::populateRenderCache()
    {
        const StateImagery* imagery;

        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        // try and get imagery for our current state
        imagery = &wlf.getStateImagery(isDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation.
        imagery->render(*this);

        // get imagery for actual progress rendering
        imagery = &wlf.getStateImagery(d_enabled ? "EnabledProgress" : "DisabledProgress");

        // get target rect for this imagery
        Rect progressRect(wlf.getNamedArea("ProgressArea").getArea().getPixelRect(*this));

        // calculate a clipper according to the current progress.
        Rect progressClipper(progressRect);

        if (d_vertical)
        {
            float height = progressRect.getHeight() * d_progress;

            if (d_reversed)
            {
                progressRect.setHeight(height);
            }
            else
            {
                progressClipper.d_top = progressClipper.d_bottom - height;
            }
        }
        else
        {
            float width = progressRect.getWidth() * d_progress;

            if (d_reversed)
            {
                progressClipper.d_left = progressClipper.d_right - width;
            }
            else
            {
                progressRect.setWidth(width);
            }
        }

        // peform the rendering operation.
        imagery->render(*this, progressRect, 0, &progressClipper);
    }

    bool FalagardProgressBar::isVertical() const
    {
        return d_vertical;
    }

    bool FalagardProgressBar::isReversed() const
    {
        return d_reversed;
    }

    void FalagardProgressBar::setVertical(bool setting)
    {
        d_vertical = setting;
    }

    void FalagardProgressBar::setReversed(bool setting)
    {
        d_reversed = setting;
    }

    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardProgressBarFactory::createWindow(const String& name)
    {
        return new FalagardProgressBar(d_type, name);
    }

    void FalagardProgressBarFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section
