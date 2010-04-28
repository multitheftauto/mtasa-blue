/************************************************************************
	filename: 	CEGUIScrollablePane.h
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
#ifndef _CEGUIScrollablePane_h_
#define _CEGUIScrollablePane_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUIScrollablePaneProperties.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Base class for the ScrollablePane widget.

        The ScrollablePane widget allows child windows to be attached which cover an area
        larger than the ScrollablePane itself and these child windows can be scrolled into
        view using the scrollbars of the scrollable pane.
    */
    class CEGUIEXPORT ScrollablePane : public Window
    {
    public:
        /*************************************************************************
            Constants
        *************************************************************************/
        static const String EventNamespace;                 //!< Namespace for global events
        static const String EventContentPaneChanged;        //!< Event fired when an area on the content pane has been updated.
        static const String EventVertScrollbarModeChanged;	//!< Event triggered when the vertical scroll bar 'force' setting changes.
        static const String EventHorzScrollbarModeChanged;	//!< Event triggered when the horizontal scroll bar 'force' setting changes.
        static const String EventAutoSizeSettingChanged;    //!< Event fired when the auto size setting changes.
        static const String EventContentPaneScrolled;       //!< Event fired when the pane gets scrolled.

        /*************************************************************************
            Construction / Destruction
        *************************************************************************/
        /*!
        \brief
            Constructor for the ScrollablePane base class.
        */
        ScrollablePane(const String& type, const String& name);

        /*!
        \brief
            Destructor for the ScrollablePane base class.
        */
        ~ScrollablePane(void);

        /*************************************************************************
        	Public interface
        *************************************************************************/
        /*!
        \brief
            Returns a pointer to the window holding the pane contents.

            The purpose of this is so that attached windows may be inspected,
            client code may not modify the returned window in any way.

        \return
            Pointer to the ScrolledContainer that is acting as the container for the
            scrollable pane content.  The returned window is const, client code should
            not modify the ScrolledContainer settings directly.
        */
        const ScrolledContainer* getContentPane(void) const;

        /*!
        \brief
            Return whether the vertical scroll bar is always shown.

        \return
            - true if the scroll bar will always be shown even if it is not required.
            - false if the scroll bar will only be shown when it is required.
        */
        bool	isVertScrollbarAlwaysShown(void) const;
        /*!
        \brief
            Set whether the vertical scroll bar should always be shown.

        \param setting
            - true if the vertical scroll bar should be shown even when it is not required.
            - false if the vertical scroll bar should only be shown when it is required.

        \return
            Nothing.
        */
        void	setShowVertScrollbar(bool setting);

        /*!
        \brief
            Return whether the horizontal scroll bar is always shown.

        \return
            - true if the scroll bar will always be shown even if it is not required.
            - false if the scroll bar will only be shown when it is required.
        */
        bool	isHorzScrollbarAlwaysShown(void) const;

        /*!
        \brief
            Set whether the horizontal scroll bar should always be shown.

        \param setting
            - true if the horizontal scroll bar should be shown even when it is not required.
            - false if the horizontal scroll bar should only be shown when it is required.

        \return
            Nothing.
        */
        void	setShowHorzScrollbar(bool setting);

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
            Return the current content pane area for the ScrollablePane.

        \return
            Rect object that details the current pixel extents of the content
            pane attached to this ScrollablePane.
        */
        const Rect& getContentPaneArea(void) const;

        /*!
        \brief
            Set the current content pane area for the ScrollablePane.

        \note
            If the ScrollablePane is configured to auto-size the content pane
            this call will have no effect.

        \param area
            Rect object that details the pixel extents to use for the content
            pane attached to this ScrollablePane.

        \return
            Nothing.
        */
        void setContentPaneArea(const Rect& area);

        /*!
        \brief
            Returns the horizontal scrollbar step size as a fraction of one
            complete view page.

        \return
            float value specifying the step size where 1.0f would be the width of
            the viewing area.
        */
        float getHorizontalStepSize(void) const;

        /*!
        \brief
            Sets the horizontal scrollbar step size as a fraction of one
            complete view page.

        \param step
            float value specifying the step size, where 1.0f would be the width of
            the viewing area.

        \return
            Nothing.
        */
        void setHorizontalStepSize(float step);

        /*!
        \brief
            Returns the horizontal scrollbar overlap size as a fraction of one
            complete view page.

        \return
            float value specifying the overlap size where 1.0f would be the width of
            the viewing area.
        */
        float getHorizontalOverlapSize(void) const;

        /*!
        \brief
            Sets the horizontal scrollbar overlap size as a fraction of one
            complete view page.

        \param overlap
            float value specifying the overlap size, where 1.0f would be the width of
            the viewing area.

        \return
            Nothing.
        */
        void setHorizontalOverlapSize(float overlap);

        /*!
        \brief
            Returns the horizontal scroll position as a fraction of the
            complete scrollable width.

        \return
            float value specifying the scroll position.
        */
        float getHorizontalScrollPosition(void) const;

        /*!
        \brief
            Sets the horizontal scroll position as a fraction of the
            complete scrollable width.

        \param position
            float value specifying the new scroll position.

        \return
            Nothing.
        */
        void setHorizontalScrollPosition(float position);

        /*!
        \brief
            Returns the vertical scrollbar step size as a fraction of one
            complete view page.

        \return
            float value specifying the step size where 1.0f would be the height of
            the viewing area.
        */
        float getVerticalStepSize(void) const;

        /*!
        \brief
            Sets the vertical scrollbar step size as a fraction of one
            complete view page.

        \param step
            float value specifying the step size, where 1.0f would be the height of
            the viewing area.

        \return
            Nothing.
        */
        void setVerticalStepSize(float step);

        /*!
        \brief
            Returns the vertical scrollbar overlap size as a fraction of one
            complete view page.

        \return
            float value specifying the overlap size where 1.0f would be the height of
            the viewing area.
        */
        float getVerticalOverlapSize(void) const;

        /*!
        \brief
            Sets the vertical scrollbar overlap size as a fraction of one
            complete view page.

        \param overlap
            float value specifying the overlap size, where 1.0f would be the height of
            the viewing area.

        \return
            Nothing.
        */
        void setVerticalOverlapSize(float overlap);

        /*!
        \brief
            Returns the vertical scroll position as a fraction of the
            complete scrollable height.

        \return
            float value specifying the scroll position.
        */
        float getVerticalScrollPosition(void) const;

        /*!
        \brief
            Sets the vertical scroll position as a fraction of the
            complete scrollable height.

        \param position
            float value specifying the new scroll position.

        \return
            Nothing.
        */
        void setVerticalScrollPosition(float position);

        /*************************************************************************
            Overridden from Window
        *************************************************************************/
        void initialise(void);

    protected:
        /*************************************************************************
        	Abstract interface
        *************************************************************************/
        /*!
        \brief
            Create a Scrollbar based widget to be used as the horizontal scrollbar.

        \param name
            String object holding the name that must be used when creating the widget.

        \return
            Scrollbar based object.
        */
        virtual Scrollbar* createHorizontalScrollbar(const String& name) const = 0;

        /*!
        \brief
            Create a Scrollbar based widget to be used as the vertical scrollbar.

        \param name
            String object holding the name that must be used when creating the widget.

        \return
            Scrollbar based object.
        */
        virtual Scrollbar* createVerticalScrollbar(const String& name) const = 0;

        /*!
        \brief
            Return a Rect that described the pane's viewable area, relative
            to this Window, in pixels.

        \return
            Rect object describing the ScrollablePane's viewable area.
        */
        virtual Rect getViewableArea(void) const = 0;

        /*************************************************************************
        	Implementation Methods
        *************************************************************************/
        /*!
        \brief
            Add ScrollablePane specific events
        */
        void addScrollablePaneEvents(bool bCommon=true);
	    void addUncommonEvents( void )							{ __super::addUncommonEvents(); addScrollablePaneEvents(false); }
    
        /*!
	    \brief
		    display required integrated scroll bars according to current size of
            the ScrollablePane view area and the size of the attached ScrolledContainer.
	    */
	    void configureScrollbars(void);

        /*!
        \brief
            Return whether the vertical scrollbar is needed.

        \return
            - true if the scrollbar is either needed or forced via setting.
            - false if the scrollbar should not be shown.
        */
        bool isVertScrollbarNeeded(void) const;

        /*!
        \brief
            Return whether the horizontal scrollbar is needed.

        \return
            - true if the scrollbar is either needed or forced via setting.
            - false if the scrollbar should not be shown.
        */
        bool isHorzScrollbarNeeded(void) const;

        /*!
        \brief
            Update the content container position according to the current 
            state of the widget (like scrollbar positions, etc).
        */
        void updateContainerPosition(void);


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
			if (class_name==(const utf8*)"ScrollablePane")	return true;
			return Window::testClassName_impl(class_name);
		}


        /*************************************************************************
        	Event triggers
        *************************************************************************/
        /*!
        \brief
            Event trigger method called when some pane content has changed size
            or location.

        \param e
            WindowEventArgs object.

        \return
            Nothing.
        */
        virtual void onContentPaneChanged(WindowEventArgs& e);

        /*!
        \brief
            Event trigger method called when the setting that controls whether the 
            vertical scrollbar is always shown or not, is changed.

        \param e
            WindowEventArgs object.

        \return
            Nothing.
        */
        virtual void onVertScrollbarModeChanged(WindowEventArgs& e);

        /*!
        \brief
            Event trigger method called when the setting that controls whether the 
            horizontal scrollbar is always shown or not, is changed.

        \param e
            WindowEventArgs object.

        \return
            Nothing.
        */
        virtual void onHorzScrollbarModeChanged(WindowEventArgs& e);

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

        /*!
        \brief
            Notification method called whenever the content pane is scrolled via
            changes in the scrollbar positions.

        \param e
            WindowEventArgs object.

        \return
            Nothing.
        */
        virtual void onContentPaneScrolled(WindowEventArgs& e);

        /*************************************************************************
        	Event handler methods
        *************************************************************************/
        /*!
        \brief
            Handler method which gets subscribed to the scrollbar position change
            events.
        */
        bool handleScrollChange(const EventArgs&  e);

        /*!
        \brief
            Handler method which gets subscribed to the ScrolledContainer content
            change events.
        */
        bool handleContentAreaChange(const EventArgs& e);

        /*!
        \brief
            Handler method which gets subscribed to the ScrolledContainer auto-size
            setting changes.
        */
        bool handleAutoSizePaneChanged(const EventArgs& e);

        /*************************************************************************
        	Overridden from Window
        *************************************************************************/
        void addChild_impl(Window* wnd);
        void removeChild_impl(Window* wnd);
        void onSized(WindowEventArgs& e);
        void onMouseWheel(MouseEventArgs& e);

        /*************************************************************************
        	Data fields
        *************************************************************************/
        bool    d_forceVertScroll;		//!< true if vertical scrollbar should always be displayed
        bool    d_forceHorzScroll;		//!< true if horizontal scrollbar should always be displayed
        Rect    d_contentRect;          //!< holds content area so we can track changes.
        float   d_vertStep;             //!< vertical scroll step fraction.
        float   d_vertOverlap;          //!< vertical scroll overlap fraction.
        float   d_horzStep;             //!< horizontal scroll step fraction.
        float   d_horzOverlap;          //!< horizontal scroll overlap fraction.

        // component widgets
        Scrollbar*  d_vertScrollbar;        //!< Scrollbar widget used for vertical scrolling.
        Scrollbar*  d_horzScrollbar;        //!< Scrollbar widget used for horizontal scrolling.
        ScrolledContainer*  d_container;    //!< ScrolledContainer widget holding the attached widgets.

    private:
	    /*************************************************************************
		    Static Properties for this class
	    *************************************************************************/
        static ScrollablePaneProperties::ForceHorzScrollbar     d_horzScrollbarProperty;
        static ScrollablePaneProperties::ForceVertScrollbar     d_vertScrollbarProperty;
	    static ScrollablePaneProperties::ContentPaneAutoSized	d_autoSizedProperty;
	    static ScrollablePaneProperties::ContentArea            d_contentAreaProperty;
        static ScrollablePaneProperties::HorzStepSize           d_horzStepProperty;
        static ScrollablePaneProperties::HorzOverlapSize        d_horzOverlapProperty;
        static ScrollablePaneProperties::HorzScrollPosition     d_horzScrollPositionProperty;
        static ScrollablePaneProperties::VertStepSize           d_vertStepProperty;
        static ScrollablePaneProperties::VertOverlapSize        d_vertOverlapProperty;
        static ScrollablePaneProperties::VertScrollPosition     d_vertScrollPositionProperty;

	    /*************************************************************************
		    Private methods
	    *************************************************************************/
	    void addScrollablePaneProperties( bool bCommon = true );
		void addUncommonProperties( void )							{ __super::addUncommonProperties(); addScrollablePaneProperties(false); }
    };

} // End of  CEGUI namespace section


#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIScrollablePane_h_
