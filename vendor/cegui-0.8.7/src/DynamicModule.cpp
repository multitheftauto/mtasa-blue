/***********************************************************************
    created:    Tue Mar 7 2006
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2013 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/DynamicModule.h"
#include "CEGUI/Base.h"
#include "CEGUI/String.h"
#include "CEGUI/Exceptions.h"
#include <stdlib.h>

#if defined(__WIN32__) || defined(_WIN32)
#   if defined(_MSC_VER)
#       pragma warning(disable : 4552)  // warning: operator has no effect; expected operator with side-effect
#   endif
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   define DYNLIB_LOAD( a ) LoadLibrary( (a).c_str() )
#   define DYNLIB_GETSYM( a, b ) GetProcAddress( a, (b).c_str() )
#   define DYNLIB_UNLOAD( a ) !FreeLibrary( a )
    typedef HMODULE DYNLIB_HANDLE;
#endif

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__HAIKU__) || defined(__CYGWIN__)
#   include "dlfcn.h"
#   define DYNLIB_LOAD( a ) dlopen( (a).c_str(), RTLD_LAZY )
#   define DYNLIB_GETSYM( a, b ) dlsym( a, (b).c_str() )
#   define DYNLIB_UNLOAD( a ) dlclose( a )
    typedef void* DYNLIB_HANDLE;
#endif

// setup default-default path
#ifndef CEGUI_MODULE_DIR
    #define CEGUI_MODULE_DIR "./bin/"
#endif

namespace CEGUI
{
//----------------------------------------------------------------------------//
struct DynamicModule::Impl :
    public AllocatedObject<DynamicModule::Impl>
{
    Impl(const String& name) :
        d_moduleName(name),
        d_handle(0)
    {}

    ~Impl()
    {
        CEGUI_UNUSED(DYNLIB_UNLOAD(d_handle));
    }

    //! Holds the name of the loaded module.
    String d_moduleName;
    //! Handle for the loaded module
    DYNLIB_HANDLE d_handle;
};

//----------------------------------------------------------------------------//
static const char MODULE_DIR_VAR_NAME[] = "CEGUI_MODULE_DIR";

//----------------------------------------------------------------------------//
#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__)
    static const String LibraryExtension(".dll");
#elif defined(__APPLE__)
    static const String LibraryExtension(".dylib");
#else
    static const String LibraryExtension(".so");
#endif

//----------------------------------------------------------------------------//
// return whether module name has it's extension in place
static bool hasDynamicLibraryExtension(const String& name)
{
    const size_t ext_len = LibraryExtension.length();

    if (name.length() < ext_len)
        return false;

    return name.compare(name.length() - ext_len, ext_len, LibraryExtension) == 0;
}
//----------------------------------------------------------------------------//
static void appendDynamicLibraryExtension(String& name)
{
    name.append(LibraryExtension);
}
//----------------------------------------------------------------------------//
static void addLibraryNameSuffixes(String& name)
{
    #ifdef CEGUI_HAS_BUILD_SUFFIX
        name += CEGUI_BUILD_SUFFIX;
    #endif

    appendDynamicLibraryExtension(name);
}

//----------------------------------------------------------------------------//
static String getModuleDirEnvVar()
{
    if (const char* envModuleDir = getenv(MODULE_DIR_VAR_NAME))
        return String(envModuleDir);

    return String();
}

//----------------------------------------------------------------------------//
// Returns a String containing the last failure message from the platform's
// dynamic loading system.
static String getFailureString()
{
    String retMsg;
#if defined(__linux__) || defined (__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__HAIKU__) || defined(__CYGWIN__)
    retMsg = dlerror();
#elif defined(__WIN32__) || defined(_WIN32)
    LPVOID msgBuffer;

    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                      FORMAT_MESSAGE_FROM_SYSTEM | 
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                      0,
                      GetLastError(),
                      MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                      reinterpret_cast<LPTSTR>(&msgBuffer),
                      0,
                      0))
    {
        retMsg = reinterpret_cast<LPTSTR>(msgBuffer);
        LocalFree(msgBuffer);
    }
    else
    {
        retMsg = "Unknown Error";
    }
#else
    retMsg = "Unknown Error";
#endif
    return retMsg;
}

//----------------------------------------------------------------------------//
static DYNLIB_HANDLE DynLibLoad(const String& name)
{
    DYNLIB_HANDLE handle = 0;

    // prefer whatever location is set in CEGUI_MODULE_DIR environment var
    const String envModuleDir(getModuleDirEnvVar());

    if (!envModuleDir.empty())
        handle = DYNLIB_LOAD(envModuleDir + '/' + name);

    #ifdef __APPLE__
    if (!handle)
        // on apple, look in the app bundle frameworks directory
        handle = DYNLIB_LOAD("@executable_path/../Frameworks/" + name);
    #endif

    if (!handle)
        // try loading without any explicit location (i.e. use OS search path)
        handle = DYNLIB_LOAD(name);

    // finally, try using the compiled-in module directory
    #if defined(CEGUI_MODULE_DIR)
    if (!handle)
        handle = DYNLIB_LOAD(CEGUI_MODULE_DIR + name);
    #endif

    return handle;
}

//----------------------------------------------------------------------------//
DynamicModule::DynamicModule(const String& name) :
    d_pimpl(CEGUI_NEW_AO Impl(name))
{
	if (name.empty())
		return;

    if (!hasDynamicLibraryExtension(d_pimpl->d_moduleName))
        addLibraryNameSuffixes(d_pimpl->d_moduleName);

    d_pimpl->d_handle = DynLibLoad(d_pimpl->d_moduleName);

#if defined(__linux__) || defined(__APPLE__) || defined(__MINGW32__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__HAIKU__)
    // see if adding a leading 'lib' helps us to open the library
    if (!d_pimpl->d_handle && d_pimpl->d_moduleName.compare(0, 3, "lib") != 0)
    {
        d_pimpl->d_moduleName.insert(0, "lib");
        d_pimpl->d_handle = DynLibLoad(d_pimpl->d_moduleName);
    }
#endif

#if defined(__CYGWIN__)
    // see if adding a leading 'cyg' helps us to open the library
    if (!d_pimpl->d_handle && d_pimpl->d_moduleName.compare(0, 3, "cyg") != 0)
    {
        d_pimpl->d_moduleName.insert(0, "cyg");
        d_pimpl->d_handle = DynLibLoad(d_pimpl->d_moduleName);
    }
#endif

    // check for library load failure
    if (!d_pimpl->d_handle)
        CEGUI_THROW(GenericException("Failed to load module '" +
            d_pimpl->d_moduleName + "': " + getFailureString()));
}

//----------------------------------------------------------------------------//
DynamicModule::~DynamicModule()
{
    CEGUI_DELETE_AO d_pimpl;
}

//----------------------------------------------------------------------------//
const String& DynamicModule::getModuleName() const
{
    return d_pimpl->d_moduleName;
}

//----------------------------------------------------------------------------//
void* DynamicModule::getSymbolAddress(const String& symbol) const
{
    return (void*)DYNLIB_GETSYM(d_pimpl->d_handle, symbol);
}

//----------------------------------------------------------------------------//

}
