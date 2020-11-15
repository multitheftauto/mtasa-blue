/***********************************************************************
    created:    Fri Oct 07 2011
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/FactoryModule.h"
#include "CEGUI/FactoryRegisterer.h"
#include "CEGUI/Exceptions.h"
#include <stdio.h>
// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
FactoryModule::~FactoryModule()
{}

//----------------------------------------------------------------------------//
void FactoryModule::registerFactory(const String& type_name)
{
    FactoryRegistry::iterator i = d_registry.begin();
    for ( ; i != d_registry.end(); ++i)
    {
        if ((*i)->d_type == type_name)
        {
            (*i)->registerFactory();
            return;
        }
    }

    CEGUI_THROW(UnknownObjectException("No factory for type '" +
        type_name + "' in this module."));
}

//----------------------------------------------------------------------------//
uint FactoryModule::registerAllFactories()
{
    FactoryRegistry::iterator i = d_registry.begin();
    for ( ; i != d_registry.end(); ++i)
        (*i)->registerFactory();

    return static_cast<uint>(d_registry.size());
}

//----------------------------------------------------------------------------//
void FactoryModule::unregisterFactory(const String& type_name)
{
    FactoryRegistry::iterator i = d_registry.begin();
    for ( ; i != d_registry.end(); ++i)
    {
        if ((*i)->d_type == type_name)
        {
            (*i)->unregisterFactory();
            return;
        }
    }

}

//----------------------------------------------------------------------------//
uint FactoryModule::unregisterAllFactories()
{
    FactoryRegistry::iterator i = d_registry.begin();
    for ( ; i != d_registry.end(); ++i)
        (*i)->unregisterFactory();

    return static_cast<uint>(d_registry.size());
}

//----------------------------------------------------------------------------//
#if defined(CEGUI_STATIC) && defined(CEGUI_BUILD_STATIC_FACTORY_MODULE)
extern "C"
{
    CEGUI::FactoryModule& getWindowFactoryModule() 
    {
        static CEGUI::FactoryModule mod;
        return mod;
    }
}
#endif

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

