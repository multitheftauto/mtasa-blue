/************************************************************************
	filename: 	CEGUIGUILayout_xmlHandler.h
	created:	5/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to XML parser for GUILayout files
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
#ifndef _CEGUIGUILayout_xmlHandler_h_
#define _CEGUIGUILayout_xmlHandler_h_

#include "CEGUIWindowManager.h"
#include "CEGUIWindow.h"
#include "CEGUIXMLHandler.h"

#include <vector>

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Handler class used to parse the GUILayout XML files using SAX2
*/
class GUILayout_xmlHandler : public XMLHandler
{
	typedef WindowManager::PropertyCallback PropertyCallback;
public:
	/*************************************************************************
		Construction & Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for GUILayout_xmlHandler objects
	*/
	GUILayout_xmlHandler(const String& name_prefix, PropertyCallback* callback = NULL, void* userdata = NULL) : 
	  d_root(NULL),
	  d_namingPrefix(name_prefix),
	  d_propertyCallback(callback),
	  d_userData(userdata)
	  {}

	/*!
	\brief
		Destructor for GUILayout_xmlHandler objects
	*/
	virtual ~GUILayout_xmlHandler(void) {}

	/*************************************************************************
		SAX2 Handler overrides
	*************************************************************************/ 
	/*!
	\brief
		document processing (only care about elements, schema validates format)
	*/
    virtual void elementStart(const String& element, const XMLAttributes& attributes);
    virtual void elementEnd(const String& element);

	/*************************************************************************
		Functions used by our implementation
	*************************************************************************/
	/*!
	\brief
		Destroy all windows created so far.
	*/
	void	cleanupLoadedWindows(void);


	/*!
	\brief
		Return a pointer to the 'root' window created.
	*/
	Window*	getLayoutRootWindow(void) const;

private:
	/*************************************************************************
		Implementation Constants
	*************************************************************************/
	static const String GUILayoutElement;				//!< Tag name for GUILayout elements.
	static const String WindowElement;				//!< Tag name for Window elements.
	static const String PropertyElement;				//!< Tag name for Property elements.
	static const String LayoutImportElement;			//!< Tag name for LayoutImport elements.
	static const String EventElement;					//!< Tag name for Event elements.
	static const char	WindowTypeAttribute[];			//!< Attribute name that stores the type of Window to create.
	static const char	WindowNameAttribute[];			//!< Attribute name that stores the name of the window to create.
	static const char	PropertyNameAttribute[];		//!< Attribute name that stores the name of the property to set.
	static const char	PropertyValueAttribute[];		//!< Attribute name that stores the value to pass to the property.
	static const char	LayoutParentAttribute[];		//!< Attribute name that stores the name of the window to attach the layout to.
	static const char	LayoutImportFilenameAttribute[];//!< Attribute name that stores the file name of the layout to import.
	static const char	LayoutImportPrefixAttribute[];	//!< Attribute name that stores the prefix to use when loading the imported layout.
    static const char	LayoutImportResourceGroupAttribute[]; //!< Attribute name that stores the resource group identifier used when loading imported file.
	static const char	EventNameAttribute[];			//!< Attribute name that stores the event name to be subscribed.
	static const char	EventFunctionAttribute[];		//!< Attribute name that stores the name of the scripted function to be bound.


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	typedef std::vector<Window*>	WindowStack;	
	Window*	d_root;				//!< Will point to first window created.
	WindowStack	d_stack;		//!< Stack used to keep track of what we're doing to which window.
	String		d_layoutParent;	//!< Name of the parent window to attach the loaded layout to.
	const String&		d_namingPrefix;	//!< Prefix that is to prepend all names of created windows.
	PropertyCallback*	d_propertyCallback; //!< Callback for every property loaded
	void*				d_userData;			//!< User data for the property callback
};


} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIGUILayout_xmlHandler_h_
