/************************************************************************
    filename: 	CEGUISpinner.h
    created:	3/2/2005
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
#ifndef _CEGUISpinner_h_
#define _CEGUISpinner_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "elements/CEGUISpinnerProperties.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
    /*!
    \brief
        Base class for the Spinner widget.

        The spinner widget has a text area where numbers may be entered
        and two buttons which may be used to increase or decrease the
        value in the text area by a user specified amount.
    */
    class CEGUIEXPORT Spinner : public Window
    {
    public:
        /*!
        \brief
            Enumerated type specifying possible input and/or display modes for the spinner.
        */
        enum TextInputMode
        {
            FloatingPoint,  //!< Floating point decimal.
            Integer,        //!< Integer decimal.
            Hexadecimal,    //!< Hexadecimal.
            Octal           //!< Octal
        };

        /*************************************************************************
            Events system constants
        *************************************************************************/
        static const String EventNamespace;                 //!< Namespace for global events
        static const String EventValueChanged;              //!< Event fired when the spinner value changes.
        static const String EventStepChanged;               //!< Event fired when the step value changes.
        static const String EventMaximumValueChanged;       //!< Event fired when the maximum spinner value changes.
        static const String EventMinimumValueChanged;       //!< Event fired when the minimum spinner value changes.
        static const String EventTextInputModeChanged;      //!< Event fired when the input/display mode is changed.

        /*************************************************************************
            Object Construction and Destruction
        *************************************************************************/
        /*!
        \brief
            Constructor for Spinner objects
        */
        Spinner(const String& type, const String& name);

        /*!
        \brief
            Destructor for Spinner objects
        */
        virtual ~Spinner(void);

	    /*!
	    \brief
		    Initialises the Window based object ready for use.

	    \note
		    This must be called for every window created.  Normally this is handled automatically by the WindowFactory for each Window type.

	    \return
		    Nothing
	    */
        void initialise(void);


        /*************************************************************************
        	Accessors
        *************************************************************************/
        /*!
        \brief
            Return the current spinner value.

        \return
            current float value of the Spinner.
        */
        float getCurrentValue(void) const;

        /*!
        \brief
            Return the current step value.

        \return
            float step value.  This is the value added to the spinner vaue when the
            up / down buttons are clicked.
        */
        float getStepSize(void) const;

        /*!
        \brief
            Return the current maximum limit value for the Spinner.

        \return
            Maximum value that is allowed for the spinner.
        */
        float getMaximumValue(void) const;

        /*!
        \brief
            Return the current minimum limit value for the Spinner.

        \return
            Minimum value that is allowed for the spinner.
        */
        float getMinimumValue(void) const;

        /*!
        \brief
            Return the current text input / display mode setting.

        \return
            One of the TextInputMode enumerated values indicating the current
            text input and display mode.
        */
        TextInputMode getTextInputMode(void) const;

        /*************************************************************************
        	Manipulators
        *************************************************************************/
        /*!
        \brief
            Set the current spinner value.

        \param value
            value to be assigned to the Spinner.

        \return
            Nothing.
        */
        void setCurrentValue(float value);

        /*!
        \brief
            Set the current step value.

        \param step
            The value added to be the spinner value when the
            up / down buttons are clicked.

        \return
            Nothing.
        */
        void setStepSize(float step);

        /*!
        \brief
            Set the spinner maximum value.

        \param maxValue
            The maximum value to be allowed by the spinner.

        \return
            Nothing.
        */
        void setMaximumValue(float maxValue);

        /*!
        \brief
            Set the spinner minimum value.

        \param minVaue
            The minimum value to be allowed by the spinner.

        \return
            Nothing.
        */
        void setMinimumValue(float minVaue);

        /*!
        \brief
            Set the spinner input / display mode.

        \param mode
            One of the TextInputMode enumerated values indicating the text
            input / display mode to be used by the spinner.

        \return
            Nothing.
        */
        void setTextInputMode(TextInputMode mode);

    protected:
        /*************************************************************************
        	Constants
        *************************************************************************/
        static const String FloatValidator;      //!< Validator regex used for floating point mode.
        static const String IntegerValidator;    //!< Validator regex used for decimal integer mode.
        static const String HexValidator;        //!< Validator regex used for hexadecimal mode.
        static const String OctalValidator;      //!< Validator regex used for octal mode.

        /*************************************************************************
        	Protected Implementation Methods
        *************************************************************************/
        /*!
        \brief
            Adds events specific to the Spinner base class.
        
        \return
            Nothing.
        */
        void addSpinnerEvents(bool bCommon=true);
	    void addUncommonEvents( void )							{ __super::addUncommonEvents(); addSpinnerEvents(false); }

        /*!
        \brief
            Returns the numerical representation of the current editbox text.

        \return
            float value that is the numerical equivalent of the editbox text.

        \exception InvalidRequestException  thrown if the text can not be converted.
        */
        virtual float getValueFromText(void) const;

        /*!
        \brief
            Returns the textual representation of the current spinner value.

        \return
            String object that is equivalent to the the numerical value of the spinner.
        */
        virtual String getTextFromValue(void) const;


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
			if (class_name==(const utf8*)"Spinner")	return true;
			return Window::testClassName_impl(class_name);
		}


        /*************************************************************************
        	Abstract Implementation methods
        *************************************************************************/
        /*!
        \brief
            Creates a PushButton based widget that will be used for the increase
            button component of the Spinner widget.

        \return
            Pointer to a valid PushButton based object.
        */
        virtual PushButton* createIncreaseButton(const String& name) const = 0;

        /*!
        \brief
            Creates a PushButton based widget that will be used for the decrease
            button component of the Spinner widget.

        \return
            Pointer to a valid PushButton based object.
        */
        virtual PushButton* createDecreaseButton(const String& name) const = 0;

        /*!
        \brief
            Creates an Editbox based widget that will be used for the text input
            area of the spinner widget.

        \return
            Pointer to a valid Editbox based object.
        */
        virtual Editbox* createEditbox(const String& name) const = 0;

        /*************************************************************************
        	Overrides for Event handler methods
        *************************************************************************/
        virtual	void onFontChanged(WindowEventArgs& e);
        virtual void onTextChanged(WindowEventArgs& e);
        virtual void onActivated(ActivationEventArgs& e);

        /*************************************************************************
        	New Event handler methods
        *************************************************************************/
        /*!
        \brief
            Method called when the spinner value changes.

        \param e
            WindowEventArgs object containing any relevant data.

        \return
            Nothing.
        */
        virtual void onValueChanged(WindowEventArgs& e);

        /*!
        \brief
            Method called when the step value changes.

        \param e
            WindowEventArgs object containing any relevant data.

        \return
            Nothing.
        */
        virtual void onStepChanged(WindowEventArgs& e);

        /*!
        \brief
            Method called when the maximum value setting changes.

        \param e
            WindowEventArgs object containing any relevant data.

        \return
            Nothing.
        */
        virtual void onMaximumValueChanged(WindowEventArgs& e);

        /*!
        \brief
            Method called when the minimum value setting changes.

        \param e
            WindowEventArgs object containing any relevant data.

        \return
            Nothing.
        */
        virtual void onMinimumValueChanged(WindowEventArgs& e);

        /*!
        \brief
            Method called when the text input/display mode is changed.

        \param e
            WindowEventArgs object containing any relevant data.

        \return
            Nothing.
        */
        virtual void onTextInputModeChanged(WindowEventArgs& e);

        /*************************************************************************
        	Internal event listener methods
        *************************************************************************/
        bool handleIncreaseButton(const EventArgs& e);
        bool handleDecreaseButton(const EventArgs& e);
        bool handleEditTextChange(const EventArgs& e);


        /*************************************************************************
        	Data Fields
        *************************************************************************/
        PushButton* d_increaseButton;   //!< Pointer to the increase button widget.
        PushButton* d_decreaseButton;   //!< Pointer to the decrease button widget.
        Editbox*    d_editbox;          //!< Pointer to the editbox widget.

        float   d_stepSize;     //!< Step size value used y the increase & decrease buttons.
        float   d_currentValue; //!< Numerical copy of the text in d_editbox.
        float   d_maxValue;     //!< Maximum value for spinner.
        float   d_minValue;     //!< Minimum value for spinner.
        TextInputMode   d_inputMode;    //!< Current text display/input mode.

    private:
        /*************************************************************************
        	Static properties for the Spinner widget
        *************************************************************************/
        static SpinnerProperties::CurrentValue  d_currentValueProperty;
        static SpinnerProperties::StepSize      d_stepSizeProperty;
        static SpinnerProperties::MaximumValue  d_maxValueProperty;
        static SpinnerProperties::MinimumValue  d_minValueProperty;
        static SpinnerProperties::TextInputMode d_textInputModeProperty;

        /*************************************************************************
        	Private Implementation Methods
        *************************************************************************/
        /*!
        \brief
            Adds properties supported by the Spinner class.

        \return
            Nothing.
        */
        void    addSpinnerProperties( bool bCommon = true );
	    void	addUncommonProperties( void )							{ __super::addUncommonProperties(); addSpinnerProperties(false); }
    };

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUISpinner_h_
