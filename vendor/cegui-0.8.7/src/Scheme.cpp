/***********************************************************************
    created:    21/2/2004
    author:     Paul D Turner

    purpose:    Implements GUI Scheme class
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
#include "CEGUI/Scheme.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/SchemeManager.h"
#include "CEGUI/Logger.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/Font.h"
#include "CEGUI/WindowFactoryManager.h"
#include "CEGUI/WindowRendererManager.h"
#include "CEGUI/FactoryModule.h"
#include "CEGUI/Scheme_xmlHandler.h"
#include "CEGUI/DataContainer.h"
#include "CEGUI/System.h"
#include "CEGUI/XMLParser.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/DynamicModule.h"

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

// Declare functions to return FactoryModules for Window and WindowRenderers
// as extern when statically linking.
#if defined(CEGUI_STATIC)
extern "C"
{
CEGUI::FactoryModule& getWindowRendererFactoryModule();
CEGUI::FactoryModule& getWindowFactoryModule();
}
#endif


// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
    Static Data definitions
*************************************************************************/
// default resource group
String Scheme::d_defaultResourceGroup;


/*************************************************************************
    Constructor for scheme objects
*************************************************************************/
Scheme::Scheme(const String& name) :
    d_name(name)
{
}


/*************************************************************************
    Destructor for scheme objects
*************************************************************************/
Scheme::~Scheme(void)
{
    unloadResources();

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent("GUI scheme '" + d_name + "' has been "
        "unloaded (object destructor). " + addr_buff, Informative);
}


/*************************************************************************
    Load all resources for this scheme
*************************************************************************/
void Scheme::loadResources(void)
{
    Logger::getSingleton().logEvent("---- Begining resource loading for GUI scheme '" + d_name + "' ----", Informative);

    // load all resources specified for this scheme.
    loadXMLImagesets();
    loadImageFileImagesets();
    loadFonts();
    loadLookNFeels();
    loadWindowRendererFactories();
    loadWindowFactories();
    loadFactoryAliases();
    loadFalagardMappings();

    Logger::getSingleton().logEvent("---- Resource loading for GUI scheme '" + d_name + "' completed ----", Informative);
}


/*************************************************************************
    Unload all resources for this scheme
*************************************************************************/
void Scheme::unloadResources(void)
{
    Logger::getSingleton().logEvent("---- Begining resource cleanup for GUI scheme '" + d_name + "' ----", Informative);

    // unload all resources specified for this scheme.
    //unloadFonts(); FIXME: Prevent unloading of cross-used fonts
    //unloadXMLImagesets(); FIXME: ????
    unloadImageFileImagesets();
    unloadWindowFactories();
    unloadWindowRendererFactories();
    unloadFactoryAliases();
    unloadFalagardMappings();
    unloadLookNFeels();

    Logger::getSingleton().logEvent("---- Resource cleanup for GUI scheme '" + d_name + "' completed ----", Informative);
}


/*************************************************************************
    Check if all resources for this Scheme are loaded
*************************************************************************/
bool Scheme::resourcesLoaded(void) const
{
    // test state of all loadable resources for this scheme.
    if (//areXMLImagesetsLoaded() && FIXME: ????
        areImageFileImagesetsLoaded() &&
        areFontsLoaded() &&
        areWindowRendererFactoriesLoaded() &&
        areWindowFactoriesLoaded() &&
        areFactoryAliasesLoaded() &&
        areFalagardMappingsLoaded())
    {
        return true;
    }

    return false;
}


/*************************************************************************
    Load all xml imagesets specified.
*************************************************************************/
void Scheme::loadXMLImagesets()
{
    // check all imagesets
    for (LoadableUIElementList::iterator pos = d_imagesets.begin();
        pos != d_imagesets.end(); ++pos)
    {
        ImageManager::getSingleton().loadImageset(
            (*pos).filename, (*pos).resourceGroup);
    }
}

/*************************************************************************
    Load all image file based imagesets specified.
*************************************************************************/
void Scheme::loadImageFileImagesets()
{
    ImageManager& imgr = ImageManager::getSingleton();

    // check images that are created directly from image files
    for (LoadableUIElementList::iterator pos = d_imagesetsFromImages.begin();
        pos != d_imagesetsFromImages.end(); ++pos)
    {
        // if name is empty use the name of the image file.
        if ((*pos).name.empty())
            (*pos).name = (*pos).filename;

        // see if image is present, and create it if not.
        if (!imgr.isDefined((*pos).name))
            imgr.addFromImageFile((*pos).name, (*pos).filename, (*pos).resourceGroup);
    }
}

/*************************************************************************
    Load all xml based fonts specified.
*************************************************************************/
void Scheme::loadFonts()
{
    FontManager& fntmgr = FontManager::getSingleton();

    // check fonts
    for (LoadableUIElementList::iterator pos = d_fonts.begin();
        pos != d_fonts.end(); ++pos)
    {
        // skip if a font with this name is already loaded
        if (!(*pos).name.empty() && fntmgr.isDefined((*pos).name))
            continue;

        // create font using specified xml file.
        Font& font = fntmgr.createFromFile((*pos).filename, (*pos).resourceGroup);
        const String realname(font.getName());

        // if name was not in scheme, set it now and proceed to next font
        if ((*pos).name.empty())
        {
            (*pos).name = realname;
            continue;
        }

        // confirm the font loaded has same name specified in scheme
        if (realname != (*pos).name)
        {
            fntmgr.destroy(font);
            CEGUI_THROW(InvalidRequestException(
                "The Font created by file '" + (*pos).filename +
                "' is named '" + realname + "', not '" + (*pos).name +
                "' as required by Scheme '" + d_name + "'."));
        }
    }
}

/*************************************************************************
    Load all xml LookNFeel files specified.
*************************************************************************/
void Scheme::loadLookNFeels()
{
    WidgetLookManager& wlfMgr   = WidgetLookManager::getSingleton();

    // load look'n'feels
    // (we can't actually check these, at the moment, so we just re-parse data;
    // it does no harm except maybe waste a bit of time)
    for (LoadableUIElementList::const_iterator pos = d_looknfeels.begin();
        pos != d_looknfeels.end(); ++pos)
    {
        wlfMgr.parseLookNFeelSpecificationFromFile((*pos).filename, (*pos).resourceGroup);
    }
}

/*************************************************************************
    Load all windowset modules specified.and register factories.
*************************************************************************/
void Scheme::loadWindowFactories()
{
    // check factories
    for (UIModuleList::iterator cmod = d_widgetModules.begin();
        cmod != d_widgetModules.end(); ++cmod)
    {
        if (!(*cmod).factoryModule)
        {
#if !defined(CEGUI_STATIC)
            // load dynamic module as required
            if (!(*cmod).dynamicModule)
                (*cmod).dynamicModule = CEGUI_NEW_AO DynamicModule((*cmod).name);

            FactoryModule& (*getWindowFactoryModuleFunc)() =
                reinterpret_cast<FactoryModule&(*)()>(
                    (*cmod).dynamicModule->
                        getSymbolAddress("getWindowFactoryModule"));

            if (!getWindowFactoryModuleFunc)
                CEGUI_THROW(InvalidRequestException(
                    "Required function export "
                    "'FactoryModule& ""getWindowFactoryModule()' "
                    "was not found in module '" + (*cmod).name + "'."));

            // get the WindowRendererModule object for this module.
            (*cmod).factoryModule = &getWindowFactoryModuleFunc();
#else
            (*cmod).factoryModule = &getWindowFactoryModule();
#endif
        }

        // see if we should just register all factories available in the module
        // (i.e. No factories explicitly specified)
        if ((*cmod).types.size() == 0)
        {
            Logger::getSingleton().logEvent("No Window factories "
                                            "specified for module '" +
                                            (*cmod).name + "' - adding all "
                                            "available factories...");
            (*cmod).factoryModule->registerAllFactories();
        }
        // some names were explicitly given, so only register those.
        else
        {
            UIModule::TypeList::const_iterator elem = (*cmod).types.begin();
            for (; elem != (*cmod).types.end(); ++elem)
                (*cmod).factoryModule->registerFactory(*elem);
        }
    }
}

/*************************************************************************
    Load all windowrendererset modules specified.and register factories.
*************************************************************************/
void Scheme::loadWindowRendererFactories()
{
    // check factories
    for (WRModuleList::iterator cmod = d_windowRendererModules.begin();
        cmod != d_windowRendererModules.end(); ++cmod)
    {
        if (!(*cmod).factoryModule)
        {
#if !defined(CEGUI_STATIC)
            // load dynamic module as required
            if (!(*cmod).dynamicModule)
                (*cmod).dynamicModule = CEGUI_NEW_AO DynamicModule((*cmod).name);

            FactoryModule& (*getWRFactoryModuleFunc)() =
                reinterpret_cast<FactoryModule&(*)()>((*cmod).dynamicModule->
                    getSymbolAddress("getWindowRendererFactoryModule"));

            if (!getWRFactoryModuleFunc)
                CEGUI_THROW(InvalidRequestException(
                    "Required function export "
                    "'FactoryModule& getWindowRendererFactoryModule()' "
                    "was not found in module '" + (*cmod).name + "'."));

            // get the WindowRendererModule object for this module.
            (*cmod).factoryModule = &getWRFactoryModuleFunc();
#else
            (*cmod).factoryModule = &getWindowRendererFactoryModule();
#endif
        }

        // see if we should just register all factories available in the module
        // (i.e. No factories explicitly specified)
        if ((*cmod).types.size() == 0)
        {
            Logger::getSingleton().logEvent("No window renderer factories "
                                            "specified for module '" +
                                            (*cmod).name + "' - adding all "
                                            "available factories...");
            (*cmod).factoryModule->registerAllFactories();
        }
        // some names were explicitly given, so only register those.
        else
        {
            UIModule::TypeList::const_iterator elem = (*cmod).types.begin();
            for (; elem != (*cmod).types.end(); ++elem)
                (*cmod).factoryModule->registerFactory(*elem);
        }
    }
}

/*************************************************************************
    Create all window factory aliases
*************************************************************************/
void Scheme::loadFactoryAliases()
{
    WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();

    // check aliases
    for (AliasMappingList::iterator alias = d_aliasMappings.begin();
        alias != d_aliasMappings.end(); ++alias)
    {
        // get iterator
        WindowFactoryManager::TypeAliasIterator iter = wfmgr.getAliasIterator();

        // look for this alias
        while (!iter.isAtEnd() && (iter.getCurrentKey() != (*alias).aliasName))
            ++iter;

        // if the alias exists
        if (!iter.isAtEnd())
        {
            // if the current target type matches
            if (iter.getCurrentValue().getActiveTarget() == (*alias).targetName)
            {
                // assume this mapping is ours and skip to next alias
                continue;
            }
        }

        // create a new alias entry
        wfmgr.addWindowTypeAlias((*alias).aliasName, (*alias).targetName);
    }
}

/*************************************************************************
    Create all required falagard mappings
*************************************************************************/
void Scheme::loadFalagardMappings()
{
    WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();

    // check falagard window mappings.
    for (FalagardMappingList::iterator falagard = d_falagardMappings.begin();
        falagard != d_falagardMappings.end(); ++falagard)
    {
        // get iterator
        WindowFactoryManager::FalagardMappingIterator iter = wfmgr.getFalagardMappingIterator();

        // look for this mapping
        while (!iter.isAtEnd() && (iter.getCurrentKey() != (*falagard).windowName))
            ++iter;

        // if the mapping exists
        if (!iter.isAtEnd())
        {
            // check if the current target and looks and window renderer match
            if ((iter.getCurrentValue().d_baseType == (*falagard).targetName) &&
                (iter.getCurrentValue().d_rendererType == (*falagard).rendererName) &&
                (iter.getCurrentValue().d_lookName == (*falagard).lookName))
            {
                // assume this mapping is ours and skip to next
                continue;
            }
        }

        // create a new mapping entry
        wfmgr.addFalagardWindowMapping((*falagard).windowName,
                                       (*falagard).targetName,
                                       (*falagard).lookName,
                                       (*falagard).rendererName,
                                       (*falagard).effectName);
    }
}

/*************************************************************************
    Unload all xml based imagesets
*************************************************************************/
void Scheme::unloadXMLImagesets()
{
/* FIXME ?
    ImageManager& imgr = ImageManager::getSingleton();

    // unload all xml based Imagesets
    for (LoadableUIElementList::const_iterator pos = d_imagesets.begin();
        pos != d_imagesets.end(); ++pos)
    {
        if (!(*pos).name.empty())
            ismgr.destroy((*pos).name);
    }
*/
}

/*************************************************************************
    Unload all image file based imagesets
*************************************************************************/
void Scheme::unloadImageFileImagesets()
{
    ImageManager& imgr = ImageManager::getSingleton();

    // unload all images that are created directly from image files
    for (LoadableUIElementList::const_iterator pos = d_imagesetsFromImages.begin();
        pos != d_imagesetsFromImages.end(); ++pos)
    {
        if (!(*pos).name.empty())
            imgr.destroy((*pos).name);
    }
}

/*************************************************************************
    Unload all xml based fonts
*************************************************************************/
void Scheme::unloadFonts()
{
    FontManager& fontManager = FontManager::getSingleton();

    // unload all loaded fonts
    for (LoadableUIElementList::iterator iter = d_fonts.begin();
        iter != d_fonts.end(); ++iter)
    {
        if (!(*iter).name.empty())
            fontManager.destroy((*iter).name);
    }
}

/*************************************************************************
    Unload all xml based LookNfeels
*************************************************************************/
void Scheme::unloadLookNFeels()
{
    // While it's possible to remove WidgetLooks etc, currently we can't
    // extract which looks were loaded from a given xml file, so we just
    // do nothing for now.
}

/*************************************************************************
    Unregister all window factories and unload shared modules
*************************************************************************/
void Scheme::unloadWindowFactories()
{
    // for all widget modules loaded
    for (UIModuleList::iterator cmod = d_widgetModules.begin();
        cmod != d_widgetModules.end(); ++cmod)
    {
        // assume module's factories were already removed if factoryModule is 0.
        if (!(*cmod).factoryModule)
            continue;

        // see if we should just unregister all factories available in the
        // module (i.e. No factories explicitly specified)
        if ((*cmod).types.size() == 0)
        {
            (*cmod).factoryModule->unregisterAllFactories();
        }
        // remove all window factories explicitly registered for this module
        else
        {
            UIModule::TypeList::const_iterator elem = (*cmod).types.begin();
            for (; elem != (*cmod).types.end(); ++elem)
                (*cmod).factoryModule->unregisterFactory(*elem);
        }

        // unload dynamic module as required
        if ((*cmod).dynamicModule)
        {
            CEGUI_DELETE_AO (*cmod).dynamicModule;
            (*cmod).dynamicModule = 0;
        }

        (*cmod).factoryModule = 0;
    }
}

/*************************************************************************
    Unregister all window renderer factories and unload shared modules
*************************************************************************/
void Scheme::unloadWindowRendererFactories()
{
    // for all widget modules loaded
    for (WRModuleList::iterator cmod = d_windowRendererModules.begin();
        cmod != d_windowRendererModules.end(); ++cmod)
    {
        // assume module's factories were already removed if wrModule is 0.
        if (!(*cmod).factoryModule)
            continue;

        // see if we should just unregister all factories available in the
        // module (i.e. No factories explicitly specified)
        if ((*cmod).types.size() == 0)
        {
            (*cmod).factoryModule->unregisterAllFactories();
        }
        // remove all window factories explicitly registered for this module
        else
        {
            UIModule::TypeList::const_iterator elem = (*cmod).types.begin();
            for (; elem != (*cmod).types.end(); ++elem)
                (*cmod).factoryModule->unregisterFactory(*elem);
        }

        // unload dynamic module as required
        if ((*cmod).dynamicModule)
        {
            CEGUI_DELETE_AO (*cmod).dynamicModule;
            (*cmod).dynamicModule = 0;
        }

        (*cmod).factoryModule = 0;
    }
}

/*************************************************************************
    Unregister all factory aliases
*************************************************************************/
void Scheme::unloadFactoryAliases()
{
    WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();

    // remove all factory aliases
    for (AliasMappingList::iterator alias = d_aliasMappings.begin();
        alias != d_aliasMappings.end(); ++alias)
    {
        // get iterator
        WindowFactoryManager::TypeAliasIterator iter = wfmgr.getAliasIterator();

        // look for this alias
        while (!iter.isAtEnd() && (iter.getCurrentKey() != (*alias).aliasName))
            ++iter;

        // if the alias exists
        if (!iter.isAtEnd())
            // remove this alias entry
            wfmgr.removeWindowTypeAlias((*alias).aliasName, (*alias).targetName);
    }
}

/*************************************************************************
    Unregister all Falagard window type mappings
*************************************************************************/
void Scheme::unloadFalagardMappings()
{
    WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();

    // remove all falagard window mappings for this scheme.
    for (FalagardMappingList::iterator falagard = d_falagardMappings.begin();
        falagard != d_falagardMappings.end(); ++falagard)
    {
        // get iterator
        WindowFactoryManager::FalagardMappingIterator iter = wfmgr.getFalagardMappingIterator();

        // look for this mapping
        while (!iter.isAtEnd() && (iter.getCurrentKey() != (*falagard).windowName))
            ++iter;

        // if the mapping exists
        if (!iter.isAtEnd())
        {
            // if the current target and looks match
            if ((iter.getCurrentValue().d_baseType == (*falagard).targetName) &&
                (iter.getCurrentValue().d_rendererType == (*falagard).rendererName) &&
                (iter.getCurrentValue().d_lookName == (*falagard).lookName))
            {
                // assume this mapping is ours and delete it
                wfmgr.removeFalagardWindowMapping((*falagard).targetName);
            }
        }
    }
}

/*************************************************************************
    Check if all XML based imagesets are loaded
*************************************************************************/
bool Scheme::areXMLImagesetsLoaded() const
{
/* FIXME ?
    ImageManager& imgr = ImageManager::getSingleton();

    // check imagesets
    for (LoadableUIElementList::const_iterator pos = d_imagesets.begin();
        pos != d_imagesets.end(); ++pos)
    {
        if ((*pos).name.empty() || !ismgr.isDefined((*pos).name))
            return false;
    }
*/
    return true;
}

/*************************************************************************
    Check if all image file based imagesets are loaded
*************************************************************************/
bool Scheme::areImageFileImagesetsLoaded() const
{
    ImageManager& imgr = ImageManager::getSingleton();

    for (LoadableUIElementList::const_iterator pos = d_imagesetsFromImages.begin();
        pos != d_imagesetsFromImages.end(); ++pos)
    {
        if ((*pos).name.empty() || !imgr.isDefined((*pos).name))
            return false;
    }

    return true;
}

/*************************************************************************
    Check if all xml based fonts are loaded
*************************************************************************/
bool Scheme::areFontsLoaded() const
{
    FontManager& fntmgr = FontManager::getSingleton();

    // check fonts
    for (LoadableUIElementList::const_iterator pos = d_fonts.begin();
        pos != d_fonts.end(); ++pos)
    {
        if ((*pos).name.empty() || !fntmgr.isDefined((*pos).name))
            return false;
    }

    return true;
}

/*************************************************************************
    Check if xml looknfeel files are loaded
*************************************************************************/
bool Scheme::areLookNFeelsLoaded() const
{
    // can't check these at the moment, just say yes!
    return true;
}

/*************************************************************************
    Check if all required window factories are registered
*************************************************************************/
bool Scheme::areWindowFactoriesLoaded() const
{
    WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();

    // check factory modules
    for (UIModuleList::const_iterator cmod = d_widgetModules.begin();
        cmod != d_widgetModules.end(); ++cmod)
    {
        // see if we should just test all factories available in the
        // module (i.e. No factories explicitly specified)
        if ((*cmod).types.size() == 0)
        {
            // TODO: This is not supported yet!
        }
        // check all window factories explicitly registered for this module
        else
        {
            UIModule::TypeList::const_iterator elem = (*cmod).types.begin();

            for (; elem != (*cmod).types.end(); ++elem)
            {
                if (!wfmgr.isFactoryPresent(*elem))
                    return false;
            }
        }
    }

    return true;
}

/*************************************************************************
    Check if all required window renderer factories are registered
*************************************************************************/
bool Scheme::areWindowRendererFactoriesLoaded() const
{
    WindowRendererManager& wfmgr = WindowRendererManager::getSingleton();

    // check factory modules
    for (WRModuleList::const_iterator cmod = d_windowRendererModules.begin();
        cmod != d_windowRendererModules.end(); ++cmod)
    {
        // see if we should just test all factories available in the
        // module (i.e. No factories explicitly specified)
        if ((*cmod).types.size() == 0)
        {
            // TODO: This is not supported yet!
        }
        // check all window factories explicitly registered for this module
        else
        {
            UIModule::TypeList::const_iterator elem = (*cmod).types.begin();

            for (; elem != (*cmod).types.end(); ++elem)
                if (!wfmgr.isFactoryPresent(*elem))
                    return false;
        }
    }

    return true;
}

/*************************************************************************
    Check if all window factory aliases are registered
*************************************************************************/
bool Scheme::areFactoryAliasesLoaded() const
{
    WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();

    // check aliases
    for (AliasMappingList::const_iterator alias = d_aliasMappings.begin();
        alias != d_aliasMappings.end(); ++alias)
    {
        // get iterator
        WindowFactoryManager::TypeAliasIterator iter = wfmgr.getAliasIterator();

        // look for this alias
        while (!iter.isAtEnd() && (iter.getCurrentKey() != (*alias).aliasName))
            ++iter;

        // if the alias exists
        if (!iter.isAtEnd())
        {
            // if the current target type matches
            if (iter.getCurrentValue().getActiveTarget() == (*alias).targetName)
                // target matches, assume we set it and continue to next alias
                continue;
        }

        // no alias or target type does not match
        return false;
    }

    return true;
}

/*************************************************************************
    Check if all falagard mappings are registered
*************************************************************************/
bool Scheme::areFalagardMappingsLoaded() const
{
    WindowFactoryManager& wfmgr = WindowFactoryManager::getSingleton();

    // check falagard window mappings.
    for (FalagardMappingList::const_iterator falagard = d_falagardMappings.begin();
        falagard != d_falagardMappings.end(); ++falagard)
    {
        // get iterator
        WindowFactoryManager::FalagardMappingIterator iter = wfmgr.getFalagardMappingIterator();

        // look for this mapping
        while (!iter.isAtEnd() && (iter.getCurrentKey() != (*falagard).windowName))
            ++iter;

        // if the mapping exists
        if (!iter.isAtEnd())
        {
            // if the current target, effect and looks match
            if ((iter.getCurrentValue().d_baseType == (*falagard).targetName) &&
                (iter.getCurrentValue().d_rendererType == (*falagard).rendererName) &&
                (iter.getCurrentValue().d_lookName == (*falagard).lookName) &&
                (iter.getCurrentValue().d_effectName == (*falagard).effectName))
            {
                // assume this mapping is ours and skip to next
                continue;
            }
        }

        return false;
    }

    return true;
}

Scheme::LoadableUIElementIterator Scheme::getXMLImagesets() const
{
    return LoadableUIElementIterator(d_imagesets.begin(), d_imagesets.end());
}

Scheme::LoadableUIElementIterator Scheme::getImageFileImagesets() const
{
    return LoadableUIElementIterator(d_imagesetsFromImages.begin(), d_imagesetsFromImages.end());
}


Scheme::LoadableUIElementIterator Scheme::getFonts() const
{
    return LoadableUIElementIterator(d_fonts.begin(), d_fonts.end());
}


Scheme::LoadableUIElementIterator Scheme::getLookNFeels() const
{
    return LoadableUIElementIterator(d_looknfeels.begin(), d_looknfeels.end());
}

} // End of  CEGUI namespace section
