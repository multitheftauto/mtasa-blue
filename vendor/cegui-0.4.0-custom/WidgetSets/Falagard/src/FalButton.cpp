/************************************************************************
    filename:   FalButton.cpp
    created:    Wed Jun 22 2005
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
#include "FalButton.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardButton::WidgetTypeName[] = "Falagard/Button";

    FalagardButton::FalagardButton(const String& type, const String& name) :
        PushButton(type, name)
    {
    }

    FalagardButton::~FalagardButton()
    {
    }

    void FalagardButton::drawSelf(float z)
    {
        // this is hackish and relies on insider knowlegde of the way that both PushButton (actually ButtonBase)
        // and Window implement things; Soon I'll get to updating things so that this can be replaced
        // with clean code.

        // do we need to update the cache?
        if (d_needsRedraw)
        {
            // remove old cached imagery
            d_renderCache.clearCachedImagery();
            // signal that we'll no loger need a redraw.
            d_needsRedraw = false;
            // call PushButton drawSelf method which will call one of the state drawing methods overridden in this class.
            PushButton::drawSelf(z);
        }

        // call Window drawSelf to get it to send the cached imagery to the renderer.
        Window::drawSelf(z);
    }

    void FalagardButton::drawNormal(float z)
    {
        doButtonRender("Normal");

        // Support for legacy custom image setting
        if (d_useNormalImage)
        {
            ColourRect colours(d_normalImage.getColours());
            colours.setAlpha(getEffectiveAlpha());
            d_normalImage.setColours(colours);
            d_normalImage.draw(d_renderCache);
        }
    }

    void FalagardButton::drawHover(float z)
    {
        doButtonRender("Hover");

        // Support for legacy custom image setting
        if (d_useHoverImage)
        {
            ColourRect colours(d_hoverImage.getColours());
            colours.setAlpha(getEffectiveAlpha());
            d_hoverImage.setColours(colours);
            d_hoverImage.draw(d_renderCache);
        }
    }

    void FalagardButton::drawPushed(float z)
    {
        doButtonRender("Pushed");

        // Support for legacy custom image setting
        if (d_usePushedImage)
        {
            ColourRect colours(d_pushedImage.getColours());
            colours.setAlpha(getEffectiveAlpha());
            d_pushedImage.setColours(colours);
            d_pushedImage.draw(d_renderCache);
        }
    }

    void FalagardButton::drawDisabled(float z)
    {
        doButtonRender("Disabled");

        // Support for legacy custom image setting
        if (d_useDisabledImage)
        {
            ColourRect colours(d_disabledImage.getColours());
            colours.setAlpha(getEffectiveAlpha());
            d_disabledImage.setColours(colours);
            d_disabledImage.draw(d_renderCache);
        }
    }

    void FalagardButton::doButtonRender(const String& state)
    {
        // this is the second part of the hackish code.  We're duplicating the first section of code
        // from the Window::drawSelf method to decide whether to actually do anything.  It is likely
        // this will be replaced with cleaner code in the near future...

        // this conditional is just here to respect old legacy settings
        if (d_useStandardImagery)
        {
            const StateImagery* imagery;

            try
            {
                // get WidgetLookFeel for the assigned look.
                const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
                // try and get imagery for the state we were given, though default to Normal state if the
                // desired state does not exist
                imagery = wlf.isStateImageryPresent(state) ? &wlf.getStateImagery(state) : &wlf.getStateImagery("Normal");
            }
            // catch exceptions, but do not exit.
            catch (UnknownObjectException)
            {
                // don't try and draw using missing imagery!
                return;
            }

            // peform the rendering operation.
            // NB: This is not in the above try block since we want UnknownObjectException exceptions to be emitted from
            // the rendering code for conditions such as missing Imagesets and/or Images.
            imagery->render(*this);
        }
    }


    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardButtonFactory::createWindow(const String& name)
    {
        return new FalagardButton(d_type, name);
    }

    void FalagardButtonFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section
