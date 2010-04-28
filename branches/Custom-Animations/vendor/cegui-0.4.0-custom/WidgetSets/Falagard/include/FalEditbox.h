/************************************************************************
    filename:   FalEditbox.h
    created:    Sat Jun 25 2005
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
#ifndef _FalEditbox_h_
#define _FalEditbox_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIEditbox.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Editbox class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled    - Rendering for when the editbox is in enabled and is in read-write mode.
            - ReadOnly  - Rendering for when the editbox is in enabled and is in read-only mode.
            - Disabled  - Rendering for when the editbox is disabled.
            - ActiveSelection   - additional state rendered for text selection (the imagery in this section is rendered within the selection area.)
            - InactiveSelection - additional state rendered for text selection (the imagery in this section is rendered within the selection area.)

        NamedAreas:
            TextArea    - area where text, selection, and carat imagery will appear.

        Imagery Sections:
            - Carat
    */
    class FALAGARDBASE_API FalagardEditbox : public Editbox
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardEditbox(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardEditbox();

    protected:
        // overridden from Editbox base class.
        void populateRenderCache();
        size_t getTextIndexFromPosition(const Point& pt) const;

        // rendering internal vars
        float	d_lastTextOffset;		//!< x rendering offset used last time we drew the widget.
    };

    /*!
    \brief
        WindowFactory for FalagardEditbox type Window objects.
    */
    class FALAGARDBASE_API FalagardEditboxFactory : public WindowFactory
    {
    public:
        FalagardEditboxFactory(void) : WindowFactory(FalagardEditbox::WidgetTypeName) { }
        ~FalagardEditboxFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalEditbox_h_
