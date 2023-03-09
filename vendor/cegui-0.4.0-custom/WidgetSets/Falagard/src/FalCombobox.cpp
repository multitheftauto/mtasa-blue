/************************************************************************
    filename:   FalCombobox.cpp
    created:    Mon Jul 4 2005
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
#include "FalCombobox.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "CEGUIWindowManager.h"
#include "elements/CEGUIPushButton.h"
#include "elements/CEGUIEditbox.h"
#include "elements/CEGUIComboDropList.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const utf8 FalagardCombobox::WidgetTypeName[] = "Falagard/Combobox";

    FalagardCombobox::FalagardCombobox(const String& type, const String& name) :
        Combobox(type, name)
    {
    }

    FalagardCombobox::~FalagardCombobox()
    {
    }

    void FalagardCombobox::populateRenderCache()
    {
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = WidgetLookManager::getSingleton().getWidgetLook(d_lookName);
        // try and get imagery for the approprite state.
        const StateImagery* imagery = &wlf.getStateImagery(isDisabled() ? "Disabled" : "Enabled");
        // peform the rendering operation
        imagery->render(*this);
    }

    Editbox* FalagardCombobox::createEditbox(const String& name) const
    {
        // return component created by look'n'feel assignment.
        return static_cast<Editbox*>(WindowManager::getSingleton().getWindow(name));
    }

    PushButton* FalagardCombobox::createPushButton(const String& name) const
    {
        // return component created by look'n'feel assignment.
        return static_cast<PushButton*>(WindowManager::getSingleton().getWindow(name));
    }

    ComboDropList* FalagardCombobox::createDropList(const String& name) const
    {
        // return component created by look'n'feel assignment.
        return static_cast<ComboDropList*>(WindowManager::getSingleton().getWindow(name));
    }

    //////////////////////////////////////////////////////////////////////////
    /*************************************************************************

        Factory Methods

    *************************************************************************/
    //////////////////////////////////////////////////////////////////////////
    Window* FalagardComboboxFactory::createWindow(const String& name)
    {
        return new FalagardCombobox(d_type, name);
    }

    void FalagardComboboxFactory::destroyWindow(Window* window)
    {
        delete window;
    }

} // End of  CEGUI namespace section
