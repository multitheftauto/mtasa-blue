/************************************************************************
    filename:   FalMultiLineEditbox.h
    created:    Thu Jul 7 2005
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
#ifndef _FalMultiLineEditbox_h_
#define _FalMultiLineEditbox_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIMultiLineEditbox.h"
#include "FalMultiLineEditboxProperties.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
   /*!
    \brief
        MultiLineEditbox class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled    - Rendering for when the editbox is in enabled and is in read-write mode.
            - ReadOnly  - Rendering for when the editbox is in enabled and is in read-only mode.
            - Disabled  - Rendering for when the editbox is disabled.

        NamedAreas:
            TextArea         - area where text, selection, and carat imagery will appear.
            TextAreaHScroll  - TextArea when only horizontal scrollbar is visible.
            TextAreaVScroll  - TextArea when only vertical scrollbar is visible.
            TextAreaHVScroll - TextArea when both horizontal and vertical scrollbar is visible.

        Imagery Sections:
            - Carat

        Child Widgets:
            Scrollbar based widget with name suffix "__auto_vscrollbar__"
            Scrollbar based widget with name suffix "__auto_hscrollbar__"

    */
    class FALAGARDBASE_API FalagardMultiLineEditbox : public MultiLineEditbox
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardMultiLineEditbox(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardMultiLineEditbox();

        const Image* getSelectionBrushImage() const;
        void setSelectionBrushImage(const Image* image);

    protected:
        // overridden from MultiLineEditbox base class.
        Scrollbar* createVertScrollbar(const String& name) const;
        Scrollbar* createHorzScrollbar(const String& name) const;
        void cacheEditboxBaseImagery();
        void cacheCaratImagery(const Rect& textArea);
        Rect getTextRenderArea(void) const;

        // properties
        static FalagardMultiLineEditboxProperties::SelectionBrushImage d_selectionBrushProperty;
    };

    /*!
    \brief
        WindowFactory for FalagardMultiLineEditbox type Window objects.
    */
    class FALAGARDBASE_API FalagardMultiLineEditboxFactory : public WindowFactory
    {
    public:
        FalagardMultiLineEditboxFactory(void) : WindowFactory(FalagardMultiLineEditbox::WidgetTypeName) { }
        ~FalagardMultiLineEditboxFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _FalMultiLineEditbox_h_
