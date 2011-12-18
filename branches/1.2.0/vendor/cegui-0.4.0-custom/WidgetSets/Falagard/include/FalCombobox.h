/************************************************************************
    filename:   FalCombobox.h
    created:    Mon Jul 4 2005
    author:     Paul D Turner <paul@cegui.org.uk>
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
#ifndef _FalCombobox_h_
#define _FalCombobox_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUICombobox.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Combobox class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Normal
            - Disabled

        Child Widgets:
            PushButton based widget with name suffix "__auto_button__"
            Editbox based widget with name suffix "__auto_editbox__"
            ComboDropList based widget with name suffix "__auto_droplist__"


    */
    class FALAGARDBASE_API FalagardCombobox : public Combobox
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardCombobox(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardCombobox();

    protected:
        // overridden from Combobox base class.
        void populateRenderCache();
        Editbox* createEditbox(const String& name) const;
        PushButton* createPushButton(const String& name) const;
        ComboDropList* createDropList(const String& name) const;
    };

    /*!
    \brief
        WindowFactory for FalagardCombobox type Window objects.
    */
    class FALAGARDBASE_API FalagardComboboxFactory : public WindowFactory
    {
    public:
        FalagardComboboxFactory(void) : WindowFactory(FalagardCombobox::WidgetTypeName) { }
        ~FalagardComboboxFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalCombobox_h_
