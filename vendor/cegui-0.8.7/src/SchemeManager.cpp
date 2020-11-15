/***********************************************************************
    created:    Mon Jul 20 2009
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
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
#include "CEGUI/SchemeManager.h"
#include "CEGUI/Logger.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
template<> SchemeManager* Singleton<SchemeManager>::ms_Singleton = 0;

//----------------------------------------------------------------------------//
SchemeManager::SchemeManager() :
    NamedXMLResourceManager<Scheme, Scheme_xmlHandler>("Scheme"),
    
    d_autoLoadResources(true)
{
    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
    "CEGUI::SchemeManager singleton created. " + String(addr_buff));
}

//----------------------------------------------------------------------------//
SchemeManager::~SchemeManager()
{
    Logger::getSingleton().logEvent(
        "---- Begining cleanup of GUI Scheme system ----");

    destroyAll();

    char addr_buff[32];
    sprintf(addr_buff, "(%p)", static_cast<void*>(this));
    Logger::getSingleton().logEvent(
        "CEGUI::SchemeManager singleton destroyed. " + String(addr_buff));
}

//----------------------------------------------------------------------------//
SchemeManager::SchemeIterator SchemeManager::getIterator(void) const
{
    return SchemeIterator(d_objects.begin(), d_objects.end());
}

//----------------------------------------------------------------------------//
void SchemeManager::setAutoLoadResources(bool enabled)
{
    d_autoLoadResources = enabled;
}

//----------------------------------------------------------------------------//
bool SchemeManager::getAutoLoadResources() const
{
    return d_autoLoadResources;
}

//----------------------------------------------------------------------------//
void SchemeManager::doPostObjectAdditionAction(Scheme& object)
{
    if (d_autoLoadResources)
    {
        object.loadResources();
    }
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
