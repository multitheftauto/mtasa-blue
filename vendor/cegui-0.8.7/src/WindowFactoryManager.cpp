/***********************************************************************
	created:	22/2/2004
	author:		Paul D Turner

	purpose:	Implements the WindowFactoryManager
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
#include "CEGUI/WindowFactoryManager.h"
#include "CEGUI/WindowFactory.h"
#include "CEGUI/Exceptions.h"
#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Static Data Definitions
*************************************************************************/
// singleton instance pointer
template<> WindowFactoryManager* Singleton<WindowFactoryManager>::ms_Singleton	= 0;
// list of owned WindowFactory object pointers
WindowFactoryManager::OwnedWindowFactoryList WindowFactoryManager::d_ownedFactories;

//----------------------------------------------------------------------------//
WindowFactoryManager::WindowFactoryManager(void)
{
    Logger::getSingleton().logEvent(
        "CEGUI::WindowFactoryManager singleton created");

    // complete addition of any pre-added WindowFactory objects
    WindowFactoryManager::OwnedWindowFactoryList::iterator i =
        d_ownedFactories.begin();

    if (d_ownedFactories.end() != i)
    {
        Logger::getSingleton().logEvent(
        "---- Adding pre-registered WindowFactory objects ----");

        for (; d_ownedFactories.end() != i; ++i)
            addFactory(*i);
    }
}

/*************************************************************************
	Adds a WindowFactory object to the registry
*************************************************************************/
void WindowFactoryManager::addFactory(WindowFactory* factory)
{
	// throw exception if passed factory is null.
	if (!factory)
	{
		CEGUI_THROW(NullObjectException(
            "The provided WindowFactory pointer was invalid."));
	}

	// throw exception if type name for factory is already in use
	if (d_factoryRegistry.find(factory->getTypeName()) != d_factoryRegistry.end())
	{
		CEGUI_THROW(AlreadyExistsException(
            "A WindowFactory for type '" + factory->getTypeName() +
            "' is already registered."));
	}

	// add the factory to the registry
	d_factoryRegistry[factory->getTypeName()] = factory;

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(factory));
	Logger::getSingleton().logEvent("WindowFactory for '" +
       factory->getTypeName() +"' windows added. " + addr_buff);
}


/*************************************************************************
	removes a WindowFactory from the registry (by name)
*************************************************************************/
void WindowFactoryManager::removeFactory(const String& name)
{
    WindowFactoryRegistry::iterator i = d_factoryRegistry.find(name);

    // exit if no factory exists for this type
    if (i == d_factoryRegistry.end())
        return;

    // see if we own this factory
    OwnedWindowFactoryList::iterator j = std::find(d_ownedFactories.begin(),
                                                   d_ownedFactories.end(),
                                                   (*i).second);

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>((*i).second));

	d_factoryRegistry.erase(name);

    Logger::getSingleton().logEvent("WindowFactory for '" + name +
                                    "' windows removed. " + addr_buff);

    // delete factory object if we created it
    if (j != d_ownedFactories.end())
    {
        Logger::getSingleton().logEvent("Deleted WindowFactory for '" +
                                        (*j)->getTypeName() +
                                        "' windows.");

        CEGUI_DELETE_AO (*j);
        d_ownedFactories.erase(j);
    }
}


/*************************************************************************
	removes a WindowFactory from the registry (by pointer)
*************************************************************************/
void WindowFactoryManager::removeFactory(WindowFactory* factory)
{
	if (factory)
	{
		removeFactory(factory->getTypeName());
	}

}

//----------------------------------------------------------------------------//
void WindowFactoryManager::removeAllFactories(void)
{
    while (!d_factoryRegistry.empty())
        removeFactory((*d_factoryRegistry.begin()).second);
}


/*************************************************************************
	returns a pointer to the requested WindowFactory object
*************************************************************************/
WindowFactory* WindowFactoryManager::getFactory(const String& type) const
{
    // first, dereference aliased types, as needed.
    String targetType(getDereferencedAliasType(type));

	// try for a 'real' type
	WindowFactoryRegistry::const_iterator pos = d_factoryRegistry.find(targetType);

	// found an actual factory for this type
	if (pos != d_factoryRegistry.end())
	{
		return pos->second;
	}
    // no concrete type, try for a falagard mapped type
    else
    {
        FalagardMapRegistry::const_iterator falagard = d_falagardRegistry.find(targetType);

        // found falagard mapping for this type
        if (falagard != d_falagardRegistry.end())
        {
            // recursively call getFactory on the target base type
            return getFactory(falagard->second.d_baseType);
        }
        // type not found anywhere, give up with an exception.
        else
        {
            CEGUI_THROW(UnknownObjectException(
                "A WindowFactory object, an alias, or mapping for '" + type +
                "' Window objects is not registered with the system.\n\n"
                "Have you forgotten to load a scheme using "
                "CEGUI::SchemeManager::createFromFile(..)?"));
        }
    }
}


/*************************************************************************
    Returns true if a WindowFactory, an alias, or a falagard mapping for
    a specified window type is present
*************************************************************************/
bool WindowFactoryManager::isFactoryPresent(const String& name) const
{
    // first, dereference aliased types, as needed.
    String targetType(getDereferencedAliasType(name));

    // now try for a 'real' type
    if (d_factoryRegistry.find(targetType) != d_factoryRegistry.end())
    {
        return true;
    }
    // not a concrete type, so return whether it's a Falagard mapped type.
    else
    {
        return (d_falagardRegistry.find(targetType) != d_falagardRegistry.end());
    }
}


/*************************************************************************
	Return a WindowFactoryManager::WindowFactoryIterator object to
	iterate over the available WindowFactory types.
*************************************************************************/
WindowFactoryManager::WindowFactoryIterator	WindowFactoryManager::getIterator(void) const
{
	return WindowFactoryIterator(d_factoryRegistry.begin(), d_factoryRegistry.end());
}


/*************************************************************************
	Return a WindowFactoryManager::TypeAliasIterator object to iterate
	over the defined aliases for window types.
*************************************************************************/
WindowFactoryManager::TypeAliasIterator WindowFactoryManager::getAliasIterator(void) const
{
	return TypeAliasIterator(d_aliasRegistry.begin(), d_aliasRegistry.end());
}


/*************************************************************************
	Add a window type alias mapping
*************************************************************************/
void WindowFactoryManager::addWindowTypeAlias(const String& aliasName, const String& targetType)
{
	TypeAliasRegistry::iterator pos = d_aliasRegistry.find(aliasName);

	if (pos == d_aliasRegistry.end())
	{
		d_aliasRegistry[aliasName].d_targetStack.push_back(targetType);
	}
	// alias already exists, add our new entry to the list already there
	else
	{
		pos->second.d_targetStack.push_back(targetType);
	}

	Logger::getSingleton().logEvent("Window type alias named '" + aliasName + "' added for window type '" + targetType +"'.");
}


/*************************************************************************
	Remove a window type alias mapping
*************************************************************************/
void WindowFactoryManager::removeWindowTypeAlias(const String& aliasName, const String& targetType)
{
	// find alias name
	TypeAliasRegistry::iterator pos = d_aliasRegistry.find(aliasName);

	// if alias name exists
	if (pos != d_aliasRegistry.end())
	{
		// find the specified target for this alias
		AliasTargetStack::TargetTypeStack::iterator aliasPos = std::find(pos->second.d_targetStack.begin(), pos->second.d_targetStack.end(), targetType);

		// if the target exists for this alias
		if (aliasPos != pos->second.d_targetStack.end())
		{
			// erase the target mapping
			pos->second.d_targetStack.erase(aliasPos);

			Logger::getSingleton().logEvent("Window type alias named '" + aliasName + "' removed for window type '" + targetType +"'.");

			// if the list of targets for this alias is now empty
			if (pos->second.d_targetStack.empty())
			{
				// erase the alias name also
				d_aliasRegistry.erase(aliasName);

				Logger::getSingleton().logEvent("Window type alias named '" + aliasName + "' has no more targets and has been removed.", Informative);
			}

		}

	}

}

void WindowFactoryManager::removeAllWindowTypeAliases()
{
	d_aliasRegistry.clear();
}

void WindowFactoryManager::addFalagardWindowMapping(const String& newType,
                                                    const String& targetType,
                                                    const String& lookName,
                                                    const String& renderer,
                                                    const String& effectName)
{
    WindowFactoryManager::FalagardWindowMapping mapping;
    mapping.d_windowType = newType;
    mapping.d_baseType   = targetType;
    mapping.d_lookName   = lookName;
    mapping.d_rendererType = renderer;
    mapping.d_effectName = effectName;

    // see if the type we're creating already exists
    if (d_falagardRegistry.find(newType) != d_falagardRegistry.end())
    {
        // type already exists, log the fact that it's going to be replaced.
        Logger::getSingleton().logEvent("Falagard mapping for type '" + newType + "' already exists - current mapping will be replaced.");
    }

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(&mapping));
    Logger::getSingleton().logEvent("Creating falagard mapping for type '" +
        newType + "' using base type '" + targetType + "', window renderer '" +
        renderer + "' Look'N'Feel '" + lookName + "' and RenderEffect '" +
        effectName + "'. " + addr_buff);

    d_falagardRegistry[newType] = mapping;
}

void WindowFactoryManager::removeFalagardWindowMapping(const String& type)
{
    FalagardMapRegistry::iterator iter = d_falagardRegistry.find(type);

    if (iter != d_falagardRegistry.end())
    {
        Logger::getSingleton().logEvent("Removing falagard mapping for type '" + type + "'.");
        d_falagardRegistry.erase(iter);
    }
}

void WindowFactoryManager::removeAllFalagardWindowMappings()
{
	d_falagardRegistry.clear();
}

WindowFactoryManager::FalagardMappingIterator WindowFactoryManager::getFalagardMappingIterator() const
{
    return FalagardMappingIterator(d_falagardRegistry.begin(), d_falagardRegistry.end());
}

bool WindowFactoryManager::isFalagardMappedType(const String& type) const
{
    return d_falagardRegistry.find(getDereferencedAliasType(type)) != d_falagardRegistry.end();
}

const String& WindowFactoryManager::getMappedLookForType(const String& type) const
{
    FalagardMapRegistry::const_iterator iter =
        d_falagardRegistry.find(getDereferencedAliasType(type));

    if (iter != d_falagardRegistry.end())
    {
        return (*iter).second.d_lookName;
    }
    // type does not exist as a mapped type (or an alias for one)
    else
    {
        CEGUI_THROW(InvalidRequestException(
            "Window factory type '" + type +
            "' is not a falagard mapped type (or an alias for one)."));
    }
}

const String& WindowFactoryManager::getMappedRendererForType(const String& type) const
{
    FalagardMapRegistry::const_iterator iter =
        d_falagardRegistry.find(getDereferencedAliasType(type));

    if (iter != d_falagardRegistry.end())
    {
        return (*iter).second.d_rendererType;
    }
    // type does not exist as a mapped type (or an alias for one)
    else
    {
        CEGUI_THROW(InvalidRequestException(
            "Window factory type '" + type +
            "' is not a falagard mapped type (or an alias for one)."));
    }
}

String WindowFactoryManager::getDereferencedAliasType(const String& type) const
{
    TypeAliasRegistry::const_iterator alias = d_aliasRegistry.find(type);

    // if this is an aliased type, ensure to fully dereference by recursively
    // calling ourselves on the active target for the given type.
    if (alias != d_aliasRegistry.end())
        return getDereferencedAliasType(alias->second.getActiveTarget());

    // we're not an alias, so return the input type unchanged
    return type;
}

const WindowFactoryManager::FalagardWindowMapping& WindowFactoryManager::getFalagardMappingForType(const String& type) const
{
    FalagardMapRegistry::const_iterator iter =
        d_falagardRegistry.find(getDereferencedAliasType(type));

    if (iter != d_falagardRegistry.end())
    {
        return (*iter).second;
    }
    // type does not exist as a mapped type (or an alias for one)
    else
    {
        CEGUI_THROW(InvalidRequestException(
            "Window factory type '" + type +
            "' is not a falagard mapped type (or an alias for one)."));
    }
}


//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Methods for AliasTargetStack class
*************************************************************************/
//////////////////////////////////////////////////////////////////////////
const String& WindowFactoryManager::AliasTargetStack::getActiveTarget(void) const
{
	return d_targetStack.back();
}


uint WindowFactoryManager::AliasTargetStack::getStackedTargetCount(void) const
{
	return (uint)d_targetStack.size();
}


} // End of  CEGUI namespace section
