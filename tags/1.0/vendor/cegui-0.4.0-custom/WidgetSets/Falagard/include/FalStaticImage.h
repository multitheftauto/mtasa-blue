/************************************************************************
    filename:   FalStaticImage.h
    created:    Tue Jul 5 2005
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
#ifndef _FalStaticImage_h_
#define _FalStaticImage_h_

#include "FalModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIStaticImage.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        StaticImage class for the FalagardBase module.

        This class requires LookNFeel to be assigned.  The LookNFeel should provide the following:

        States:
            - Enabled            - basic rendering for enabled state.
            - Disabled           - basic rendering for disabled state.
            - EnabledFrame       - frame rendering for enabled state
            - DisabledFrame      - frame rendering for disabled state.
            - EnabledBackground  - backdrop rendering for enabled state
            - DisabledBackground - backdrop rendering for disabled state

        Named Areas:
            - WithFrameImageRenderArea  - Area to render image into when the frame is enabled.
            - NoFrameImageRenderArea    - Area to render image into when the frame is disabled.
    */
    class FALAGARDBASE_API FalagardStaticImage : public StaticImage
    {
    public:
        static const utf8   WidgetTypeName[];       //!< type name for this widget.

        /*!
        \brief
            Constructor
        */
        FalagardStaticImage(const String& type, const String& name);

        /*!
        \brief
            Destructor
        */
        ~FalagardStaticImage();

		// overridden from StaticImage base class.
		Rect getUnclippedInnerRect(void) const;

    protected:
        // overridden from StaticImage base class.
        void populateRenderCache();
    };

    /*!
    \brief
        WindowFactory for FalagardStaticImage type Window objects.
    */
    class FALAGARDBASE_API FalagardStaticImageFactory : public WindowFactory
    {
    public:
        FalagardStaticImageFactory(void) : WindowFactory(FalagardStaticImage::WidgetTypeName) { }
        ~FalagardStaticImageFactory(void){}
        Window* createWindow(const String& name);
        void destroyWindow(Window* window);
    };

} // End of  CEGUI namespace section


#endif  // end of guard _FalStaticImage_h_
