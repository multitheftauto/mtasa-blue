/***************************************************************************
    This file was originally borowed from the Ogre project, though is now
    largely rewritten.
 ***************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2009 Paul D Turner & The CEGUI Development Team
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
#include <CoreFoundation/CoreFoundation.h>
#include "macPlugins.h"
#include "CEGUI/Logger.h"

namespace CEGUI 
{
//----------------------------------------------------------------------------//   
CFStringRef createBundleNameString(const char* name)
{
    CFStringRef s = CFStringCreateWithCString(0, name, kCFStringEncodingASCII);

    if(!CFStringHasSuffix(s, CFSTR(".bundle"))) 
    {
        CFMutableStringRef ms = CFStringCreateMutableCopy(0, 0, s);
        CFRelease(s);
        CFStringAppend(ms, CFSTR(".bundle"));
        s = ms;
    }
    
    return s;
}

//----------------------------------------------------------------------------//
CFURLRef createFullPluginsURL(CFBundleRef bundle)
{
    CFURLRef pluginsURL = 0;

    CFURLRef baseURL = CFBundleCopyBundleURL(bundle);

    if (baseURL)
    {
        CFURLRef pluginsLocation = CFBundleCopyBuiltInPlugInsURL(bundle);
        
        if (pluginsLocation)
        {
            pluginsURL = CFURLCreateCopyAppendingPathComponent(0, baseURL,
                                              CFURLGetString(pluginsLocation),
                                              false);
            CFRelease(pluginsLocation);
        }
        
        CFRelease(baseURL);
    }
    
    return pluginsURL;
}

//----------------------------------------------------------------------------//
CFBundleRef createBundle(CFBundleRef sourceBundle, CFStringRef bundleName)
{
    CFBundleRef bundle = 0;

    CFURLRef pluginsURL = createFullPluginsURL(sourceBundle);

    if (pluginsURL)
    {
        CFURLRef bundleURL =
            CFURLCreateCopyAppendingPathComponent(0, pluginsURL,
                                                  bundleName, false);
        if (bundleURL)
        {
            bundle = CFBundleCreate(0, bundleURL);
            CFRelease(bundleURL);
        }

        CFRelease(pluginsURL);
    }

    return bundle;
}

//----------------------------------------------------------------------------//
CFBundleRef mac_loadExeBundle(const char* name) 
{
    Logger* logger = Logger::getSingletonPtr();

    if (logger)
    {
       logger->logEvent("---- Beginning exe bundle loading ----");
       logger->logEvent("-- Input bundle name: " + String(name));
    }

    CFBundleRef bundle = 0;

    CFStringRef pluginName = createBundleNameString(name);
    
    if (pluginName)
    {
        bundle = createBundle(CFBundleGetMainBundle(), pluginName);

        if (!bundle)
        {
            if(logger)
                logger->logEvent(
                    "-- Couldn't get bundle from main app bundle reference; "
                    "trying CEGUIbase instead.");

            bundle =
                createBundle(CFBundleGetBundleWithIdentifier(
                                CFSTR("net.sourceforge.crayzedsgui.CEGUIBase")), 
                             pluginName);
        }

        if (bundle) 
        {
            if (!CFBundleLoadExecutable(bundle))
            {
                CFRelease(bundle);
                bundle = 0;
            }
        }
        
        CFRelease(pluginName);
    }

    if (logger)
    {
        if (bundle)
            logger->logEvent("-- Bundle loaded successfully.");
        else
            logger->logEvent("-- Bundle loading failed!");

        logger->logEvent("---- Finished exe bundle loading ----");
    }
  
    return bundle;
}

//----------------------------------------------------------------------------//
void* mac_getBundleSym(CFBundleRef bundle, const char* name) 
{
    CFStringRef nameRef =
        CFStringCreateWithCString(0, name, kCFStringEncodingASCII);
    void* sym = CFBundleGetFunctionPointerForName(bundle, nameRef);
    CFRelease(nameRef);

    return sym;
}

//----------------------------------------------------------------------------//
// Returns 1 on error, 0 otherwise.
bool mac_unloadExeBundle(CFBundleRef bundle) 
{
    if (bundle) 
    {
        // No-op, can't unload Obj-C bundles without crashing.
        return 0;
    }
    
    return 1;
}

//----------------------------------------------------------------------------//
const char* mac_errorBundle() 
{
    return "Unknown Error";
}

//----------------------------------------------------------------------------//

}
