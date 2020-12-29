/************************************************************************
    filename:   FalProgressBar.h
    created:    Sat Jul 2 2005
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
#ifndef _FalProgressBar_h_
#define _FalProgressBar_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIProgressBar.h"
#include "FalProgressBarProperties.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        ProgressBar class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled
            - Disabled
            - EnabledProgress
            - DisabledProgress

        Named Areas:
            - ProgressArea
    */
    class FALAGARDBASE_API FalagardProgressBar : public ProgressBar
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardProgressBar(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardProgressBar();

        bool isVertical() const;
        bool isReversed() const;

        void setVertical(bool setting);
        void setReversed(bool setting);

    protected:
        // overridden from ProgressBar base class.
        void populateRenderCache();

        // settings to make this class universal.
        bool d_vertical;    //!< True if progress bar operates on the vertical plane.
        bool d_reversed;    //!< True if progress grows in the opposite direction to usual (i.e. to the left / downwards).

        // property objects
        static FalagardProgressBarProperties::VerticalProgress d_verticalProperty;
        static FalagardProgressBarProperties::ReversedProgress d_reversedProperty;
    };

    /*!
    \brief
        WindowFactory for FalagardProgressBar type Window objects.
    */
    class FALAGARDBASE_API FalagardProgressBarFactory : public WindowFactory
    {
    public:
        FalagardProgressBarFactory(void) : WindowFactory(FalagardProgressBar::WidgetTypeName) { }
        ~FalagardProgressBarFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif  // end of guard _FalProgressBar_h_
