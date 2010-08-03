/************************************************************************
	filename: 	CEGUIScriptModule.h
	created:	16/7/2004
	author:		Paul D Turner

	purpose:	Abstract class interface for scripting support
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
#ifndef _CEGUIScriptModule_h_
#define _CEGUIScriptModule_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIEvent.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Abstract interface required for all scripting support modules to be used with
	the CEGUI system.
*/
class CEGUIEXPORT ScriptModule
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for ScriptModule base class
	*/
	ScriptModule(void);


	/*!
	\brief
		Destructor for ScriptModule base class.
	*/
	virtual ~ScriptModule(void) {}


	/*************************************************************************
		Abstract interface
	*************************************************************************/
	/*!
	\brief
		Execute a script file.

	\param filename
		String object holding the filename of the script file that is to be executed
		
	\param resourceGroup
		Resource group idendifier to be passed to the ResourceProvider when loading the script file.
	*/
	virtual	void	executeScriptFile(const String& filename, const String& resourceGroup = "")	= 0;


	/*!
	\brief
		Execute a scripted global function.  The function should not take any parameters and should return an integer.

	\param function_name
		String object holding the name of the function, in the global script environment, that
		is to be executed.

	\return
		The integer value returned from the script function.
	*/
	virtual int	executeScriptGlobal(const String& function_name)	= 0;


	/*!
	\brief
		Execute a scripted global 'event handler' function.  The function should take some kind of EventArgs like parameter
		that the concrete implementation of this function can create from the passed EventArgs based object.  The function
		should not return anything.

	\param handler_name
		String object holding the name of the scripted handler function.

	\param e
		EventArgs based object that should be passed, by any appropriate means, to the scripted function.

	\return
		- true if the event was handled.
		- false if the event was not handled.
	*/
	virtual	bool	executeScriptedEventHandler(const String& handler_name, const EventArgs& e)		= 0;


    /*!
    \brief
        Execute script code contained in the given CEGUI::String object.

    \param str
        String object holding the valid script code that should be executed.

    \return
        Nothing.
    */
    virtual void executeString(const String& str) = 0;


    /*!
    \brief
        Method called during system initialisation, prior to running any scripts via the ScriptModule, to enable the ScriptModule
        to perform any operations required to complete initialisation or binding of the script language to the gui system objects.

    \return
        Nothing.
    */
    virtual void createBindings(void) {}


    /*!
    \brief
        Method called during system destruction, after all scripts have been run via the ScriptModule, to enable the ScriptModule
        to perform any operations required to cleanup bindings of the script language to the gui system objects, as set-up in the
        earlier createBindings call.

    \return
        Nothing.
    */
    virtual void destroyBindings(void) {}

    /*!
    \brief
        Return identification string for the ScriptModule.  If the internal id string has not been
        set by the ScriptModule creator, a generic string of "Unknown scripting module" will be returned.

    \return
        String object holding a string that identifies the ScriptModule in use.
    */
    const String& getIdentifierString() const;

protected:
    String d_identifierString;                 //!< String that holds some id information about the module.
};


/*!
\brief
	Functor class used for binding named script functions to events
*/
class ScriptFunctor
{
public:
	ScriptFunctor(const String& functionName) : scriptFunctionName(functionName) {}
    ScriptFunctor(const ScriptFunctor& obj) : scriptFunctionName(obj.scriptFunctionName) {}
	bool	operator()(const EventArgs& e) const;

private:
    // no assignment possible
    ScriptFunctor& operator=(const ScriptFunctor& rhs);

	const String	scriptFunctionName;
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIScriptModule_h_
