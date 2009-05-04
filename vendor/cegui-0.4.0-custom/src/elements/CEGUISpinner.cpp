/************************************************************************
	filename: 	CEGUISpinner.cpp
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
#include "StdInc.h"
#include "elements/CEGUISpinner.h"
#include "elements/CEGUIPushButton.h"
#include "elements/CEGUIEditbox.h"
#include "CEGUIExceptions.h"
#include <stdio.h>
#include <sstream>
#include <iomanip>

// Start of CEGUI namespace section
namespace CEGUI
{
    //////////////////////////////////////////////////////////////////////////
    // event strings
    const String Spinner::EventNamespace("Spinner");
    const String Spinner::EventValueChanged("ValueChanged");
    const String Spinner::EventStepChanged("StepChanged");
    const String Spinner::EventMaximumValueChanged("MaximumValueChanged");
    const String Spinner::EventMinimumValueChanged("MinimumValueChanged");
    const String Spinner::EventTextInputModeChanged("TextInputModeChanged");
    // Validator strings
    const String Spinner::FloatValidator("-?\\d*\\.?\\d*");
    const String Spinner::IntegerValidator("-?\\d*");
    const String Spinner::HexValidator("[0-9a-fA-F]*");
    const String Spinner::OctalValidator("[0-7]*");
    // properties
    SpinnerProperties::CurrentValue  Spinner::d_currentValueProperty;
    SpinnerProperties::StepSize      Spinner::d_stepSizeProperty;
    SpinnerProperties::MaximumValue  Spinner::d_maxValueProperty;
    SpinnerProperties::MinimumValue  Spinner::d_minValueProperty;
    SpinnerProperties::TextInputMode Spinner::d_textInputModeProperty;
    //////////////////////////////////////////////////////////////////////////
    
    Spinner::Spinner(const String& type, const String& name) :
        Window(type, name),
        d_increaseButton(0),
        d_decreaseButton(0),
        d_editbox(0),
        d_stepSize(1.0f),
        d_currentValue(1.0f),
        d_maxValue(32767.0f),
        d_minValue(-32768.0f),
        d_inputMode((TextInputMode)-1)
    {
        addSpinnerEvents();
        addSpinnerProperties();
    }

    Spinner::~Spinner(void)
    {
        // Nothing to do here.
    }

    void Spinner::initialise(void)
    {
        Window::initialise();

        // create all the component widgets
        d_increaseButton = createIncreaseButton(getName() + "__auto_incbtn__");
        addChildWindow(d_increaseButton);
        d_decreaseButton = createDecreaseButton(getName() + "__auto_decbtn__");
        addChildWindow(d_decreaseButton);
        d_editbox = createEditbox(getName() + "__auto_editbox__");
        addChildWindow(d_editbox);

        // setup component controls
        d_increaseButton->setWantsMultiClickEvents(false);
        d_increaseButton->setMouseAutoRepeatEnabled(true);
        d_decreaseButton->setWantsMultiClickEvents(false);
        d_decreaseButton->setMouseAutoRepeatEnabled(true);

        // perform event subscriptions.
        d_increaseButton->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&Spinner::handleIncreaseButton, this));
        d_decreaseButton->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&Spinner::handleDecreaseButton, this));
        d_editbox->subscribeEvent(Window::EventTextChanged, Event::Subscriber(&Spinner::handleEditTextChange, this));

        // final initialisation
        setTextInputMode(Integer);
        setCurrentValue(0.0f);
        performChildWindowLayout();
    }

    float Spinner::getCurrentValue(void) const
    {
        return d_currentValue;
    }

    float Spinner::getStepSize(void) const
    {
        return d_stepSize;
    }

    float Spinner::getMaximumValue(void) const
    {
        return d_maxValue;
    }

    float Spinner::getMinimumValue(void) const
    {
        return d_minValue;
    }

    Spinner::TextInputMode Spinner::getTextInputMode(void) const
    {
        return d_inputMode;
    }

    void Spinner::setCurrentValue(float value)
    {
        if (value != d_currentValue)
        {
            // limit input value to within valid range for spinner
            value = ceguimax(ceguimin(value, d_maxValue), d_minValue);

            d_currentValue = value;

            WindowEventArgs args(this);
            onValueChanged(args);
        }
    }

    void Spinner::setStepSize(float step)
    {
        if (step != d_stepSize)
        {
            d_stepSize = step;
            
            WindowEventArgs args(this);
            onStepChanged(args);
        }
    }

    void Spinner::setMaximumValue(float maxValue)
    {
        if (maxValue != d_maxValue)
        {
            d_maxValue = maxValue;

            WindowEventArgs args(this);
            onMaximumValueChanged(args);
        }
    }

    void Spinner::setMinimumValue(float minVaue)
    {
        if (minVaue != d_minValue)
        {
            d_minValue = minVaue;

            WindowEventArgs args(this);
            onMinimumValueChanged(args);
        }
    }

    void Spinner::setTextInputMode(TextInputMode mode)
    {
        if (mode != d_inputMode)
        {
            switch (mode)
            {
            case FloatingPoint:
                d_editbox->setValidationString(FloatValidator);
                break;
            case Integer:
                d_editbox->setValidationString(IntegerValidator);
                break;
            case Hexadecimal:
                d_editbox->setValidationString(HexValidator);
                break;
            case Octal:
                d_editbox->setValidationString(OctalValidator);
                break;
            default:
                throw InvalidRequestException("Spinner::setTextInputMode - An unknown TextInputMode was specified.");
            }

            d_inputMode = mode;

            WindowEventArgs args(this);
            onTextInputModeChanged(args);
        }
    }

    void Spinner::addSpinnerEvents(bool bCommon)
    {
        if ( bCommon == false )	addEvent(EventValueChanged);
        if ( bCommon == false )	addEvent(EventStepChanged);
        if ( bCommon == false )	addEvent(EventMaximumValueChanged);
        if ( bCommon == false )	addEvent(EventMinimumValueChanged);
        if ( bCommon == false )	addEvent(EventTextInputModeChanged);
    }

    void Spinner::addSpinnerProperties( bool bCommon )
    {
        if ( bCommon == false )   addProperty(&d_currentValueProperty);
        if ( bCommon == false )   addProperty(&d_stepSizeProperty);
        if ( bCommon == false )   addProperty(&d_maxValueProperty);
        if ( bCommon == false )   addProperty(&d_minValueProperty);
        if ( bCommon == false )   addProperty(&d_textInputModeProperty);
    }

    float Spinner::getValueFromText(void) const
    {
        // handle empty case
        if (d_editbox->getText().empty())
        {
            return 0.0f;
        }

        int res, tmp;
        uint utmp;
        float val;

        switch (d_inputMode)
        {
        case FloatingPoint:
            res = sscanf(d_editbox->getText().c_str(), "%f", &val);
            break;
        case Integer:
            res = sscanf(d_editbox->getText().c_str(), "%d", &tmp);
            val = static_cast<float>(tmp);
            break;
        case Hexadecimal:
            res = sscanf(d_editbox->getText().c_str(), "%x", &utmp);
            val = static_cast<float>(utmp);
            break;
        case Octal:
            res = sscanf(d_editbox->getText().c_str(), "%o", &utmp);
            val = static_cast<float>(utmp);
            break;
        default:
            throw InvalidRequestException("Spinner::getValueFromText - An unknown TextInputMode was encountered.");
        }

        if (res)
        {
            return val;            
        }

        throw InvalidRequestException("Spinner::getValueFromText - The string '" + d_editbox->getText() + "' ca not be converted to numerical representation.");
    }

    String Spinner::getTextFromValue(void) const
    {
        std::stringstream tmp;

        switch (d_inputMode)
        {
        case FloatingPoint:
            tmp << d_currentValue;
            break;
        case Integer:
            tmp << static_cast<int>(d_currentValue);
            break;
        case Hexadecimal:
            tmp << std::hex << std::uppercase << static_cast<int>(d_currentValue);
            break;
        case Octal:
            tmp << std::oct << static_cast<int>(d_currentValue);
            break;
        default:
            throw InvalidRequestException("Spinner::getValueFromText - An unknown TextInputMode was encountered.");
        }

        return String(tmp.str());
    }

    void Spinner::onFontChanged(WindowEventArgs& e)
    {
        // Propagate to children
        d_editbox->setFont(getFont());
        // Call base class handler
        Window::onFontChanged(e);
    }

    void Spinner::onTextChanged(WindowEventArgs& e)
    {
        // update only if needed
        if (d_editbox->getText() != d_text)
        {
            // done before doing base class processing so event subscribers see
            // 'updated' version.
            d_editbox->setText(d_text);
            e.handled = true;

            Window::onTextChanged(e);
        }
    }

    void Spinner::onActivated(ActivationEventArgs& e)
    {
        if (!isActive())
        {
            Window::onActivated(e);

            if (!d_editbox->isActive())
            {
                d_editbox->activate();
            }
        }
    }

    void Spinner::onValueChanged(WindowEventArgs& e)
    {
        // mute to save doing unnecessary events work.
        bool wasMuted = d_editbox->isMuted();
        d_editbox->setMutedState(true);

        // Update text with new value.
        // (allow special 'empty' case to equal 0 with no text change required)
        if (!(d_currentValue == 0 && d_editbox->getText().empty()))
        {
            d_editbox->setText(getTextFromValue());
        }
        // restore previous mute state.
        d_editbox->setMutedState(wasMuted);

        fireEvent(EventValueChanged, e, EventNamespace);
    }

    void Spinner::onStepChanged(WindowEventArgs& e)
    {
        fireEvent(EventStepChanged, e, EventNamespace);
    }

    void Spinner::onMaximumValueChanged(WindowEventArgs& e)
    {
        fireEvent(EventMaximumValueChanged, e, EventNamespace);

        if (d_currentValue > d_maxValue)
        {
            setCurrentValue(d_maxValue);
        }
    }

    void Spinner::onMinimumValueChanged(WindowEventArgs& e)
    {
        fireEvent(EventMinimumValueChanged, e, EventNamespace);

        if (d_currentValue < d_minValue)
        {
            setCurrentValue(d_minValue);
        }
    }

    void Spinner::onTextInputModeChanged(WindowEventArgs& e)
    {
        // update edit box text to reflect new mode.
        // mute to save doing unnecessary events work.
        bool wasMuted = d_editbox->isMuted();
        d_editbox->setMutedState(true);
        // Update text with new value.
        d_editbox->setText(getTextFromValue());
        // restore previous mute state.
        d_editbox->setMutedState(wasMuted);

        fireEvent(EventTextInputModeChanged, e, EventNamespace);
    }

    bool Spinner::handleIncreaseButton(const EventArgs& e)
    {
        if (((const MouseEventArgs&)e).button == LeftButton)
        {
            setCurrentValue(d_currentValue + d_stepSize);
            return true;
        }
        
        return false;
    }

    bool Spinner::handleDecreaseButton(const EventArgs& e)
    {
        if (((const MouseEventArgs&)e).button == LeftButton)
        {
            setCurrentValue(d_currentValue - d_stepSize);
            return true;
        }

        return false;
    }

    bool Spinner::handleEditTextChange(const EventArgs& e)
    {
        // set this windows text to match
        setText(d_editbox->getText());
        // update value
        setCurrentValue(getValueFromText());
        return true;
    }

} // End of  CEGUI namespace section
