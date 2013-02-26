/************************************************************************
	filename: 	CEGUIFactoryModule.cpp
	created:	12/4/2004
	author:		Paul D Turner
	
	purpose:	Implements FactoryModule for Win32 systems
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
#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUIExceptions.h"
#include "CEGUIFactoryModule.h"
#include "..\widgetsets\falagard\include\falmodule.h"

#if defined(__WIN32__) || defined(_WIN32)
#	if defined(_MSC_VER)
#		pragma warning(disable : 4552)	// warning: operator has no effect; expected operator with side-effect
#	endif
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#if defined(__APPLE_CC__)
#   include "macPlugins.h"
#endif

#if defined(__linux__)
#   include "dlfcn.h"
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
const char	FactoryModule::RegisterFactoryFunctionName[] = "registerFactory";
const char  FactoryModule::RegisterAllFunctionName[]     = "registerAllFactories";


/*************************************************************************
	Construct the FactoryModule object by loading the dynamic loadable
	module specified.
*************************************************************************/
FactoryModule::FactoryModule(const String& filename) :
	d_moduleName(filename)
{
	// Statically linked factory modules (e.g. falagard), so skip!

	/**

#if defined(__linux__)
	// dlopen() does not add .so to the filename, like windows does for .dll
	if (d_moduleName.substr(d_moduleName.length() - 3, 3) != (utf8*)".so")
	{
		d_moduleName += (utf8*)".so";
	}

	// see if we need to add the leading 'lib'
	if (d_moduleName.substr(0, 3) != (utf8*)"lib")
	{
		d_moduleName.insert(0, (utf8*)"lib");
	}
#endif

	/// This optionally adds a _d to the loaded module name under the debug config for Win32
#if defined(__WIN32__) || defined(_WIN32)
#	if defined (_DEBUG) && defined (CEGUI_LOAD_MODULE_APPEND_SUFFIX_FOR_DEBUG)
	// if name has .dll extension, assume it's complete and do not touch it.
	if (d_moduleName.substr(d_moduleName.length() - 4, 4) != (utf8*)".dll")
	{
		d_moduleName += (utf8*)CEGUI_LOAD_MODULE_DEBUG_SUFFIX;
	}
#	endif
#endif

	d_handle = DYNLIB_LOAD(d_moduleName.c_str());

	// check for library load failure
	if (d_handle == NULL)
	{
		throw	GenericException((utf8*)"FactoryModule::FactoryModule - Failed to load module '" + d_moduleName + "'.");
	}

    // functions are now optional, and only throw upon the first attempt to use a missing function.
    d_regFunc = (FactoryRegisterFunction)DYNLIB_GETSYM(d_handle, RegisterFactoryFunctionName);
    d_regAllFunc = (RegisterAllFunction)DYNLIB_GETSYM(d_handle, RegisterAllFunctionName);

	**/
}


/*************************************************************************
	Destroys the FactoryModule object and unloads any loadable module.
*************************************************************************/
FactoryModule::~FactoryModule(void)
{
	DYNLIB_UNLOAD(d_handle);
}


/*************************************************************************
	Register a WindowFactory for 'type' Windows.
*************************************************************************/
void FactoryModule::registerFactory(const String& type) const
{
    // are we attempting to use a missing function export
    if (!d_regFunc)
    {
        throw InvalidRequestException("FactoryModule::registerFactory - Required function export 'void registerFactory(const String& type)' was not found in module '" + d_moduleName + "'.");
    }

	d_regFunc(type);
}

uint FactoryModule::registerAllFactories() const
{
	// Statically linked factory modules (e.g. falagard), so skip!
	
	/**

	// are we attempting to use a missing function export
    if (!d_regAllFunc)
    {
        throw InvalidRequestException("FactoryModule::registerAllFactories - Required function export 'uint registerAllFactories(void)' was not found in module '" + d_moduleName + "'.");
    }

    return d_regAllFunc();

	**/

	registerAllFactoriesF();

	return 1;
}

} // End of  CEGUI namespace section
