/************************************************************************
	filename: 	CEGUISystem.h
	created:	20/2/2004
	author:		Paul D Turner

	purpose:	Defines interface for main GUI system class
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
#ifndef _CEGUISystem_h_
#define _CEGUISystem_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUISingleton.h"
#include "CEGUIRenderer.h"
#include "CEGUIMouseCursor.h"
#include "CEGUIInputEvent.h"
#include "CEGUIResourceProvider.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4275)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
//! Implementation struct that tracks and controls multiclick for mouse buttons.
struct MouseClickTrackerImpl;


/*!
\brief
	The System class is the CEGUI class that provides access to all other elements in this system.

	This object must be created by the client application.  The System object requires that you pass it
	an initialised Renderer object which it can use to interface to whatever rendering system will be
	used to display the GUI imagery.
*/
class CEGUIEXPORT System : public Singleton<System>, public EventSet
{
public:
	static const String EventNamespace;				//!< Namespace for global events

	/*************************************************************************
		Constants
	*************************************************************************/
	static const double		DefaultSingleClickTimeout;		//!< Default timeout for generation of single click events.
	static const double		DefaultMultiClickTimeout;		//!< Default timeout for generation of multi-click events.
	static const Size		DefaultMultiClickAreaSize;		//!< Default allowable mouse movement for multi-click event generation.

	// event names
	static const String EventGUISheetChanged;				//!< Name of event fired whenever the GUI sheet is changed.
	static const String EventSingleClickTimeoutChanged;	//!< Name of event fired when the single-click timeout is changed.
	static const String EventMultiClickTimeoutChanged;	//!< Name of event fired when the multi-click timeout is changed.
	static const String EventMultiClickAreaSizeChanged;	//!< Name of event fired when the size of the multi-click tolerance area is changed.
	static const String EventDefaultFontChanged;			//!< Name of event fired when the default font changes.
	static const String EventDefaultMouseCursorChanged;	//!< Name of event fired when the default mouse cursor changes.
	static const String EventMouseMoveScalingChanged;		//!< Name of event fired when the mouse move scaling factor changes.


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructs a System object.

	\param renderer
		Pointer to the valid Renderer object that will be used to render GUI imagery

	\param logFile
		pointer to a utf8 encoded buffer containing the name to use for the log file.
	*/
	System(Renderer* renderer, const utf8* logFile = (const utf8*)"CEGUI.log");

	/*!
	\brief
		Construct a new System object

	\param renderer
		Pointer to the valid Renderer object that will be used to render GUI imagery

	\param resourceProvider
		Pointer to a ResourceProvider object.

	\param logFile
		pointer to a utf8 encoded buffer containing the name to use for the log file.
	*/
    System(Renderer* renderer, ResourceProvider* resourceProvider, const utf8* logFile = (const utf8*)"CEGUI.log");


	/*!
	\brief
		Construct a new System object

	\param renderer
		Pointer to the valid Renderer object that will be used to render GUI imagery

	\param scriptModule
		Pointer to a ScriptModule object.  may be NULL.

	\param configFile
		pointer to a utf8 encoded buffer containing the name to use for the configuration file.
	*/
	System(Renderer* renderer, ScriptModule* scriptModule, const utf8* configFile = (const utf8*)"cegui.config");


	/*!
	\brief
		Construct a new System object

	\param renderer
		Pointer to the valid Renderer object that will be used to render GUI imagery

	\param scriptModule
		Pointer to a ScriptModule object.  may be NULL.

	\param resourceProvider
		Pointer to a ResourceProvider object.

	\param configFile
		pointer to a utf8 encoded buffer containing the name to use for the configuration file.
	*/
	System(Renderer* renderer, ScriptModule* scriptModule, ResourceProvider* resourceProvider, const utf8* configFile = (const utf8*)"cegui.config");


    /*!
    \brief
        Constructs a System object.

    \param renderer
        Pointer to the valid Renderer object that will be used to render GUI imagery

    \param xmlParser
        Pointer to a valid XMLParser object to be used when parsing XML files, or NULL to use a default parser.

    \param logFile
        pointer to a utf8 encoded buffer containing the name to use for the log file.
    */
    System(Renderer* renderer, XMLParser* xmlParser, const utf8* logFile = (const utf8*)"CEGUI.log");

    
    /*!
    \brief
        Constructs a System object.

    \param renderer
        Pointer to the valid Renderer object that will be used to render GUI imagery

    \param resourceProvider
        Pointer to a ResourceProvider object.
    
    \param xmlParser
        Pointer to a valid XMLParser object to be used when parsing XML files, or NULL to use a default parser.

    \param logFile
        pointer to a utf8 encoded buffer containing the name to use for the log file.
    */
    System(Renderer* renderer, ResourceProvider* resourceProvider, XMLParser* xmlParser, const utf8* logFile = (const utf8*)"CEGUI.log");

    
    /*!
    \brief
        Construct a new System object

    \param renderer
        Pointer to the valid Renderer object that will be used to render GUI imagery

    \param xmlParser
        Pointer to a valid XMLParser object to be used when parsing XML files, or NULL to use a default parser.
    
    \param scriptModule
        Pointer to a ScriptModule object.  may be NULL.

    \param configFile
        pointer to a utf8 encoded buffer containing the name to use for the configuration file.
    */
    System(Renderer* renderer, XMLParser* xmlParser, ScriptModule* scriptModule, const utf8* configFile = (const utf8*)"cegui.config");

        
    /*!
    \brief
        Construct a new System object

    \param renderer
        Pointer to the valid Renderer object that will be used to render GUI imagery

    \param resourceProvider
        Pointer to a ResourceProvider object.
    
    \param xmlParser
        Pointer to a valid XMLParser object to be used when parsing XML files, or NULL to use a default parser.
    
    \param scriptModule
        Pointer to a ScriptModule object.  may be NULL.

    \param configFile
        pointer to a utf8 encoded buffer containing the name to use for the configuration file.
    */
    System(Renderer* renderer, ResourceProvider* resourceProvider, XMLParser* xmlParser, ScriptModule* scriptModule, const utf8* configFile = (const utf8*)"cegui.config");


	/*!
	\brief
		Destructor for System objects.
	*/
	~System(void);


	/*!
	\brief
		Return a pointer to the Renderer object being used by the system

	\return
		Pointer to the Renderer object used by the system.
	*/
	Renderer*	getRenderer(void) const			{return d_renderer;}

	/*!
	\brief
		Return a pointer to the FontManager object being used by the system

	\return
		Pointer to the Renderer object used by the system.
	*/
	FontManager*	getFontManager(void) const			{return d_fontManager;}


	/*!
	\brief
		Return singleton System object

	\return
		Singleton System object
	*/
	static	System&	getSingleton(void);


	/*!
	\brief
		Return pointer to singleton System object

	\return
		Pointer to singleton System object
	*/
	static	System*	getSingletonPtr(void);


	/*!
	\brief
		Set the default font to be used by the system

	\param name
		String object containing the name of the font to be used as the system default.

	\return
		Nothing.
	*/
	void	setDefaultFont(const String& name);


	/*!
	\brief
		Set the default font to be used by the system

	\param font
		Pointer to the font to be used as the system default.

	\return
		Nothing.
	*/
	void	setDefaultFont(Font* font);


	/*!
	\brief
		Return a pointer to the default Font for the GUI system

	\return
		Pointer to a Font object that is the default font in the system.
	*/
	Font*	getDefaultFont(void) const				{return d_defaultFont;}


	/*!
	\brief
		Causes a full re-draw next time renderGUI() is called

	\return
		Nothing
	*/
	void	signalRedraw()		{d_gui_redraw = true;}


	/*!
	\brief
		Return a boolean value to indicate whether a full re-draw is requested next time renderGUI() is called.

	\return
		true if a re-draw has been requested
	*/
	bool	isRedrawRequested() const		{return d_gui_redraw;}


	/*!
	\brief
		Render the GUI

		Depending upon the internal state, this may either re-use rendering from last time, or trigger a full re-draw from all elements.

	\return
		Nothing
	*/
	void	renderGUI(void);


	/*!
	\brief
		Set the active GUI sheet (root) window.

	\param sheet
		Pointer to a Window object that will become the new GUI 'root'

	\return
		Pointer to the window that was previously set as the GUI root.
	*/
	Window*	setGUISheet(Window* sheet);


	/*!
	\brief
		Return a pointer to the active GUI sheet (root) window.

	\return
		Pointer to the window object that has been set as the GUI root element.
	*/
	Window*	getGUISheet(void) const		{return d_activeSheet;}


	/*!
	\brief
		Return the current timeout for generation of single-click events.

		A single-click is defined here as a button being pressed and then released.

	\return
		double value equal to the current single-click timeout value.
	*/
	double	getSingleClickTimeout(void) const		{return d_click_timeout;}


	/*!
	\brief
		Return the current timeout for generation of multi-click events.

		A multi-click event is a double-click, or a triple-click.  The value returned
		here is the maximum allowable time between mouse button down events for which
		a multi-click event will be generated.

	\return
		double value equal to the current multi-click timeout value.
	*/
	double	getMultiClickTimeout(void) const		{return d_dblclick_timeout;}


	/*!
	\brief
		Return the size of the allowable mouse movement tolerance used when generating multi-click events.

		This size defines an area with the mouse at the centre.  The mouse must stay within the tolerance defined
		for a multi-click (double click, or triple click) event to be generated.

	\return
		Size object describing the current multi-click tolerance area size.
	*/
	const Size&	getMultiClickToleranceAreaSize(void) const		{return d_dblclick_size;}


	/*!
	\brief
		Set the timeout used for generation of single-click events.

		A single-click is defined here as a button being pressed and then released.

	\param timeout
		double value equal to the single-click timeout value to be used from now onwards.

	\return
		Nothing.
	*/
	void	setSingleClickTimeout(double timeout);


	/*!
	\brief
		Set the timeout to be used for the generation of multi-click events.

		A multi-click event is a double-click, or a triple-click.  The value returned
		here is the maximum allowable time between mouse button down events for which
		a multi-click event will be generated.

	\param timeout
		double value equal to the multi-click timeout value to be used from now onwards.

	\return
		Nothing.
	*/
	void setMultiClickTimeout(double timeout);


	/*!
	\brief
		Set the size of the allowable mouse movement tolerance used when generating multi-click events.

		This size defines an area with the mouse at the centre.  The mouse must stay within the tolerance defined
		for a multi-click (double click, or triple click) event to be generated.

	\param sz
		Size object describing the multi-click tolerance area size to be used.

	\return
		Nothing.
	*/
	void setMultiClickToleranceAreaSize(const Size&	sz);


	/*!
	\brief
		Return the currently set default mouse cursor image

	\return
		Pointer to the current default image used for the mouse cursor.  May return NULL if default cursor has not been set,
		or has intentionally been set to NULL - which results in a blank default cursor.
	*/
	const Image*	getDefaultMouseCursor(void) const	{return d_defaultMouseCursor;}


	/*!
	\brief
		Set the image to be used as the default mouse cursor.

	\param image
		Pointer to an image object that is to be used as the default mouse cursor.  To have no cursor rendered by default, you
		can specify NULL here.

	\return
		Nothing.
	*/
	void	setDefaultMouseCursor(const Image* image);


	/*!
	\brief
		Set the image to be used as the default mouse cursor.

	\param image
		One of the MouseCursorImage enumerated values.

	\return
		Nothing.
	*/
	void	setDefaultMouseCursor(MouseCursorImage image)		{setDefaultMouseCursor((const Image*)image);}


	/*!
	\brief
		Set the image to be used as the default mouse cursor.

	\param imageset
		String object that contains the name of the Imageset  that contains the image to be used.

	\param image_name
		String object that contains the name of the Image on \a imageset that is to be used.

	\return
		Nothing.

	\exception UnknownObjectException	thrown if \a imageset is not known, or if \a imageset contains no Image named \a image_name.
	*/
	void	setDefaultMouseCursor(const String& imageset, const String& image_name);


	/*!
	\brief
		Return the Window object that the mouse is presently within

	\return
		Pointer to the Window object that currently contains the mouse cursor, or NULL if none.
	*/
	Window*	getWindowContainingMouse(void) const	{return d_wndWithMouse;}


	/*!
	\brief
		Return a pointer to the ScriptModule being used for scripting within the GUI system.

	\return
		Pointer to a ScriptModule based object.
	*/
	ScriptModule*	getScriptingModule(void) const;

	/*!
	\brief
		Return a pointer to the ResourceProvider being used within the GUI system.

	\return
		Pointer to a ResourceProvider based object.
	*/
	ResourceProvider* getResourceProvider(void) const;

	/*!
	\brief
		Execute a script file if possible.

	\param filename
		String object holding the filename of the script file that is to be executed
		
	\param resourceGroup
		Resource group identifier to be passed to the ResourceProvider when loading the script file.
	*/
	void	executeScriptFile(const String& filename, const String& resourceGroup = "") const;


	/*!
	\brief
		Execute a scripted global function if possible.  The function should not take any parameters and should return an integer.

	\param function_name
		String object holding the name of the function, in the global script environment, that
		is to be executed.

	\return
		The integer value returned from the script function.
	*/
	int		executeScriptGlobal(const String& function_name) const;


    /*!
    \brief
        If possible, execute script code contained in the given CEGUI::String object.

    \param str
        String object holding the valid script code that should be executed.

    \return
        Nothing.
    */
    void executeScriptString(const String& str) const;


    /*!
	\brief
		return the current mouse movement scaling factor.

	\return
		float value that is equal to the currently set mouse movement scaling factor.  Defaults to 1.0f.
	*/
	float	getMouseMoveScaling(void) const;


	/*!
	\brief
		Set the current mouse movement scaling factor

	\param scaling
		float value specifying the scaling to be applied to mouse movement inputs.

	\return
		nothing.
	*/
	void	setMouseMoveScaling(float scaling);


	/*!
	\brief
		Internal method used to inform the System object whenever a window is destroyed, so that System can perform any required
		housekeeping.

	\note
		This method is not intended for client code usage.  If you use this method anything can, and probably will, go wrong!
	*/
	void	notifyWindowDestroyed(const Window* window);


    /*!
    \brief
        Return the current system keys value.

    \return
        uint value representing a combination of the SystemKey bits.
    */
    uint    getSystemKeys(void) const   { return d_sysKeys; }

    /*!
    \brief
        Return the XMLParser object.
     */
    XMLParser* getXMLParser(void) const     { return d_xmlParser; }


    /*!
    \brief
        Set the system default Tooltip object.  This value may be NULL to indicate that no default Tooltip will be
        available.

    \param tooltip
        Pointer to a valid Tooltip based object which should be used as the default tooltip for the system, or NULL to
        indicate that no system default tooltip is required.  Note that when passing a pointer to a Tooltip object,
        ownership of the Tooltip does not pass to System.

    \return
        Nothing.
    */
    void setTooltip(Tooltip* tooltip);

    /*!
    \brief
        Set the system default Tooltip to be used by specifying a Window type.

        System will internally attempt to create an instance of the specified window type (which must be
        derived from the base Tooltip class).  If the Tooltip creation fails, the error is logged and no
        system default Tooltip will be available.

    \param tooltipType
        String object holding the name of the Tooltip based Window type which should be used as the Tooltip for
        the system default.

    \return
        Nothing.
    */
    void setTooltip(const String& tooltipType);

    /*!
    \brief
        return a poiter to the system default tooltip.  May return 0.

    \return
        Pointer to the current system default tooltip.  May return 0 if
        no system default tooltip is available.
     */
    Tooltip* getDefaultTooltip(void) const  { return d_defaultTooltip; }


	/*!
	\brief
		Internal method to directly set the current modal target.

	\note
		This method is called internally by Window, and must be used by client code.
		Doing so will most likely not have the expected results.
	*/
	void setModalTarget(Window* target)		{d_modalTarget = target;}

	/*!
	\brief
		Return a pointer to the Window that is currently the modal target.

	\return
		Pointer to the current modal target. NULL if there is no modal target.
	*/
	Window* getModalTarget(void) const		{return d_modalTarget;}


	/*************************************************************************
		Input injection interface
	*************************************************************************/
	/*!
	\brief
		Method that injects a mouse movement event into the system

	\param delta_x
		amount the mouse moved on the x axis.

	\param delta_y
		amount the mouse moved on the y axis.

	\return
		- true if the input was processed by the gui system.
		- false if the input was not processed by the gui system.
	*/
	bool	injectMouseMove(float delta_x, float delta_y);


	/*!
	\brief
		Method that injects that the mouse has left the application window

	\return
		- true if the generated mouse move event was handled.
		- false if the generated mouse move event was not handled.
	*/
	bool	injectMouseLeaves(void);


	/*!
	\brief
		Method that injects a mouse button down event into the system.

	\param button
		One of the MouseButton values indicating which button was pressed.

	\return
		- true if the input was processed by the gui system.
		- false if the input was not processed by the gui system.
	*/
	bool	injectMouseButtonDown(MouseButton button);


	/*!
	\brief
		Method that injects a mouse button up event into the system.

	\param button
		One of the MouseButton values indicating which button was released.

	\return
		- true if the input was processed by the gui system.
		- false if the input was not processed by the gui system.
	*/
	bool	injectMouseButtonUp(MouseButton button);


	/*!
	\brief
		Method that injects a key down event into the system.

	\param key_code
		uint value indicating which key was pressed.

	\return
		- true if the input was processed by the gui system.
		- false if the input was not processed by the gui system.
	*/
	bool	injectKeyDown(uint key_code);


	/*!
	\brief
		Method that injects a key up event into the system.

	\param key_code
		uint value indicating which key was released.

	\return
		- true if the input was processed by the gui system.
		- false if the input was not processed by the gui system.
	*/
	bool	injectKeyUp(uint key_code);


	/*!
	\brief
		Method that injects a typed character event into the system.

	\param code_point
		Unicode code point of the character that was typed.

	\return
		- true if the input was processed by the gui system.
		- false if the input was not processed by the gui system.
	*/
	bool	injectChar(utf32 code_point);


	/*!
	\brief
		Method that injects a mouse-wheel / scroll-wheel event into the system.

	\param delta
		float value representing the amount the wheel moved.

	\return
		- true if the input was processed by the gui system.
		- false if the input was not processed by the gui system.
	*/
	bool	injectMouseWheelChange(float delta);


	/*!
	\brief
		Method that injects a new position for the mouse cursor.

	\param x_pos
		New absolute pixel position of the mouse cursor on the x axis.

	\param y_pos
		New absolute pixel position of the mouse cursoe in the y axis.

	\return
		- true if the generated mouse move event was handled.
		- false if the generated mouse move event was not handled.
	*/
	bool	injectMousePosition(float x_pos, float y_pos);


	/*!
	\brief
		Method to inject time pulses into the system.

	\param timeElapsed
		float value indicating the amount of time passed, in seconds, since the last time this method was called.

	\return
		Currently, this method always returns true.
	*/
	bool	injectTimePulse(float timeElapsed);

    void                            SetGuiWorkingDirectory      ( const String& strDir )    { d_guiWorkingDirectory = strDir; }
    String                          GetGuiWorkingDirectory      ( void )                    { return d_guiWorkingDirectory; }

    static void                     SetBidiEnabled              ( bool bEnabled ) { ms_bBidiEnabled = bEnabled; }
    static bool                     ms_bBidiEnabled;
private:
    // unimplemented constructors / assignment
    System(const System& obj);
    System& operator=(const System& obj);

	/*************************************************************************
		Implementation Constants
	*************************************************************************/
	static const char	CEGUIConfigSchemaName[];			//!< Filename of the XML schema used for validating Config files.


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Given Point \a pt, return a pointer to the Window that should receive a mouse input if \a pt is the mouse location.

	\param pt
		Point object describing a screen location in pixels.

	\return
		Pointer to a Window object that should receive mouse input with the system in its current state and the mouse at location \a pt.
	*/
	Window*	getTargetWindow(const Point& pt) const;


	/*!
	\brief
		Return a pointer to the Window that should receive keyboard input considering the current modal target.

	\return
		Pointer to a Window object that should receive the keyboard input.
	*/
	Window* getKeyboardTargetWindow(void) const;


	/*!
	\brief
		Return a pointer to the next window that is to receive the input if the given Window did not use it.

	\param w
		Pointer to the Window that just received the input.

	\return
		Pointer to the next window to receive the input.
	*/
	Window* getNextTargetWindow(Window* w) const;


	/*!
	\brief
		Translate a MouseButton value into the corresponding SystemKey value

	\param btn
		MouseButton value describing the value to be converted

	\return
		SystemKey value that corresponds to the same button as \a btn
	*/
	SystemKey	mouseButtonToSyskey(MouseButton btn) const;


	/*!
	\brief
		Translate a Key::Scan value into the corresponding SystemKey value.

		This takes key direction into account, since we map two keys onto one value.

	\param key
		Key::Scan value describing the value to be converted

	\param direction
		true if the key is being pressed, false if the key is being released.

	\return
		SystemKey value that corresponds to the same key as \a key, or 0 if key was not a system key.
	*/
	SystemKey	keyCodeToSyskey(Key::Scan key, bool direction);


	/*!
	\brief
		Method to do the work of the constructor
	*/
       void	constructor_impl(Renderer* renderer, ResourceProvider* resourceProvider, XMLParser* xmlParser, ScriptModule* scriptModule, const String& configFile, const String& logFile);


	/*!
	\brief
		add events for the System object
	*/
	void	addSystemEvents(void);


	/*!
	\brief
		Handler method for display size change notifications
	*/
	bool	handleDisplaySizeChange(const EventArgs& e);


	/*************************************************************************
		Handlers for System events
	*************************************************************************/
	/*!
	\brief
		Handler called when the main system GUI Sheet (or root window) is changed.

		\a e is a WindowEventArgs with 'window' set to the old root window.
	*/
	void	onGUISheetChanged(WindowEventArgs& e);


	/*!
	\brief
		Handler called when the single-click timeout value is changed.
	*/
	void	onSingleClickTimeoutChanged(EventArgs& e);


	/*!
	\brief
		Handler called when the multi-click timeout value is changed.
	*/
	void	onMultiClickTimeoutChanged(EventArgs& e);


	/*!
	\brief
		Handler called when the size of the multi-click tolerance area is changed.
	*/
	void	onMultiClickAreaSizeChanged(EventArgs& e);


	/*!
	\brief
		Handler called when the default system font is changed.
	*/
	void	onDefaultFontChanged(EventArgs& e);


	/*!
	\brief
		Handler called when the default system mouse cursor image is changed.
	*/
	void	onDefaultMouseCursorChanged(EventArgs& e);


	/*!
	\brief
		Handler called when the mouse movement scaling factor is changed.
	*/
	void	onMouseMoveScalingChanged(EventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	Renderer*	d_renderer;			//!< Holds the pointer to the Renderer object given to us in the constructor
    ResourceProvider* d_resourceProvider;      //!< Holds the pointer to the ResourceProvider object given to us by the renderer or the System constructor.
	Font*		d_defaultFont;		//!< Holds a pointer to the default GUI font.
    FontManager* d_fontManager;     //!< Holds a pointer to the FontManager object in the constructor
	bool		d_gui_redraw;		//!< True if GUI should be re-drawn, false if render should re-use last times queue.

	Window*		d_wndWithMouse;		//!< Pointer to the window that currently contains the mouse.
	Window*		d_activeSheet;		//!< The active GUI sheet (root window)
	Window*		d_modalTarget;		//!< Pointer to the window that is the current modal target. NULL is there is no modal target.

	String d_strVersion;    //!< CEGUI version

	uint		d_sysKeys;			//!< Current set of system keys pressed (in mk1 these were passed in, here we track these ourself).
	bool		d_lshift;			//!< Tracks state of left shift.
	bool		d_rshift;			//!< Tracks state of right shift.
	bool		d_lctrl;			//!< Tracks state of left control.
	bool		d_rctrl;			//!< Tracks state of right control.
	bool		d_lalt;				//!< Tracks state of left alt.
	bool		d_ralt;				//!< Tracks state of right alt.

	double		d_click_timeout;	//!< Timeout value, in seconds, used to generate a single-click (button down then up)
	double		d_dblclick_timeout;	//!< Timeout value, in seconds, used to generate multi-click events (botton down, then up, then down, and so on).
	Size		d_dblclick_size;	//!< Size of area the mouse can move and still make multi-clicks.

	MouseClickTrackerImpl* const	d_clickTrackerPimpl;		//!< Tracks mouse button click generation.

	// mouse cursor related
	const Image*	d_defaultMouseCursor;		//!< Image to be used as the default mouse cursor.

	// scripting
	ScriptModule*	d_scriptModule;			//!< Points to the scripting support module.
	String			d_termScriptName;		//!< Name of the script to run upon system shutdown.

    String          d_guiWorkingDirectory;

	float	d_mouseScalingFactor;			//!< Scaling applied to mouse movement inputs.

    XMLParser*  d_xmlParser;        //!< XMLParser object we use to process xml files.
    bool        d_ourXmlParser;     //!< true when we created the xml parser.

    Tooltip* d_defaultTooltip;      //!< System default tooltip object.
    bool     d_weOwnTooltip;        //!< true if System created the custom Tooltip.
};

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUISystem_h_
