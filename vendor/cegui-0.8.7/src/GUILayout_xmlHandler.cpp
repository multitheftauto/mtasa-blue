/***********************************************************************
    created:    5/7/2004
    author:     Paul D Turner

    purpose:    Implements XML parser for GUILayout files
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/GUILayout_xmlHandler.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/System.h"
#include "CEGUI/ScriptModule.h"
#include "CEGUI/XMLAttributes.h"
#include "CEGUI/WindowManager.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
    Implementation Constants
*************************************************************************/
// note: The assets' versions aren't usually the same as CEGUI version, they are versioned from version 1 onwards!
const String GUILayout_xmlHandler::NativeVersion( "4" );

const String GUILayout_xmlHandler::GUILayoutElement( "GUILayout" );
const String GUILayout_xmlHandler::LayoutImportElement( "LayoutImport" );
const String GUILayout_xmlHandler::EventElement( "Event" );
const String GUILayout_xmlHandler::LayoutImportFilenameAttribute( "filename" );
const String GUILayout_xmlHandler::LayoutImportResourceGroupAttribute( "resourceGroup" );
const String GUILayout_xmlHandler::EventNameAttribute( "name" );
const String GUILayout_xmlHandler::EventFunctionAttribute( "function" );
const String GUILayout_xmlHandler::GUILayoutVersionAttribute( "version" );

const String& GUILayout_xmlHandler::getSchemaName() const
{
    return WindowManager::GUILayoutSchemaName;
}
const String& GUILayout_xmlHandler::getDefaultResourceGroup() const
{
    return WindowManager::getSingleton().getDefaultResourceGroup();
}

void GUILayout_xmlHandler::elementStart(const String& element, const XMLAttributes& attributes)
{
	// handle root GUILayoutElement element
	if (element == GUILayoutElement)
	{
		elementGUILayoutStart(attributes);
	}
    // handle Window element (create window and make an entry on our "window stack")
	else if (element == Window::WindowXMLElementName)
    {
        elementWindowStart(attributes);
    }
    // handle AutoWindow element (get an auto child from the window on top of the "window stack")
    else if (element == Window::AutoWindowXMLElementName)
    {
        elementAutoWindowStart(attributes);
    }
	// handle UserString element (set user string for window at top of stack)
	else if (element == Window::UserStringXMLElementName)
    {
        elementUserStringStart(attributes);
    }
    // handle Property element (set property for window at top of stack)
    else if (element == Property::XMLElementName)
    {
        elementPropertyStart(attributes);
    }
    // handle layout import element (attach a layout to the window at the top of the stack)
    else if (element == LayoutImportElement)
    {
        elementLayoutImportStart(attributes);
    }
    // handle event subscription element
    else if (element == EventElement)
    {
        elementEventStart(attributes);
    }
    // anything else is an error which *should* have already been caught by XML validation
    else
    {
        Logger::getSingleton().logEvent("GUILayout_xmlHandler::startElement - Unexpected data was found while parsing the gui-layout file: '" + element + "' is unknown.", Errors);
    }
}

void GUILayout_xmlHandler::elementEnd(const String& element)
{
	//if (element == GUILayoutElement)
    //{
	//	NOOP
    //}

    // handle Window element
    if (element == Window::WindowXMLElementName)
    {
        elementWindowEnd();
    }
    // handle Window element
    else if (element == Window::AutoWindowXMLElementName)
    {
        elementAutoWindowEnd();
    }
    // handle UserString element
    else if (element == Window::UserStringXMLElementName)
    {
        elementUserStringEnd();
    }
    // handle Property element
    else if (element == Property::XMLElementName)
    {
        elementPropertyEnd();
    }
}


/*************************************************************************
    Put free text into the propertvalue buffer
*************************************************************************/
void GUILayout_xmlHandler::text(const String& text)
{
    d_stringItemValue += text;
}

/*************************************************************************
    Destroy all windows created so far.
*************************************************************************/
void GUILayout_xmlHandler::cleanupLoadedWindows(void)
{
    // Notes: We could just destroy the root window of the layout, which normally would also destroy
    // all attached windows.  Since the client may have specified that certain windows are not auto-destroyed
    // we can't rely on this, so we work backwards detaching and deleting windows instead.
    while (!d_stack.empty())
    {
        // only destroy if not an auto window
        if (d_stack.back().second)
        {
            Window* wnd = d_stack.back().first;

            // detach from parent
            if (wnd->getParent())
            {
                wnd->getParent()->removeChild(wnd);
            }

            // destroy the window
            WindowManager::getSingleton().destroyWindow(wnd);
        }

        // pop window from stack
        d_stack.pop_back();
    }

    d_root = 0;
}


/*************************************************************************
    Return a pointer to the 'root' window created.
*************************************************************************/
Window* GUILayout_xmlHandler::getLayoutRootWindow(void) const
{
    return d_root;
}

/*************************************************************************
    Method that handles the opening GUILayout XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementGUILayoutStart(const XMLAttributes& attributes)
{
    const String version(
        attributes.getValueAsString(GUILayoutVersionAttribute, "unknown"));

    if (version != NativeVersion)
    {
        CEGUI_THROW(InvalidRequestException(
            "You are attempting to load a layout of version '" + version +
            "' but this CEGUI version is only meant to load layouts of "
            "version '" + NativeVersion + "'. Consider using the "
            "migrate.py script bundled with CEGUI Unified Editor to "
            "migrate your data."));
    }
}

/*************************************************************************
    Method that handles the opening Window XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementWindowStart(const XMLAttributes& attributes)
{
    // get type of window to create
    const String windowType(
        attributes.getValueAsString(Window::WindowTypeXMLAttributeName));
    // get name for new window
    const String windowName(
        attributes.getValueAsString(Window::WindowNameXMLAttributeName));

    // attempt to create window
    CEGUI_TRY
    {
        Window* wnd = WindowManager::getSingleton().createWindow(windowType, windowName);

        // add this window to the current parent (if any)
        if (!d_stack.empty())
            d_stack.back().first->addChild(wnd);
        else
            d_root = wnd;

        // make this window the top of the stack
        d_stack.push_back(WindowStackEntry(wnd,true));

        // tell it that it is being initialised
        wnd->beginInitialisation();
    }
    CEGUI_CATCH (AlreadyExistsException&)
    {
        // delete all windows created
        cleanupLoadedWindows();

        // signal error - with more info about what we have done.
        CEGUI_THROW(InvalidRequestException(
            "layout loading has been aborted since Window named '" + windowName + "' already exists."));
    }
    CEGUI_CATCH (UnknownObjectException&)
    {
        // delete all windows created
        cleanupLoadedWindows();

        // signal error - with more info about what we have done.
        CEGUI_THROW(InvalidRequestException(
            "layout loading has been aborted since no WindowFactory is available for '" + windowType + "' objects."));
    }
}

/*************************************************************************
    Method that handles the opening AutoWindow XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementAutoWindowStart(const XMLAttributes& attributes)
{
    // get window name
    const String name_path(
        attributes.getValueAsString(Window::AutoWindowNamePathXMLAttributeName));

    CEGUI_TRY
    {
        // we need a window to fetch children
        if (!d_stack.empty())
        {
            Window* wnd = d_stack.back().first->getChild(name_path);
            // make this window the top of the stack
            d_stack.push_back(WindowStackEntry(wnd,false));
        }
    }
    CEGUI_CATCH (UnknownObjectException&)
    {
        // delete all windows created
        cleanupLoadedWindows();

        // signal error - with more info about what we have done.
        CEGUI_THROW(InvalidRequestException(
            "layout loading has been aborted since auto window '" +
            name_path + "' could not be referenced."));
    }
}

/*************************************************************************
    Method that handles the UserString XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementUserStringStart(const XMLAttributes& attributes)
{
    // Get user string name
    const String userStringName(
        attributes.getValueAsString(Window::UserStringNameXMLAttributeName));

    // Get user string value
    String userStringValue;
    if (attributes.exists(Window::UserStringValueXMLAttributeName))
        userStringValue = attributes.getValueAsString(
            Window::UserStringValueXMLAttributeName);

    // Short user string
    if (!userStringValue.empty())
    {
        d_stringItemName.clear();
        CEGUI_TRY
        {
            // need a window to be able to set properties!
            if (!d_stack.empty())
            {
                // get current window being defined.
                Window* curwindow = d_stack.back().first;

                curwindow->setUserString(userStringName, userStringValue);
            }
        }
        CEGUI_CATCH (Exception&)
        {
            // Don't do anything here, but the error will have been logged.
        }
    }
    // Long user string
    else
    {
        // Store name for later use
        d_stringItemName = userStringName;
        // reset the property (user string) value buffer
        d_stringItemValue.clear();
    }
}

/*************************************************************************
    Method that handles the Property XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementPropertyStart(const XMLAttributes& attributes)
{
    // get property name
    String propertyName(
        attributes.getValueAsString(Property::NameXMLAttributeName));

    // get property value string
    String propertyValue;
    if (attributes.exists(Property::ValueXMLAttributeName))
        propertyValue = attributes.getValueAsString(Property::ValueXMLAttributeName);

    // Short property 
    if (!propertyValue.empty())
    {
        d_stringItemName.clear();
        CEGUI_TRY
        {
            // need a window to be able to set properties!
            if (!d_stack.empty())
            {
                // get current window being defined.
                Window* curwindow = d_stack.back().first;

                bool useit = true;

                // if client defined a callback, call it and discover if we should
                // set the property.
                if (d_propertyCallback)
                    useit = (*d_propertyCallback)(curwindow,
                                                  propertyName,
                                                  propertyValue,
                                                  d_userData);
                // set the property as needed
                if (useit)
                    curwindow->setProperty(propertyName, propertyValue);
            }
        }
        CEGUI_CATCH (Exception&)
        {
            // Don't do anything here, but the error will have been logged.
        }
    }
    // Long property 
    else 
    {
        // Store name for later use 
        d_stringItemName = propertyName;
        // reset the property value buffer
        d_stringItemValue.clear();
    }
}

/*************************************************************************
    Method that handles the LayoutImport XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementLayoutImportStart(const XMLAttributes& attributes)
{
    CEGUI_TRY
    {
        // attempt to load the imported sub-layout
        Window* subLayout = WindowManager::getSingleton().loadLayoutFromFile(
                attributes.getValueAsString(LayoutImportFilenameAttribute),
                attributes.getValueAsString(LayoutImportResourceGroupAttribute),
                d_propertyCallback,
                d_userData);

        // attach the imported layout to the window being defined
        if ((subLayout != 0) && (!d_stack.empty()))
            d_stack.back().first->addChild(subLayout);
    }
    // something failed when loading the sub-layout
    CEGUI_CATCH (Exception&)
    {
        // delete all windows created so far
        cleanupLoadedWindows();

        // signal error - with more info about what we have done.
        CEGUI_THROW(GenericException(
            "layout loading aborted due to imported layout load failure (see error(s) above)."));
    }
}

/*************************************************************************
    Method that handles the Event XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementEventStart(const XMLAttributes& attributes)
{
    // get name of event
    String eventName(attributes.getValueAsString(EventNameAttribute));
    // get name of function
    String functionName(attributes.getValueAsString(EventFunctionAttribute));

    // attempt to subscribe property on window
    CEGUI_TRY
    {
        if (!d_stack.empty())
            d_stack.back().first->subscribeScriptedEvent(eventName, functionName);
    }
    CEGUI_CATCH (Exception&)
    {
        // Don't do anything here, but the error will have been logged.
    }
}

/*************************************************************************
    Method that handles the closing Window XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementWindowEnd()
{
    // pop a window from the window stack
    if (!d_stack.empty())
    {
        d_stack.back().first->endInitialisation();
        d_stack.pop_back();
    }
}

/*************************************************************************
    Method that handles the closing AutoWindow XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementAutoWindowEnd()
{
    // pop a window from the window stack
    if (!d_stack.empty())
    {
        d_stack.pop_back();
    }
}

/*************************************************************************
    Method that handles the closing UserString XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementUserStringEnd()
{
    // only do something if this is a "long" user string
    if (d_stringItemName.empty())
    {
        return;
    }
    CEGUI_TRY
    {
        // need a window to be able to set user strings!
        if (!d_stack.empty())
        {
            // get current window being defined.
            Window* curwindow = d_stack.back().first;

            curwindow->setUserString(d_stringItemName, d_stringItemValue);
        }
    }
    CEGUI_CATCH (Exception&)
    {
        // Don't do anything here, but the error will have been logged.
    }
}

/*************************************************************************
    Method that handles the closing Property XML element.
*************************************************************************/
void GUILayout_xmlHandler::elementPropertyEnd()
{
    // only do something if this is a "long" property
    if (d_stringItemName.empty())
    {
        return;
    }
    CEGUI_TRY
    {
        // need a window to be able to set properties!
        if (!d_stack.empty())
        {
            // get current window being defined.
            Window* curwindow = d_stack.back().first;

            bool useit = true;

            // if client defined a callback, call it and discover if we should
            // set the property.
            if (d_propertyCallback)
                useit = (*d_propertyCallback)(curwindow,
                                              d_stringItemName,
                                              d_stringItemValue,
                                              d_userData);
            // set the property as needed
            if (useit)
                curwindow->setProperty(d_stringItemName, d_stringItemValue);
        }
    }
    CEGUI_CATCH (Exception&)
    {
        // Don't do anything here, but the error will have been logged.
    }
}
} // End of  CEGUI namespace section
