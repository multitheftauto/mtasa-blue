/************************************************************************
	filename: 	CEGUIResourceProvider.h
	created:	8/7/2004
	author:		James '_mental_' O'Sullivan
	
	purpose:	Defines abstract base class for loading DataContainer objects
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
#ifndef _CEGUIResourceProvider_h_
#define _CEGUIResourceProvider_h_

#include "CEGUIBase.h"
#include "CEGUIDataContainer.h"
#include "CEGUIString.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Abstract class that defines the required interface for all resource provider sub-classes.

	A ResourceProvider is used to load both XML and binary data from an external source.  This could be from a filesystem or the resource manager of a specific renderer.
*/
class CEGUIEXPORT ResourceProvider
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
    /*!
    \brief
        Constructor for the ResourceProvider class
    */
	ResourceProvider() { }

    /*!
    \brief
        Destructor for the ResourceProvider class
    */
	virtual ~ResourceProvider(void) { }

    /*************************************************************************
        Accessor functions
    *************************************************************************/

//    /*!
//    \brief
//        Load XML data using InputSource objects.
//
//    \param filename
//        String containing a filename of the resource to be loaded.
//
//	\param output
//		Reference to a InputSourceContainer object to load the data into.
//   */
//    virtual void loadInputSourceContainer(const String& filename, InputSourceContainer& output) = 0;

    /*!
    \brief
        Load raw binary data.

    \param filename
        String containing a filename of the resource to be loaded.

	\param output
        Reference to a RawDataContainer object to load the data into.

    \param resourceGroup
        Optional String that may be used by implementations to identify the group from
        which the resource should be loaded.
    */
    virtual void loadRawDataContainer(const String& filename, RawDataContainer& output, const String& resourceGroup) = 0;

    /*!
    \brief
        Unload raw binary data. This gives the resource provider a change to unload the data
        in its own way before the data container object is destroyed.  If it does nothing,
        then the object will release its memory.

	\param data
        Reference to a RawDataContainer object that is about to be destroyed.

    */
    virtual void unloadRawDataContainer(RawDataContainer& data)  { }

    /*!
    \brief
        Return the current default resource group identifier.

    \return
        String object containing the currently set default resource group identifier.
    */
    const String&   getDefaultResourceGroup(void) const     { return d_defaultResourceGroup; }
    
    /*!
    \brief
        Set the default resource group identifier.

    \param resourceGroup
        String object containing the default resource group identifier to be used.

    \return
        Nothing.
    */
    void    setDefaultResourceGroup(const String& resourceGroup)    { d_defaultResourceGroup = resourceGroup; }

protected:
    String  d_defaultResourceGroup;     //!< Default resource group identifier.
};

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIResourceProvider_h_
