/************************************************************************
    filename:   FalThumb.cpp
    created:    Sun Jul 3 2005
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
#include "FalThumb.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardThumb::WidgetTypeName[] = "Falagard/Thumb";


    FalagardThumb::FalagardThumb(const String& type, const String& name) :
        Thumb(type, name)
    {
    }

    FalagardThumb::~FalagardThumb()
    {
    }

    void FalagardThumb::drawSelf(float z)
    {
        // this is hackish and relies on insider knowlegde of the way that both Thumb (actually ButtonBase)
        // and Window implement things; Soon I'll get to updating things so that this can be replaced
        // with clean code.

        // call Thumb drawSelf method which will call one of the state drawing methods overridden in this class.
        Thumb::drawSelf(z);

        // call Window drawSelf to get it to send the cached imagery to the renderer.
        Window::drawSelf(z);
    }

    void FalagardThumb::drawNormal(float z)
    {
        doThumbRender("Normal");
    }

    void FalagardThumb::drawHover(float z)
    {
        doThumbRender("Hover");
    }

    void FalagardThumb::drawPushed(float z)
    {
        doThumbRender("Pushed");
    }

    void FalagardThumb::drawDisabled(float z)
    {
        doThumbRender("Disabled");
    }

    void FalagardThumb::doThumbRender(const String& state)
    {
        // this is the second part of the hackish code.  We're duplicating the first section of code
        // from the Window::drawSelf method to decide whether to actually do anything.  It is likely
        // this will be replaced with cleaner code in the near future...

        // do we need to update the cache?
        if (d_needsRedraw)
        {
            // remove old cached imagery
            d_renderCache.clearCachedImagery();
            // signal that we'll no loger need a redraw.
            d_needsRedraw = false;

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
    }


    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardThumbFactory::createWindow(const String& name)
    {
        return new FalagardThumb(d_type, name);
    }

    void FalagardThumbFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section
