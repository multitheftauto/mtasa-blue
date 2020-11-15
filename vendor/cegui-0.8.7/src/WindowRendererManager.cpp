/*************************************************************************
 *
 *
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
#include "CEGUI/WindowRendererManager.h"
#include "CEGUI/Exceptions.h"
#include <algorithm>

// Start CEGUI namespace
namespace CEGUI
{

/*************************************************************************
    Static data
*************************************************************************/
template<> WindowRendererManager* Singleton<WindowRendererManager>::ms_Singleton = 0;
WindowRendererManager::OwnedFactoryList WindowRendererManager::d_ownedFactories;

/*************************************************************************
    Singleton functions
*************************************************************************/
WindowRendererManager& WindowRendererManager::getSingleton(void)
{
    return Singleton<WindowRendererManager>::getSingleton();
}
WindowRendererManager* WindowRendererManager::getSingletonPtr(void)
{
    return Singleton<WindowRendererManager>::getSingletonPtr();
}

/*************************************************************************
    Constructor / Destructor
*************************************************************************/
WindowRendererManager::WindowRendererManager()
{
    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
        "CEGUI::WindowRendererManager singleton created " + String(addr_buff));

    // complete addition of any pre-added WindowRendererFactory objects
    OwnedFactoryList::iterator i = d_ownedFactories.begin();

    if (d_ownedFactories.end() != i)
    {
        Logger::getSingleton().logEvent(
        "---- Adding pre-registered WindowRendererFactory objects ----");

        for (; d_ownedFactories.end() != i; ++i)
            addFactory(*i);
    }
}

WindowRendererManager::~WindowRendererManager()
{
    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
        "CEGUI::WindowRendererManager singleton destroyed " + String(addr_buff));
}

/*************************************************************************
    Is there a WindowRenderer by this name?
*************************************************************************/
bool WindowRendererManager::isFactoryPresent(const String& name) const
{
    return (d_wrReg.find(name) != d_wrReg.end());
}

/*************************************************************************
    Get the named WindowRenderer
*************************************************************************/
WindowRendererFactory* WindowRendererManager::getFactory(const String& name) const
{
    WR_Registry::const_iterator i = d_wrReg.find(name);
    if (i != d_wrReg.end())
    {
        return (*i).second;
    }
    CEGUI_THROW(UnknownObjectException("There is no WindowRendererFactory named '"+name+"' available"));
}

/*************************************************************************
    Add a new WindowRenderer factory
*************************************************************************/
void WindowRendererManager::addFactory(WindowRendererFactory* wr)
{
    if (wr == 0)
    {
        return;
    }
    if (d_wrReg.insert(std::make_pair(wr->getName(), wr)).second == false)
    {
        CEGUI_THROW(AlreadyExistsException("A WindowRendererFactory named '"+wr->getName()+"' already exist"));
    }

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(wr));
    Logger::getSingleton().logEvent("WindowRendererFactory '"+wr->getName()+
        "' added. " + addr_buff);
}

/*************************************************************************
    Remove a factory by name
*************************************************************************/
void WindowRendererManager::removeFactory(const String& name)
{
    WR_Registry::iterator i = d_wrReg.find(name);

	// non-existing or already removed? The latter can happen when more then one Scheme
	// was loaded using the same renderer.
	if (i == d_wrReg.end())
	{
		return;
	}

    // see if we own this factory
    OwnedFactoryList::iterator j = std::find(d_ownedFactories.begin(),
                                             d_ownedFactories.end(),
                                             (*i).second);

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>((*i).second));

    d_wrReg.erase(name);

    Logger::getSingleton().logEvent("WindowRendererFactory for '" + name +
                                    "' WindowRenderers removed. " + addr_buff);

    // delete factory object if we created it
    if (j != d_ownedFactories.end())
    {
        Logger::getSingleton().logEvent("Deleted WindowRendererFactory for '" +
                                        (*j)->getName() +
                                        "' WindowRenderers.");

        CEGUI_DELETE_AO (*j);
        d_ownedFactories.erase(j);
    }
}

/*************************************************************************
    Create a WindowRenderer instance by factory name
*************************************************************************/
WindowRenderer* WindowRendererManager::createWindowRenderer(const String& name)
{
    WindowRendererFactory* factory = getFactory(name);
    return factory->create();
}

/*************************************************************************
    Destroy a WindowRenderer using its factory
*************************************************************************/
void WindowRendererManager::destroyWindowRenderer(WindowRenderer* wr)
{
    WindowRendererFactory* factory = getFactory(wr->getName());
    factory->destroy(wr);
}

} // CEGUI
