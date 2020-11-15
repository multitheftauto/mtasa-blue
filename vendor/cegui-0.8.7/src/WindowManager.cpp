/***********************************************************************
	created:	21/2/2004
	author:		Paul D Turner

	purpose:	Implements the WindowManager class
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
#include "CEGUI/WindowManager.h"
#include "CEGUI/WindowFactoryManager.h"
#include "CEGUI/WindowFactory.h"
#include "CEGUI/Window.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/GUILayout_xmlHandler.h"
#include "CEGUI/XMLParser.h"
#include "CEGUI/RenderEffectManager.h"
#include "CEGUI/RenderingWindow.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Static Data Definitions
*************************************************************************/
// singleton instance pointer
template<> WindowManager* Singleton<WindowManager>::ms_Singleton	= 0;
// default resource group
String WindowManager::d_defaultResourceGroup;

/*************************************************************************
	Definition of constant data for WindowManager
*************************************************************************/
// Declared in WindowManager
const String WindowManager::GUILayoutSchemaName("GUILayout.xsd");
const String WindowManager::GeneratedWindowNameBase("__cewin_uid_");
const String WindowManager::EventNamespace("WindowManager");
const String WindowManager::EventWindowCreated("WindowCreated");
const String WindowManager::EventWindowDestroyed("WindowDestroyed");
    

/*************************************************************************
    Constructor
*************************************************************************/
WindowManager::WindowManager(void) :
    d_uid_counter(0),
    d_lockCount(0)
{
    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
        "CEGUI::WindowManager singleton created " + String(addr_buff));
}


/*************************************************************************
	Destructor
*************************************************************************/
WindowManager::~WindowManager(void)
{
	destroyAllWindows();
    cleanDeadPool();

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
        "CEGUI::WindowManager singleton destroyed " + String(addr_buff));
}


/*************************************************************************
	Create a new window of the specified type
*************************************************************************/
Window* WindowManager::createWindow(const String& type, const String& name)
{
    // only allow creation of Window objects if we are in unlocked state
    if (isLocked())
        CEGUI_THROW(InvalidRequestException(
            "WindowManager is in the locked state."));

    String finalName(name.empty() ? generateUniqueWindowName() : name);

    WindowFactoryManager& wfMgr = WindowFactoryManager::getSingleton();
    WindowFactory* factory = wfMgr.getFactory(type);

    Window* newWindow = factory->createWindow(finalName);

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(newWindow));
    Logger::getSingleton().logEvent("Window '" + finalName +"' of type '" +
        type + "' has been created. " + addr_buff, Informative);

    // see if we need to assign a look to this window
    if (wfMgr.isFalagardMappedType(type))
    {
        const WindowFactoryManager::FalagardWindowMapping& fwm = wfMgr.getFalagardMappingForType(type);
        // this was a mapped type, so assign a look to the window so it can finalise
        // its initialisation
        newWindow->d_falagardType = type;
        newWindow->setWindowRenderer(fwm.d_rendererType);
        newWindow->setLookNFeel(fwm.d_lookName);

        initialiseRenderEffect(newWindow, fwm.d_effectName);
    }

	d_windowRegistry.push_back(newWindow);

    // fire event to notify interested parites about the new window.
    WindowEventArgs args(newWindow);
    fireEvent(EventWindowCreated, args, EventNamespace);
    
	return newWindow;
}

//---------------------------------------------------------------------------//
void WindowManager::initialiseRenderEffect(
        Window* wnd, const String& effect) const
{
    Logger& logger(Logger::getSingleton());

    // nothing to do if effect is empty string
    if (effect.empty())
        return;

    // if requested RenderEffect is not available, log that and continue
    if (!RenderEffectManager::getSingleton().isEffectAvailable(effect))
    {
        logger.logEvent("Missing RenderEffect '" + effect + "' requested for "
            "window '" + wnd->getName() + "' - continuing without effect...",
            Errors);

       return;
    }

    // If we do not have a RenderingSurface, enable AutoRenderingSurface to
    // try and create one
    if (!wnd->getRenderingSurface())
    {
        logger.logEvent("Enabling AutoRenderingSurface on '" +
            wnd->getName() + "' for RenderEffect support.");

        wnd->setUsingAutoRenderingSurface(true);
    }

    // If we have a RenderingSurface and it's a RenderingWindow
    if (wnd->getRenderingSurface() &&
        wnd->getRenderingSurface()->isRenderingWindow())
    {
        // Set an instance of the requested RenderEffect
        static_cast<RenderingWindow*>(wnd->getRenderingSurface())->
                setRenderEffect(&RenderEffectManager::getSingleton().
                        create(effect, wnd));
    }
    // log fact that we could not get a usable RenderingSurface
    else
    {
        logger.logEvent("Unable to set effect for window '" +
            wnd->getName() + "' since RenderingSurface is either missing "
            "or of wrong type (i.e. not a RenderingWindow).",
            Errors);
    }
}

/*************************************************************************
	Destroy the given window by pointer
*************************************************************************/
void WindowManager::destroyWindow(Window* window)
{
	WindowVector::iterator iter =
        std::find(d_windowRegistry.begin(),
                  d_windowRegistry.end(),
                  window);

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(&window));

	if (iter == d_windowRegistry.end())
    {
        Logger::getSingleton().logEvent("[WindowManager] Attempt to delete "
            "Window that does not exist!  Address was: " + String(addr_buff) +
            ". WARNING: This could indicate a double-deletion issue!!",
            Errors);
        return;
    }

    d_windowRegistry.erase(iter);

    Logger::getSingleton().logEvent("Window at '" + window->getNamePath() +
        "' will be added to dead pool. " + addr_buff, Informative);

    // do 'safe' part of cleanup
    window->destroy();

    // add window to dead pool
    d_deathrow.push_back(window);

    // fire event to notify interested parites about window destruction.
    // TODO: Perhaps this should fire first, so window is still usable?
    WindowEventArgs args(window);
    fireEvent(EventWindowDestroyed, args, EventNamespace);
}


/*************************************************************************
	Destroy all Window objects
*************************************************************************/
void WindowManager::destroyAllWindows(void)
{
	while (!d_windowRegistry.empty())
		destroyWindow(*d_windowRegistry.begin());
}

//----------------------------------------------------------------------------//
bool WindowManager::isAlive(const Window* window) const
{
	WindowVector::const_iterator iter =
        std::find(d_windowRegistry.begin(),
                  d_windowRegistry.end(),
                  window);

	return iter != d_windowRegistry.end();
}

Window* WindowManager::loadLayoutFromContainer(const RawDataContainer& source, PropertyCallback* callback, void* userdata)
{
    // log the fact we are about to load a layout
    Logger::getSingleton().logEvent("---- Beginning loading of GUI layout from a RawDataContainer ----", Informative);

    // create handler object
    GUILayout_xmlHandler handler(callback, userdata);

    // do parse (which uses handler to create actual data)
    CEGUI_TRY
    {
        System::getSingleton().getXMLParser()->parseXML(handler, source, GUILayoutSchemaName);
    }
    CEGUI_CATCH(...)
    {
        Logger::getSingleton().logEvent("WindowManager::loadWindowLayout - loading of layout from a RawDataContainer failed.", Errors);
        CEGUI_RETHROW;
    }

    // log the completion of loading
    Logger::getSingleton().logEvent("---- Successfully completed loading of GUI layout from a RawDataContainer ----", Standard);

    return handler.getLayoutRootWindow();
}

Window* WindowManager::loadLayoutFromFile(const String& filename, const String& resourceGroup, PropertyCallback* callback, void* userdata)
{
	if (filename.empty())
	{
		CEGUI_THROW(InvalidRequestException(
            "Filename supplied for gui-layout loading must be valid."));
	}

	// log the fact we are about to load a layout
	Logger::getSingleton().logEvent("---- Beginning loading of GUI layout from '" + filename + "' ----", Informative);

    // create handler object
    GUILayout_xmlHandler handler(callback, userdata);

	// do parse (which uses handler to create actual data)
	CEGUI_TRY
	{
        System::getSingleton().getXMLParser()->parseXMLFile(handler,
            filename, GUILayoutSchemaName, resourceGroup.empty() ? d_defaultResourceGroup : resourceGroup);
	}
	CEGUI_CATCH(...)
	{
        Logger::getSingleton().logEvent("WindowManager::loadLayoutFromFile - loading of layout from file '" + filename +"' failed.", Errors);
        CEGUI_RETHROW;
	}

    // log the completion of loading
    Logger::getSingleton().logEvent("---- Successfully completed loading of GUI layout from '" + filename + "' ----", Standard);

	return handler.getLayoutRootWindow();
}

Window* WindowManager::loadLayoutFromString(const String& source, PropertyCallback* callback, void* userdata)
{
    // log the fact we are about to load a layout
    Logger::getSingleton().logEvent("---- Beginning loading of GUI layout from string ----", Informative);

    // create handler object
    GUILayout_xmlHandler handler(callback, userdata);

    // do parse (which uses handler to create actual data)
    CEGUI_TRY
    {
        System::getSingleton().getXMLParser()->parseXMLString(handler, source, GUILayoutSchemaName);
    }
    CEGUI_CATCH(...)
    {
        Logger::getSingleton().logEvent("WindowManager::loadLayoutFromString - loading of layout from string failed.", Errors);
        CEGUI_RETHROW;
    }

    // log the completion of loading
    Logger::getSingleton().logEvent("---- Successfully completed loading of GUI layout from string ----", Standard);

    return handler.getLayoutRootWindow();
}

bool WindowManager::isDeadPoolEmpty(void) const
{
    return d_deathrow.empty();
}

void WindowManager::cleanDeadPool(void)
{
    WindowVector::reverse_iterator curr = d_deathrow.rbegin();
    for (; curr != d_deathrow.rend(); ++curr)
    {
// in debug mode, log what gets cleaned from the dead pool (insane level)
#if defined(DEBUG) || defined (_DEBUG)
        CEGUI_LOGINSANE("Window '" + (*curr)->getName() + "' about to be finally destroyed from dead pool.");
#endif

        WindowFactory* factory = WindowFactoryManager::getSingleton().getFactory((*curr)->getType());
        factory->destroyWindow(*curr);
    }

    // all done here, so clear all pointers from dead pool
    d_deathrow.clear();
}

void WindowManager::writeLayoutToStream(const Window& window, OutStream& out_stream) const
{

    XMLSerializer xml(out_stream);
    // output GUILayout start element
    xml.openTag(GUILayout_xmlHandler::GUILayoutElement);
    xml.attribute(GUILayout_xmlHandler::GUILayoutVersionAttribute,
                  GUILayout_xmlHandler::NativeVersion);
    
    // write windows
    window.writeXMLToStream(xml);
    // write closing GUILayout element
    xml.closeTag();
}

String WindowManager::getLayoutAsString(const Window& window) const
{
    std::ostringstream str;
    writeLayoutToStream(window, str);

    return String(reinterpret_cast<const encoded_char*>(str.str().c_str()));
}

//----------------------------------------------------------------------------//
void WindowManager::saveLayoutToFile(const Window& window,
                                     const String& filename) const
{
    std::ofstream stream(filename.c_str());

    if (!stream.good())
        CEGUI_THROW(FileIOException(
            "failed to create stream for writing."));

    writeLayoutToStream(window, stream);
}

String WindowManager::generateUniqueWindowName()
{
    const String ret = GeneratedWindowNameBase +
        PropertyHelper<unsigned long>::toString(d_uid_counter);

    // update counter for next time
    unsigned long old_uid = d_uid_counter;
    ++d_uid_counter;

    // log if we ever wrap-around (which should be pretty unlikely)
    if (d_uid_counter < old_uid)
        Logger::getSingleton().logEvent("UID counter for generated Window "
            "names has wrapped around - the fun shall now commence!");

    return ret;
}

/*************************************************************************
	Return a WindowManager::WindowIterator object to iterate over the
	currently defined Windows.
*************************************************************************/
WindowManager::WindowIterator WindowManager::getIterator(void) const
{
	return WindowIterator(d_windowRegistry.begin(), d_windowRegistry.end());
}

/*************************************************************************
    Outputs the names of ALL existing windows to log (DEBUG function).
*************************************************************************/
void WindowManager::DEBUG_dumpWindowNames(String zone) const
{
    Logger::getSingleton().logEvent("WINDOW NAMES DUMP (" + zone + ")");
    Logger::getSingleton().logEvent("-----------------");

    for (WindowVector::const_iterator i = d_windowRegistry.begin();
         i != d_windowRegistry.end();
         ++i)
    {
        Logger::getSingleton().logEvent("Window : " + (*i)->getNamePath());
    }
    Logger::getSingleton().logEvent("-----------------");
}

//----------------------------------------------------------------------------//
void WindowManager::lock()
{
    ++d_lockCount;
}

//----------------------------------------------------------------------------//
void WindowManager::unlock()
{
    if (d_lockCount > 0)
        --d_lockCount;
}

//----------------------------------------------------------------------------//
bool WindowManager::isLocked() const
{
    return d_lockCount != 0;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
