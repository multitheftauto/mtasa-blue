/************************************************************************
	filename: 	CEGUISystem.cpp
	created:	20/2/2004
	author:		Paul D Turner

	purpose:	Implementation of main system object
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
#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "CEGUISystem.h"
#include "CEGUILogger.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIFontManager.h"
#include "CEGUIWindowFactoryManager.h"
#include "CEGUIWindowManager.h"
#include "CEGUISchemeManager.h"
#include "CEGUIMouseCursor.h"
#include "CEGUIWindow.h"
#include "CEGUIImageset.h"
#include "CEGUIExceptions.h"
#include "elements/CEGUIGUISheet.h"
#include "elements/CEGUIDragContainer.h"
#include "elements/CEGUIScrolledContainer.h"
#include "elements/CEGUITooltip.h"
#include "CEGUIScriptModule.h"
#include "CEGUIConfig_xmlHandler.h"
#include "CEGUIDataContainer.h"
#include "CEGUIResourceProvider.h"
#include "CEGUIGlobalEventSet.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "CEGUIPropertyHelper.h"
#include <time.h>

// set up for whichever default xml parser will be used
#ifdef CEGUI_WITH_XERCES
#   include "CEGUIXercesParser.h"
#   define CEGUI_DEFAULT_XMLPARSER     XercesParser
#else
#   include "CEGUITinyXMLParser.h"
#   define CEGUI_DEFAULT_XMLPARSER     TinyXMLParser
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
const String System::EventNamespace("System");

/*!
\brief
	Simple timer class.
*/
class SimpleTimer
{
	clock_t d_baseTime;

public:
	SimpleTimer() : d_baseTime(clock()) {}

	void	restart()	{ d_baseTime = clock(); }
	double	elapsed()	{ return static_cast<double>(clock() - d_baseTime) / CLOCKS_PER_SEC; }
};

/*!
\brief
	Implementation structure used in tracking up & down mouse button inputs in order to generate click, double-click,
	and triple-click events.
*/
struct MouseClickTracker
{
	MouseClickTracker(void) : d_click_count(0), d_click_area(0, 0, 0, 0) {}

	SimpleTimer		d_timer;			//!< Timer used to track clicks for this button.
	int				d_click_count;		//!< count of clicks made so far.
	Rect			d_click_area;		//!< area used to detect multi-clicks
    Window*         d_target_window;    //!< target window for any events generated.
};


struct MouseClickTrackerImpl
{
	MouseClickTracker	click_trackers[MouseButtonCount];
};


/*************************************************************************
	Constants definitions
*************************************************************************/
const char	System::CEGUIConfigSchemaName[]		= "CEGUIConfig.xsd";


/*************************************************************************
	Static Data Definitions
*************************************************************************/
// singleton instance pointer
template<> System* Singleton<System>::ms_Singleton	= NULL;

// click event generation defaults
const double	System::DefaultSingleClickTimeout	= 0.2;
const double	System::DefaultMultiClickTimeout	= 0.33;
const Size		System::DefaultMultiClickAreaSize(12,12);

// event names
const String System::EventGUISheetChanged( (utf8*)"GUISheetChanged" );
const String System::EventSingleClickTimeoutChanged( (utf8*)"SingleClickTimeoutChanged" );
const String System::EventMultiClickTimeoutChanged( (utf8*)"MultiClickTimeoutChanged" );
const String System::EventMultiClickAreaSizeChanged( (utf8*)"MultiClickAreaSizeChanged" );
const String System::EventDefaultFontChanged( (utf8*)"DefaultFontChanged" );
const String System::EventDefaultMouseCursorChanged( (utf8*)"DefaultMouseCursorChanged" );
const String System::EventMouseMoveScalingChanged( (utf8*)"MouseMoveScalingChanged" );


/*************************************************************************
	Constructor
*************************************************************************/
System::System(Renderer* renderer, const utf8* logFile) :
	d_clickTrackerPimpl(new MouseClickTrackerImpl)
{
	constructor_impl(renderer, NULL, NULL, NULL, (const utf8*)"", logFile);
}
/*************************************************************************
	Construct a new System object
*************************************************************************/
System::System(Renderer* renderer, ResourceProvider* resourceProvider,const utf8* logFile) :
	d_clickTrackerPimpl(new MouseClickTrackerImpl)
{
    constructor_impl(renderer, resourceProvider, NULL, NULL, (const utf8*)"", logFile);
}

/*************************************************************************
	Construct a new System object
*************************************************************************/
System::System(Renderer* renderer, ScriptModule* scriptModule, const utf8* configFile) :
	d_clickTrackerPimpl(new MouseClickTrackerImpl)
{
    constructor_impl(renderer, NULL, NULL, scriptModule, configFile, (const utf8*)"CEGUI.log");
}


/*************************************************************************
	Construct a new System object
*************************************************************************/
System::System(Renderer* renderer, ScriptModule* scriptModule, ResourceProvider* resourceProvider, const utf8* configFile) :
	d_clickTrackerPimpl(new MouseClickTrackerImpl)
{
    constructor_impl(renderer, resourceProvider, NULL, scriptModule, configFile, (const utf8*)"CEGUI.log");
}

/*************************************************************************
    Construct a new System object
*************************************************************************/
System::System(Renderer* renderer, XMLParser* xmlParser, const utf8* logFile) :
        d_clickTrackerPimpl(new MouseClickTrackerImpl)
{
    constructor_impl(renderer, NULL, xmlParser, NULL, (const utf8*)"", logFile);
}

/*************************************************************************
    Construct a new System object
*************************************************************************/
System::System(Renderer* renderer, ResourceProvider* resourceProvider, XMLParser* xmlParser, const utf8* logFile) :
        d_clickTrackerPimpl(new MouseClickTrackerImpl)
{
    constructor_impl(renderer, resourceProvider, xmlParser, NULL, (const utf8*)"", logFile);
}

/*************************************************************************
    Construct a new System object
*************************************************************************/
System::System(Renderer* renderer, XMLParser* xmlParser, ScriptModule* scriptModule, const utf8* configFile) :
        d_clickTrackerPimpl(new MouseClickTrackerImpl)
{
    constructor_impl(renderer, NULL, xmlParser, scriptModule, configFile, (const utf8*)"CEGUI.log");
}

/*************************************************************************
    Construct a new System object
*************************************************************************/
System::System(Renderer* renderer, ResourceProvider* resourceProvider, XMLParser* xmlParser, ScriptModule* scriptModule, const utf8* configFile) :
        d_clickTrackerPimpl(new MouseClickTrackerImpl)
{
    constructor_impl(renderer, resourceProvider, xmlParser, scriptModule, configFile, (const utf8*)"CEGUI.log");
}

/*************************************************************************
	Method to do the work of the constructor
*************************************************************************/
void System::constructor_impl(Renderer* renderer, ResourceProvider* resourceProvider,  XMLParser* xmlParser, ScriptModule* scriptModule, const String& configFile, const String& logFile)
{
    // Instantiate logger first (we have no file at this point, but entries will be cached until we do)
    new Logger();

    // Set CEGUI version
    d_strVersion = PropertyHelper::uintToString(CEGUI_VERSION_MAJOR) + "." +
       PropertyHelper::uintToString(CEGUI_VERSION_MINOR) + "." +
       PropertyHelper::uintToString(CEGUI_VERSION_PATCH);

	d_renderer		= renderer;
	d_gui_redraw	= false;
	d_defaultFont	= NULL;
	d_wndWithMouse	= NULL;
	d_activeSheet	= NULL;
	d_modalTarget	= NULL;
	d_sysKeys		= 0;

	d_lshift	= false;
	d_rshift	= false;
	d_lctrl		= false;
	d_rctrl		= false;
    d_ralt      = false;
    d_lalt      = false;

	d_click_timeout		= DefaultSingleClickTimeout;
	d_dblclick_timeout	= DefaultMultiClickTimeout;
	d_dblclick_size		= DefaultMultiClickAreaSize;

	d_defaultMouseCursor = NULL;
	d_scriptModule		 = scriptModule;

	d_mouseScalingFactor = 1.0f;

    // Tooltip setup
    d_defaultTooltip = 0;
    d_weOwnTooltip = false;

	// add events for Sytem object
	addSystemEvents();

    // if there has been a resource provider supplied use that otherwise create one.
    d_resourceProvider = resourceProvider ? resourceProvider : renderer->createResourceProvider();

    // use supplied xml parser if provided, otherwise create one of the defaults
    if (xmlParser)
    {
        d_xmlParser = xmlParser;
        d_ourXmlParser = false;
    }
    else
    {
        d_xmlParser = new CEGUI_DEFAULT_XMLPARSER;
        d_ourXmlParser = true;
    }

    // perform initialisation of XML parser.
    d_xmlParser->initialise();

	// strings we may get from the configuration file.
	String configLogname, configSchemeName, configLayoutName, configInitScript, defaultFontName;

	// now XML is available, read the configuration file (if any)
	if (!configFile.empty())
	{
        // create handler object
        Config_xmlHandler handler;

		// do parsing of xml file
		try
		{
            d_xmlParser->parseXMLFile(handler, configFile, CEGUIConfigSchemaName, "");
		}
		catch(...)
		{
			// cleanup XML stuff
            d_xmlParser->cleanup();
            delete d_xmlParser;

            throw;
		}

        // set the logging level
        Logger::getSingleton().setLoggingLevel(handler.getLoggingLevel());

        // get the strings read
        configLogname		= handler.getLogFilename();
        configSchemeName	= handler.getSchemeFilename();
        configLayoutName	= handler.getLayoutFilename();
        defaultFontName		= handler.getDefaultFontName();
        configInitScript	= handler.getInitScriptFilename();
        d_termScriptName	= handler.getTermScriptFilename();

        // set default resource group if it was specified.
        if (!handler.getDefaultResourceGroup().empty())
        {
            d_resourceProvider->setDefaultResourceGroup(handler.getDefaultResourceGroup());
        }
	}

	// Start up the logger:
	// prefer log filename from config file
	if (!configLogname.empty())
	{
        Logger::getSingleton().setLogFilename(configLogname, false);
	}
	// no log specified in configuration, use default / hard-coded option
	else
	{
        Logger::getSingleton().setLogFilename(logFile, false);
	}

	// beginning main init
	Logger::getSingleton().logEvent((utf8*)"---- Begining CEGUI System initialisation ----");

	// cause creation of other singleton objects
	new ImagesetManager();
	d_fontManager = new FontManager();
	new WindowFactoryManager();
	new WindowManager();
	new SchemeManager();
	new MouseCursor();
	new GlobalEventSet();
    new WidgetLookManager();

    // Add factories for types that the system supports natively
    // (mainly because they do no rendering)
    WindowFactoryManager::getSingleton().addFactory(new GUISheetFactory);
    WindowFactoryManager::getSingleton().addFactory(new DragContainerFactory);
    WindowFactoryManager::getSingleton().addFactory(new ScrolledContainerFactory);

	// GUISheet's name was changed, register an alias so both can be used
	WindowFactoryManager::getSingleton().addWindowTypeAlias((utf8*)"DefaultGUISheet", GUISheet::WidgetTypeName);

	// success - we are created!  Log it for prosperity :)
	Logger::getSingleton().logEvent((utf8*)"CEGUI::System singleton created.");
	Logger::getSingleton().logEvent((utf8*)"---- CEGUI System initialisation completed ----");
    Logger::getSingleton().logEvent((utf8*)"---- Version " + d_strVersion + " ----");
    Logger::getSingleton().logEvent("---- Renderer module is: " + d_renderer->getIdentifierString() + " ----");
    Logger::getSingleton().logEvent("---- XML Parser module is: " + d_xmlParser->getIdentifierString() + " ----");
    Logger::getSingleton().logEvent(d_scriptModule ? "---- Scripting module is: " + d_scriptModule->getIdentifierString() + " ----" : "---- Scripting module is: None ----");
	// subscribe to hear about display mode changes
	d_renderer->subscribeEvent(Renderer::EventDisplaySizeChanged, Event::Subscriber(&CEGUI::System::handleDisplaySizeChange, this));

	// load base scheme
	if (!configSchemeName.empty())
	{
		try
		{
			SchemeManager::getSingleton().loadScheme(configSchemeName, d_resourceProvider->getDefaultResourceGroup());

			// set default font if that was specified also
			if (!defaultFontName.empty())
			{
				setDefaultFont(defaultFontName);
			}

		}
		catch (CEGUI::Exception exc) {}  // catch exception and try to continue anyway

	}

	// load initial layout
	if (!configLayoutName.empty())
	{
		try
		{
			setGUISheet(WindowManager::getSingleton().loadWindowLayout(configLayoutName));
		}
		catch (CEGUI::Exception exc) {}  // catch exception and try to continue anyway

	}

    // Create script module bindings
    if (d_scriptModule)
    {
        d_scriptModule->createBindings();
    }

	// execute start up script
	if (!configInitScript.empty())
	{
		try
		{
			executeScriptFile(configInitScript);
		}
		catch (...) {}  // catch all exceptions and try to continue anyway

	}

}


/*************************************************************************
	Destructor
*************************************************************************/
System::~System(void)
{
	Logger::getSingleton().logEvent((utf8*)"---- Begining CEGUI System destruction ----");

	// execute shut-down script
	if (!d_termScriptName.empty())
	{
		try
		{
			executeScriptFile(d_termScriptName);
		}
		catch (...) {}  // catch all exceptions and continue system shutdown

	}

    // Cleanup script module bindings
    if (d_scriptModule)
    {
        d_scriptModule->destroyBindings();
    }

	// cleanup XML stuff
    if (d_xmlParser)
    {
        d_xmlParser->cleanup();
        if (d_ourXmlParser)
            delete d_xmlParser;
    }

    //
	// perform cleanup in correct sequence
	//
	// destroy windows so it's safe to destroy factories
    WindowManager::getSingleton().destroyAllWindows();
    WindowManager::getSingleton().cleanDeadPool();

	// get pointers to the factories we added
	WindowFactory* guiSheetFactory =
        WindowFactoryManager::getSingleton().getFactory(GUISheet::WidgetTypeName);

    WindowFactory* dragContainerFactory =
        WindowFactoryManager::getSingleton().getFactory(DragContainer::WidgetTypeName);

    WindowFactory* scrolledContainerFactory =
        WindowFactoryManager::getSingleton().getFactory(ScrolledContainer::WidgetTypeName);

    // remove factories so it's safe to unload GUI modules
	WindowFactoryManager::getSingleton().removeAllFactories();

	// destroy factories we created
	delete guiSheetFactory;
    delete dragContainerFactory;
    delete scrolledContainerFactory;

	// cleanup singletons
	delete	SchemeManager::getSingletonPtr();
	delete	WindowManager::getSingletonPtr();
	delete	WindowFactoryManager::getSingletonPtr();
    delete  WidgetLookManager::getSingletonPtr();
	delete	FontManager::getSingletonPtr();
	delete	MouseCursor::getSingletonPtr();
	delete	ImagesetManager::getSingletonPtr();
	delete	GlobalEventSet::getSingletonPtr();

	Logger::getSingleton().logEvent((utf8*)"CEGUI::System singleton destroyed.");
	Logger::getSingleton().logEvent((utf8*)"---- CEGUI System destruction completed ----");
	delete Logger::getSingletonPtr();

	delete d_clickTrackerPimpl;
}


/*************************************************************************
	Render the GUI for this frame
*************************************************************************/
void System::renderGUI(void)
{
	//////////////////////////////////////////////////////////////////////////
	// This makes use of some tricks the Renderer can do so that we do not
	// need to do a full redraw every frame - only when some UI element has
	// changed.
	//
	// Since the mouse is likely to move very often, and in order not to
	// short-circuit the above optimisation, the mouse is not queued, but is
	// drawn directly to the display every frame.
	//////////////////////////////////////////////////////////////////////////

	if (d_gui_redraw)
	{
		d_renderer->resetZValue();
		d_renderer->setQueueingEnabled(true);
		d_renderer->clearRenderList();

		if (d_activeSheet != NULL)
		{
			d_activeSheet->render();
		}

		d_gui_redraw = false;
	}

	d_renderer->doRender();

	// draw mouse
	d_renderer->setQueueingEnabled(false);
	MouseCursor::getSingleton().draw();

    // do final destruction on dead-pool windows
    WindowManager::getSingleton().cleanDeadPool();
}


/*************************************************************************
	Set the active GUI sheet (root) window.
*************************************************************************/
Window* System::setGUISheet(Window* sheet)
{
	Window* old = d_activeSheet;
	d_activeSheet = sheet;

    // Force and update for the area Rects for 'sheet' so they're correct according
    // to the screen size.
    if (sheet != 0)
    {
        WindowEventArgs sheetargs(0);
        sheet->onParentSized(sheetargs);
    }

	// fire event
	WindowEventArgs args(old);
	onGUISheetChanged(args);

	return old;
}


/*************************************************************************
	Set the default font to be used by the system
*************************************************************************/
void System::setDefaultFont(const String& name)
{
	if (name.empty())
	{
		setDefaultFont(NULL);
	}
	else
	{
		setDefaultFont(FontManager::getSingleton().getFont(name));
	}

}


/*************************************************************************
	Set the default font to be used by the system
*************************************************************************/
void System::setDefaultFont(Font* font)
{
	d_defaultFont = font;

	// fire event
	EventArgs args;
	onDefaultFontChanged(args);
}


/*************************************************************************
	Set the image to be used as the default mouse cursor.
*************************************************************************/
void System::setDefaultMouseCursor(const Image* image)
{
    // the default, default, is for nothing!
    if (image == (const Image*)DefaultMouseCursor)
        image = 0;

    // if mouse cursor is set to the current default we *may* need to
    // update its Image immediately (first, we will investigate further!)
    //
    // NB: The reason we do this check, is to allow code to modify the cursor
    // image directly without a call to this member changing the image back
    // again.  However, 'normal' updates to the cursor when the mouse enters
    // a window will, of course, update the mouse image as expected.
    if (MouseCursor::getSingleton().getImage() == d_defaultMouseCursor)
    {
        // does the window containing the mouse use the default cursor?
        if ((d_wndWithMouse) && (0 == d_wndWithMouse->getMouseCursor(false)))
        {
            // default cursor is active, update the image immediately
            MouseCursor::getSingleton().setImage(image);
        }
    }

    // update our pointer for the default mouse cursor image.
    d_defaultMouseCursor = image;

    // fire off event.
    EventArgs args;
    onDefaultMouseCursorChanged(args);
}


/*************************************************************************
	Set the image to be used as the default mouse cursor.
*************************************************************************/
void System::setDefaultMouseCursor(const String& imageset, const String& image_name)
{
	setDefaultMouseCursor(&ImagesetManager::getSingleton().getImageset(imageset)->getImage(image_name));
}


/*************************************************************************
	Return a pointer to the ScriptModule being used for scripting within
	the GUI system.
*************************************************************************/
ScriptModule* System::getScriptingModule(void) const
{
	return d_scriptModule;
}

/*************************************************************************
	Return a pointer to the ResourceProvider being used for within the GUI
    system.
*************************************************************************/
ResourceProvider* System::getResourceProvider(void) const
{
	return d_resourceProvider;
}

/*************************************************************************
	Execute a script file if possible.
*************************************************************************/
void System::executeScriptFile(const String& filename, const String& resourceGroup) const
{
	if (d_scriptModule != NULL)
	{
		try
		{
			d_scriptModule->executeScriptFile(filename, resourceGroup);
		}
		catch(...)
		{
			throw GenericException((utf8*)"System::executeScriptFile - An exception was thrown during the execution of the script file.");
		}

	}
	else
	{
		Logger::getSingleton().logEvent((utf8*)"System::executeScriptFile - the script named '" + filename +"' could not be executed as no ScriptModule is available.", Errors);
	}

}


/*************************************************************************
	Execute a scripted global function if possible.  The function should
	not take any parameters and should return an integer.
*************************************************************************/
int	System::executeScriptGlobal(const String& function_name) const
{
	if (d_scriptModule != NULL)
	{
		try
		{
			return d_scriptModule->executeScriptGlobal(function_name);
		}
		catch(...)
		{
			throw GenericException((utf8*)"System::executeScriptGlobal - An exception was thrown during execution of the scripted function.");
		}

	}
	else
	{
		Logger::getSingleton().logEvent((utf8*)"System::executeScriptGlobal - the global script function named '" + function_name +"' could not be executed as no ScriptModule is available.", Errors);
	}

	return 0;
}


/*************************************************************************
    If possible, execute script code contained in the given
    CEGUI::String object.
*************************************************************************/
void System::executeScriptString(const String& str) const
{
    if (d_scriptModule != NULL)
    {
        try
        {
            d_scriptModule->executeString(str);
        }
        catch(...)
        {
            throw GenericException((utf8*)"System::executeScriptString - An exception was thrown during execution of the script code.");
        }

    }
    else
    {
        Logger::getSingleton().logEvent((utf8*)"System::executeScriptString - the script code could not be executed as no ScriptModule is available.", Errors);
    }
}


/*************************************************************************
	return the current mouse movement scaling factor.
*************************************************************************/
float System::getMouseMoveScaling(void) const
{
	return d_mouseScalingFactor;
}


/*************************************************************************
	Set the current mouse movement scaling factor
*************************************************************************/
void System::setMouseMoveScaling(float scaling)
{
	d_mouseScalingFactor = scaling;

	// fire off event.
	EventArgs args;
	onMouseMoveScalingChanged(args);
}


/*************************************************************************
	Method that injects a mouse movement event into the system
*************************************************************************/
bool System::injectMouseMove(float delta_x, float delta_y)
{
	MouseEventArgs ma(NULL);
	MouseCursor& mouse = MouseCursor::getSingleton();

	ma.moveDelta.d_x = delta_x * d_mouseScalingFactor;
	ma.moveDelta.d_y = delta_y * d_mouseScalingFactor;
	ma.sysKeys = d_sysKeys;
	ma.wheelChange = 0;
	ma.clickCount = 0;

	// move the mouse cursor & update position in args.
	mouse.offsetPosition(ma.moveDelta);
	ma.position = mouse.getPosition();

	Window* dest_window = getTargetWindow(ma.position);

	// if there is no GUI sheet, then there is nowhere to send input
	if (dest_window != NULL)
	{
		if (dest_window != d_wndWithMouse)
		{
			if (d_wndWithMouse != NULL)
			{
				ma.window = d_wndWithMouse;
				d_wndWithMouse->onMouseLeaves(ma);
			}

			d_wndWithMouse = dest_window;
			ma.window = dest_window;
			dest_window->onMouseEnters(ma);
		}

		// ensure event starts as 'not handled'
		ma.handled = false;

		// loop backwards until event is handled or we run out of windows.
		while ((!ma.handled) && (dest_window != NULL))
		{
			ma.window = dest_window;
			dest_window->onMouseMove(ma);
			dest_window = getNextTargetWindow(dest_window);
		}

	}

	return ma.handled;
}


/*************************************************************************
	Method that injects that the mouse is leaves the application window
*************************************************************************/
bool System::injectMouseLeaves(void)
{
	MouseEventArgs ma(NULL);

	// if there is no window that currently contains the mouse, then
	// there is nowhere to send input
	if (d_wndWithMouse != NULL)
	{
		ma.position = MouseCursor::getSingleton().getPosition();
		ma.moveDelta = Vector2(0.0f, 0.0f);
		ma.button = NoButton;
		ma.sysKeys = d_sysKeys;
		ma.wheelChange = 0;
		ma.window = d_wndWithMouse;
		ma.clickCount = 0;

		d_wndWithMouse->onMouseLeaves(ma);
		d_wndWithMouse = NULL;
	}

	return ma.handled;
}


/*************************************************************************
	Method that injects a mouse button down event into the system.
*************************************************************************/
bool System::injectMouseButtonDown(MouseButton button)
{
	// update system keys
	d_sysKeys |= mouseButtonToSyskey(button);

	MouseEventArgs ma(NULL);
	ma.position = MouseCursor::getSingleton().getPosition();
	ma.moveDelta = Vector2(0.0f, 0.0f);
	ma.button = button;
	ma.sysKeys = d_sysKeys;
	ma.wheelChange = 0;

    // find the likely destination for generated events.
    Window* dest_window = getTargetWindow(ma.position);
	
    //
	// Handling for multi-click generation
	//
	MouseClickTracker& tkr = d_clickTrackerPimpl->click_trackers[button];

	tkr.d_click_count++;

    // if multi-click requirements are not met
    if ((tkr.d_timer.elapsed() > d_dblclick_timeout) ||
        (!tkr.d_click_area.isPointInRect(ma.position)) ||
        (tkr.d_target_window != dest_window) ||
        (tkr.d_click_count > 3))
    {
        // reset to single down event.
        tkr.d_click_count = 1;

        // build new allowable area for multi-clicks
        tkr.d_click_area.setPosition(ma.position);
        tkr.d_click_area.setSize(d_dblclick_size);
        tkr.d_click_area.offset(Point(-(d_dblclick_size.d_width / 2), -(d_dblclick_size.d_height / 2)));

        // set target window for click events on this tracker
        tkr.d_target_window = dest_window;
    }

	// set click count in the event args
	ma.clickCount = tkr.d_click_count;

	// loop backwards until event is handled or we run out of windows.
	while ((!ma.handled) && (dest_window != NULL))
	{
		ma.window = dest_window;

        if (dest_window->wantsMultiClickEvents())
        {
            switch (tkr.d_click_count)
            {
            case 1:
                dest_window->onMouseButtonDown(ma);
                break;

            case 2:
                dest_window->onMouseDoubleClicked(ma);
                break;

            case 3:
                dest_window->onMouseTripleClicked(ma);
                break;
            }
        }
        // current target window does not want multi-clicks,
        // so just send a mouse down event instead.
        else
        {
            dest_window->onMouseButtonDown(ma);
        }

		dest_window = getNextTargetWindow(dest_window);
	}

	// reset timer for this tracker.
	tkr.d_timer.restart();

	return ma.handled;
}


/*************************************************************************
	Method that injects a mouse button up event into the system.
*************************************************************************/
bool System::injectMouseButtonUp(MouseButton button)
{
	// update system keys
	d_sysKeys &= ~mouseButtonToSyskey(button);

	MouseEventArgs ma(NULL);
	ma.position = MouseCursor::getSingleton().getPosition();
	ma.moveDelta = Vector2(0.0f, 0.0f);
	ma.button = button;
	ma.sysKeys = d_sysKeys;
	ma.wheelChange = 0;

    // get the tracker that holds the number of down events seen so far for this button
    MouseClickTracker& tkr = d_clickTrackerPimpl->click_trackers[button];
    // set click count in the event args
    ma.clickCount = tkr.d_click_count;

    Window* const initial_dest_window = getTargetWindow(ma.position);
	Window* dest_window = initial_dest_window;

	// loop backwards until event is handled or we run out of windows.
	while ((!ma.handled) && (dest_window != NULL))
	{
		ma.window = dest_window;
		dest_window->onMouseButtonUp(ma);
		dest_window = getNextTargetWindow(dest_window);
	}

	bool wasUpHandled = ma.handled;

    // if requirements for click events are met
    if ((tkr.d_timer.elapsed() <= d_click_timeout) &&
        (tkr.d_click_area.isPointInRect(ma.position)) &&
        (tkr.d_target_window == initial_dest_window))
    {
		ma.handled = false;
        dest_window = initial_dest_window;

		// loop backwards until event is handled or we run out of windows.
		while ((!ma.handled) && (dest_window != NULL))
		{
			ma.window = dest_window;
			dest_window->onMouseClicked(ma);
			dest_window = getNextTargetWindow(dest_window);
		}

	}

	return (ma.handled | wasUpHandled);
}


/*************************************************************************
	Method that injects a key down event into the system.
*************************************************************************/
bool System::injectKeyDown(uint key_code)
{
	// update system keys
	d_sysKeys |= keyCodeToSyskey((Key::Scan)key_code, true);

	KeyEventArgs args(NULL);

	if (d_activeSheet != NULL)
	{
		args.scancode = (Key::Scan)key_code;
		args.sysKeys = d_sysKeys;

		Window* dest = getKeyboardTargetWindow();

		// loop backwards until event is handled or we run out of windows.
		while ((dest != NULL) && (!args.handled))
		{
			args.window = dest;
			dest->onKeyDown(args);
			dest = getNextTargetWindow(dest);
		}

	}

	return args.handled;
}


/*************************************************************************
	Method that injects a key up event into the system.
*************************************************************************/
bool System::injectKeyUp(uint key_code)
{
	// update system keys
	d_sysKeys &= ~keyCodeToSyskey((Key::Scan)key_code, false);

	KeyEventArgs args(NULL);

	if (d_activeSheet != NULL)
	{
		args.scancode = (Key::Scan)key_code;
		args.sysKeys = d_sysKeys;

		Window* dest = getKeyboardTargetWindow();

		// loop backwards until event is handled or we run out of windows.
		while ((dest != NULL) && (!args.handled))
		{
			args.window = dest;
			dest->onKeyUp(args);
			dest = getNextTargetWindow(dest);
		}

	}

	return args.handled;
}


/*************************************************************************
	Method that injects a typed character event into the system.
*************************************************************************/
bool System::injectChar(utf32 code_point)
{
	KeyEventArgs args(NULL);

	if (d_activeSheet != NULL)
	{
		args.codepoint = code_point;
		args.sysKeys = d_sysKeys;

		Window* dest = getKeyboardTargetWindow();

        // Make sure the glyph is loaded for this font
        if ( dest != NULL )
        {
            const Font* destFont = dest->getFont(true);
            String newGlyphCache = destFont->OnGlyphDrawn(code_point);
            if ( newGlyphCache != "" )
            {
                destFont->extendFontGlyphs ( newGlyphCache );
            }
        }

		// loop backwards until event is handled or we run out of windows.
		while ((dest != NULL) && (!args.handled))
		{
			args.window = dest;
			dest->onCharacter(args);
			dest = getNextTargetWindow(dest);
		}

	}

	return args.handled;
}


/*************************************************************************
	Method that injects a mouse-wheel / scroll-wheel event into the system.
*************************************************************************/
bool System::injectMouseWheelChange(float delta)
{
	MouseEventArgs ma(NULL);
	ma.position = MouseCursor::getSingleton().getPosition();
	ma.moveDelta = Vector2(0.0f, 0.0f);
	ma.button = NoButton;
	ma.sysKeys = d_sysKeys;
	ma.wheelChange = delta;
	ma.clickCount = 0;

	Window* dest_window = getTargetWindow(ma.position);

	// loop backwards until event is handled or we run out of windows.
	while ((!ma.handled) && (dest_window != NULL))
	{
		ma.window = dest_window;
		dest_window->onMouseWheel(ma);
		dest_window = getNextTargetWindow(dest_window);
	}

	return ma.handled;
}


/*************************************************************************
	Method that injects a new position for the mouse cursor.
*************************************************************************/
bool System::injectMousePosition(float x_pos, float y_pos)
{
	// set new mouse position
	MouseCursor::getSingleton().setPosition(Point(x_pos, y_pos));

	// do the real work
	return injectMouseMove(0, 0);
}


/*************************************************************************
	Method to inject time pulses into the system.
*************************************************************************/
bool System::injectTimePulse(float timeElapsed)
{
	if (d_activeSheet != NULL)
	{
		d_activeSheet->update(timeElapsed);
	}

	return true;
}


/*************************************************************************
	Return window that should get mouse inouts when mouse it at 'pt'
*************************************************************************/
Window*	System::getTargetWindow(const Point& pt) const
{
	Window* dest_window = NULL;

	// if there is no GUI sheet, then there is nowhere to send input
	if (d_activeSheet != NULL)
	{
		dest_window = Window::getCaptureWindow();

		if (dest_window == NULL)
		{
			dest_window = d_activeSheet->getTargetChildAtPosition(pt);

			if (dest_window == NULL)
			{
				dest_window = d_activeSheet;
			}

		}
		else
		{
            if (dest_window->distributesCapturedInputs())
            {
                Window* child_window = dest_window->getTargetChildAtPosition(pt);

                if (child_window != NULL)
                {
                    dest_window = child_window;
                }

            }

		}

		// modal target overrules
		if (d_modalTarget != NULL && dest_window != d_modalTarget)
		{
			if (!dest_window->isAncestor(d_modalTarget))
			{
				dest_window = d_modalTarget;
			}

		}

	}

	return dest_window;
}


/*************************************************************************
	Return window that should receive keyboard input
*************************************************************************/
Window* System::getKeyboardTargetWindow(void) const
{
	Window* target = NULL;

	if (d_modalTarget == NULL)
	{
		target = d_activeSheet->getActiveChild();
	}
	else
	{
		target = d_modalTarget->getActiveChild();
		if (target == NULL)
		{
			target = d_modalTarget;
		}
	}

	return target;
}


/*************************************************************************
	Return the next window that should receive input in the chain
*************************************************************************/
Window* System::getNextTargetWindow(Window* w) const
{
	// if we have not reached the modal target, return the parent
	if (w != d_modalTarget)
	{
		return w->getParent();
	}

	// otherwise stop now
	return NULL;
}


/*************************************************************************
	Translate a MouseButton value into the corresponding SystemKey value
*************************************************************************/
SystemKey System::mouseButtonToSyskey(MouseButton btn) const
{
	switch (btn)
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
		throw InvalidRequestException((utf8*)"System::mouseButtonToSyskey - the parameter 'btn' is not a valid MouseButton value.");
	}
}


/*************************************************************************
	Translate a Key::Scan value into the corresponding SystemKey value
*************************************************************************/
SystemKey System::keyCodeToSyskey(Key::Scan key, bool direction)
{
	switch (key)
	{
	case Key::LeftShift:
		d_lshift = direction;

		if (!d_rshift)
		{
			return Shift;
		}
		break;

	case Key::RightShift:
		d_rshift = direction;

		if (!d_lshift)
		{
			return Shift;
		}
		break;


	case Key::LeftControl:
		d_lctrl = direction;

		if (!d_rctrl)
		{
			return Control;
		}
		break;

	case Key::RightControl:
		d_rctrl = direction;

		if (!d_lctrl)
		{
			return Control;
		}
		break;

	case Key::LeftAlt:
		d_lalt = direction;

		if (!d_ralt)
		{
			return Alt;
		}
		break;

	case Key::RightAlt:
		d_ralt = direction;

		if (!d_lalt)
		{
			return Alt;
		}
		break;

    default:
        break;
	}

	// if not a system key or overall state unchanged, return 0.
	return (SystemKey)0;
}


System&	System::getSingleton(void)
{
	return Singleton<System>::getSingleton();
}


System*	System::getSingletonPtr(void)
{
	return Singleton<System>::getSingletonPtr();
}



/*************************************************************************
	Set the timeout to be used for the generation of single-click events.
*************************************************************************/
void System::setSingleClickTimeout(double timeout)
{
	d_click_timeout = timeout;

	// fire off event.
	EventArgs args;
	onSingleClickTimeoutChanged(args);
}


/*************************************************************************
	Set the timeout to be used for the generation of multi-click events.
*************************************************************************/
void System::setMultiClickTimeout(double timeout)
{
	d_dblclick_timeout = timeout;

	// fire off event.
	EventArgs args;
	onMultiClickTimeoutChanged(args);
}


/*************************************************************************
	Set the size of the allowable mouse movement tolerance used when
	generating multi-click events.
*************************************************************************/
void System::setMultiClickToleranceAreaSize(const Size&	sz)
{
	d_dblclick_size = sz;

	// fire off event.
	EventArgs args;
	onMultiClickAreaSizeChanged(args);
}


/*************************************************************************
	add events for the System object
*************************************************************************/
void System::addSystemEvents(void)
{
	addEvent(EventGUISheetChanged);
	addEvent(EventSingleClickTimeoutChanged);
	addEvent(EventMultiClickTimeoutChanged);
	addEvent(EventMultiClickAreaSizeChanged);
	addEvent(EventDefaultFontChanged);
	addEvent(EventDefaultMouseCursorChanged);
	addEvent(EventMouseMoveScalingChanged);
}


/*************************************************************************
	Handler called when the main system GUI Sheet (or root window) is changed
*************************************************************************/
void System::onGUISheetChanged(WindowEventArgs& e)
{
	fireEvent(EventGUISheetChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the single-click timeout value is changed.
*************************************************************************/
void System::onSingleClickTimeoutChanged(EventArgs& e)
{
	fireEvent(EventSingleClickTimeoutChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the multi-click timeout value is changed.
*************************************************************************/
void System::onMultiClickTimeoutChanged(EventArgs& e)
{
	fireEvent(EventMultiClickTimeoutChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the size of the multi-click tolerance area is
	changed.
*************************************************************************/
void System::onMultiClickAreaSizeChanged(EventArgs& e)
{
	fireEvent(EventMultiClickAreaSizeChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the default system font is changed.
*************************************************************************/
void System::onDefaultFontChanged(EventArgs& e)
{
	fireEvent(EventDefaultFontChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the default system mouse cursor image is changed.
*************************************************************************/
void System::onDefaultMouseCursorChanged(EventArgs& e)
{
	fireEvent(EventDefaultMouseCursorChanged, e, EventNamespace);
}


/*************************************************************************
	Handler called when the mouse movement scaling factor is changed.
*************************************************************************/
void System::onMouseMoveScalingChanged(EventArgs& e)
{
	fireEvent(EventMouseMoveScalingChanged, e, EventNamespace);
}


/*************************************************************************
	Handler method for display size change notifications
*************************************************************************/
bool System::handleDisplaySizeChange(const EventArgs& e)
{
	// notify the imageset/font manager of the size change
	Size new_sz = getRenderer()->getSize();
	ImagesetManager::getSingleton().notifyScreenResolution(new_sz);
	FontManager::getSingleton().notifyScreenResolution(new_sz);

	// notify gui sheet / root if size change, event propagation will ensure everything else
	// gets updated as required.
	if (d_activeSheet != NULL)
	{
		WindowEventArgs args(NULL);
		d_activeSheet->onParentSized(args);
	}

	return true;
}


/*************************************************************************
	Internal method used to inform the System object whenever a window is
	destroyed, so that System can perform any required housekeeping.
*************************************************************************/
void System::notifyWindowDestroyed(const Window* window)
{
	if (d_wndWithMouse == window)
	{
		d_wndWithMouse = NULL;
	}

	if (d_activeSheet == window)
	{
		d_activeSheet = NULL;
	}

	if (d_modalTarget == window)
	{
		d_modalTarget = NULL;
	}

}

void System::setTooltip(Tooltip* tooltip)
{
    // destroy current custom tooltip if one exists and we created it
    if (d_defaultTooltip && d_weOwnTooltip)
        WindowManager::getSingleton().destroyWindow(d_defaultTooltip);

    // set new custom tooltip 
    d_weOwnTooltip = false;
    d_defaultTooltip = tooltip;
}

void System::setTooltip(const String& tooltipType)
{
    // destroy current tooltip if one exists and we created it
    if (d_defaultTooltip && d_weOwnTooltip)
        WindowManager::getSingleton().destroyWindow(d_defaultTooltip);

    if (tooltipType.empty())
    {
        d_defaultTooltip = 0;
        d_weOwnTooltip = false;
    }
    else
    {
        try
        {
            d_defaultTooltip = static_cast<Tooltip*>(WindowManager::getSingleton().createWindow(tooltipType, "CEGUI::System::default__auto_tooltip__"));
            d_weOwnTooltip = true;
        }
        catch(UnknownObjectException x)
        {
            d_defaultTooltip = 0;
            d_weOwnTooltip = false;
        }
    }
}

} // End of  CEGUI namespace section
