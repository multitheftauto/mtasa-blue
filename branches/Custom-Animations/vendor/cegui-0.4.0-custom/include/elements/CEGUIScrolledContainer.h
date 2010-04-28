/************************************************************************
	filename: 	CEGUIScrolledContainer.h
	created:	1/3/2005
	author:		Paul D Turner
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
#ifndef _CEGUIScrolledContainer_h_
#define _CEGUIScrolledContainer_h_

#include "CEGUIWindow.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIScrolledContainerProperties.h"
#include <map>

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Helper container window class which is used in the implementation of the
        ScrollablePane widget class.
    */
    class CEGUIEXPORT ScrolledContainer : public Window
    {
    public:
        /*************************************************************************
            Constants
        *************************************************************************/
        static const String WidgetTypeName;     //!< Type name for ScrolledContainer.
        static const String EventNamespace;     //!< Namespace for global events
        static const String EventContentChanged;    //!< Event fired whenever some child changes.
        static const String EventAutoSizeSettingChanged;    //!< Event fired when the autosize setting changes.

        /*************************************************************************
        	Object construction and destruction
        *************************************************************************/
        /*!
        \brief
            Constructor for ScrolledContainer objects.
        */
        ScrolledContainer(const String& type, const String& name);

        /*!
        \brief
            Destructor for ScrolledContainer objects.
        */
        ~ScrolledContainer(void);

        /*************************************************************************
        	Public interface methods
        *************************************************************************/
        /*!
        \brief
            Return whether the content pane is auto sized.

        \return
            - true to indicate the content pane will automatically resize itself.
            - false to indicate the content pane will not automatically resize itself.
        */
        bool isContentPaneAutoSized(void) const;

        /*!
        \brief
            Set whether the content pane should be auto-sized.

        \param setting
            - true to indicate the content pane should automatically resize itself.
            - false to indicate the content pane should not automatically resize itself.

        \return 
            Nothing.
        */
        void setContentPaneAutoSized(bool setting);

        /*!
        \brief
            Return the current content pane area for the ScrolledContainer.

        \return
            Rect object that details the current pixel extents of the content
            pane represented by this ScrolledContainer.
        */
        const Rect& getContentArea(void) const;

        /*!
        \brief
            Set the current content pane area for the ScrolledContainer.

        \note
            If the ScrolledContainer is configured to auto-size the content pane
            this call will have no effect.

        \param area
            Rect object that details the pixel extents to use for the content
            pane represented by this ScrolledContainer.

        \return
            Nothing.
        */
        void setContentArea(const Rect& area);

        /*!
        \brief
            Return the current extents of the attached content.

        \return
            Rect object that describes the pixel extents of the attached
            child windows.  This is effectively the smallest bounding box
            that could contain all the attached windows.
        */
        Rect getChildExtentsArea(void) const;

    protected:
        /*************************************************************************
        	Implementation methods
        *************************************************************************/
        /*!
        \brief
            Adds events for this widget type.
        */
        void addScrolledContainerEvents(bool bCommon=true);
	    void addUncommonEvents( void )							{ __super::addUncommonEvents(); addScrolledContainerEvents(false); }


		/*!
		\brief
			Return whether this window was inherited from the given class name at some point in the inheritance heirarchy.

		\param class_name
			The class name that is to be checked.

		\return
			true if this window was inherited from \a class_name. false if not.
		*/
		virtual bool	testClassName_impl(const String& class_name) const
		{
			if (class_name==(const utf8*)"ScrolledContainer")	return true;
			return Window::testClassName_impl(class_name);
		}

        /*************************************************************************
        	Implementation of abstract methods from Window
        *************************************************************************/
        void drawSelf(float z) {};

        /*************************************************************************
        	Event trigger methods.
        *************************************************************************/
        /*!
        \brief
            Notification method called whenever the content size may have changed.

        \param e
            WindowEventArgs object.

        \return
            Nothing.
        */
        virtual void onContentChanged(WindowEventArgs& e);

        /*!
        \brief
            Notification method called whenever the setting that controls whether
            the content pane is automatically sized is changed.

        \param e
            WindowEventArgs object.

        \return
            Nothing.
        */
        virtual void onAutoSizeSettingChanged(WindowEventArgs& e);

        /*************************************************************************
        	Event callbacks
        *************************************************************************/
        /*!
        \brief
            Method which receives notifications about child windows being moved.
        */
        bool handleChildSized(const EventArgs& e);

        /*!
        \brief
            Method which receives notifications about child windows being sized.
        */
        bool handleChildMoved(const EventArgs& e);

        /*************************************************************************
        	Overridden from Window.
        *************************************************************************/
        Rect getUnclippedInnerRect(void) const;
        void onChildAdded(WindowEventArgs& e);
        void onChildRemoved(WindowEventArgs& e);
        void onParentSized(WindowEventArgs& e);

        /*************************************************************************
        	Data fields
        *************************************************************************/
        typedef std::multimap<Window*, Event::Connection>  ConnectionTracker;
        ConnectionTracker d_eventConnections;   //!< Tracks event connections we make.
        Rect d_contentArea;     //!< Holds extents of the content pane.
        bool d_autosizePane;    //!< true if the pane auto-sizes itself.

    private:
	    /*************************************************************************
		    Static Properties for this class
	    *************************************************************************/
	    static ScrolledContainerProperties::ContentPaneAutoSized	d_autoSizedProperty;
	    static ScrolledContainerProperties::ContentArea             d_contentAreaProperty;
	    static ScrolledContainerProperties::ChildExtentsArea        d_childExtentsAreaProperty;

	    /*************************************************************************
		    Private methods
	    *************************************************************************/
	    void addScrolledContainerProperties( bool bCommon = true );
		void addUncommonProperties( void )							{ __super::addUncommonProperties(); addScrolledContainerProperties(false); }
    };

    /*!
    \brief
        Factory class for producing ScrolledContainer windows
    */
    class ScrolledContainerFactory : public WindowFactory
    {
    public:
        ScrolledContainerFactory(void) : WindowFactory(ScrolledContainer::WidgetTypeName) { }
        ~ScrolledContainerFactory(void){}

        Window* createWindow(const String& name)
        {
            ScrolledContainer* wnd = new ScrolledContainer(d_type, name);
            return wnd;
        }

        void destroyWindow(Window* window)
        {
            if (window->getType() == d_type)
                delete window;
        }

    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIScrolledContainer_h_
