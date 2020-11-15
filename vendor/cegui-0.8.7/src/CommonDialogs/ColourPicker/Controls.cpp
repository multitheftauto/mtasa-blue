/***********************************************************************
    created:    20th February 2010
    author:     Lukas E Meindl

    purpose:    Implementation of ColourPickerControls base class
*************************************************************************/
/***************************************************************************
*   Copyright (C) 2004 - 2011 Paul D Turner & The CEGUI Development Team
*
*   Permission is hereby granted, free of charge, to any person obtaining
*   a copy of this software and associated documentation files (the
*   "Software"), to deal in the Software without restriction, including
*   without limitation the rights to use, copy, modify, merge, publish,
*   distribute, sublicense, and/or sell copies of the Software, and to
*   permit persons to whom the Software is furnished to do so, subject to
*   the following conditions:
*
*   The above copyright notice and this permission notice shall be
*   included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
*   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
*   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
*   OTHER DEALINGS IN THE SOFTWARE.
***************************************************************************/
#include "CEGUI/widgets/PushButton.h"
#include "CEGUI/widgets/Editbox.h"
#include "CEGUI/widgets/RadioButton.h"
#include "CEGUI/widgets/Titlebar.h"
#include "CEGUI/widgets/Slider.h"
#include "CEGUI/widgets/Thumb.h"

#include "CEGUI/MouseCursor.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/Exceptions.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/BasicImage.h"
#include "CEGUI/CoordConverter.h"

#include "CEGUI/CommonDialogs/ColourPicker/ColourPicker.h"
#include "CEGUI/CommonDialogs/ColourPicker/Controls.h"
#include "CEGUI/CommonDialogs/ColourPicker/Conversions.h"

#include "CEGUI/TextureTarget.h"
#include "CEGUI/Texture.h"
#include "CEGUI/PropertyHelper.h"

#include "CEGUI/RegexMatcher.h"

#include <sstream>
#include <algorithm>
#include <math.h>

namespace CEGUI
{
//----------------------------------------------------------------------------//
const String ColourPickerControls::EventNamespace("ColourPickerControls");
const String ColourPickerControls::WidgetTypeName("CEGUI/ColourPickerControls");
const String ColourPickerControls::EventColourAccepted("ColourAccepted");
const String ColourPickerControls::EventClosed("Closed");
const String ColourPickerControls::ColourRectPropertyName("Colour");
//----------------------------------------------------------------------------//
const String ColourPickerControls::ColourPickerControlsPickingTextureImageName("ColourPickingTexture");
const String ColourPickerControls::ColourPickerControlsColourSliderTextureImageName("ColourSliderTexture");
const String ColourPickerControls::ColourPickerControlsAlphaSliderTextureImageName("AlphaSliderTexture");
//----------------------------------------------------------------------------//
const float ColourPickerControls::LAB_L_MIN(0.0f);
const float ColourPickerControls::LAB_L_MAX(100.0f);
const float ColourPickerControls::LAB_L_DIFF(LAB_L_MAX - LAB_L_MIN);
const float ColourPickerControls::LAB_A_MIN(-130.0f);
const float ColourPickerControls::LAB_A_MAX(130.0f);
const float ColourPickerControls::LAB_A_DIFF(LAB_A_MAX - LAB_A_MIN);
const float ColourPickerControls::LAB_B_MIN(-130.0f);
const float ColourPickerControls::LAB_B_MAX(130.0f);
const float ColourPickerControls::LAB_B_DIFF(LAB_B_MAX - LAB_B_MIN);
//----------------------------------------------------------------------------//
// Child Widget name constants
const String ColourPickerControls::CancelButtonName("__auto_cancelbutton__");
const String ColourPickerControls::AcceptButtonName("__auto_acceptbutton__");
const String ColourPickerControls::HexaDecimalDescriptionName("__auto_hexadecimalstatictext__");
const String ColourPickerControls::HexaDecimalEditBoxName("__auto_hexadecimaleditbox__");
const String ColourPickerControls::TitleBarName("__auto_titlebar__");

const String ColourPickerControls::ColourPickerStaticImageName("__auto_colourpickerstaticimage__");
const String ColourPickerControls::ColourPickerImageSliderName("__auto_colourpickerimageslider__");
const String ColourPickerControls::ColourPickerAlphaSliderName("__auto_colourpickeralphaslider__");

const String ColourPickerControls::NewColourDescriptionName("__auto_newcolourdescription__");
const String ColourPickerControls::OldColourDescriptionName("__auto_oldcolourdescription__");

const String ColourPickerControls::NewColourRectName("__auto_newcolourrect__");
const String ColourPickerControls::OldColourRectName("__auto_oldcolourrect__");

const String ColourPickerControls::ColourEditBoxRDescriptionName("__auto_coloureditboxRdescription__");
const String ColourPickerControls::ColourEditBoxRName("__auto_coloureditboxR__");
const String ColourPickerControls::ColourEditBoxGDescriptionName("__auto_coloureditboxGdescription__");
const String ColourPickerControls::ColourEditBoxGName("__auto_coloureditboxG__");
const String ColourPickerControls::ColourEditBoxBDescriptionName("__auto_coloureditboxBdescription__");
const String ColourPickerControls::ColourEditBoxBName("__auto_coloureditboxB__");

const String ColourPickerControls::HSVRadioButtonHName("__auto_HSVradiobuttonH__");
const String ColourPickerControls::HSVEditBoxHName("__auto_HSVeditboxH__");
const String ColourPickerControls::HSVRadioButtonSName("__auto_HSVradiobuttonS__");
const String ColourPickerControls::HSVEditBoxSName("__auto_HSVeditboxS__");
const String ColourPickerControls::HSVRadioButtonVName("__auto_HSVradiobuttonV__");
const String ColourPickerControls::HSVEditBoxVName("__auto_HSVeditboxV__");

const String ColourPickerControls::LabRadioButtonLName("__auto_LabradiobuttonL__");
const String ColourPickerControls::LabEditBoxLName("__auto_LabeditboxL__");
const String ColourPickerControls::LabRadioButtonAName("__auto_Labradiobuttona__");
const String ColourPickerControls::LabEditBoxAName("__auto_Labeditboxa__");
const String ColourPickerControls::LabRadioButtonBName("__auto_Labradiobuttonb__");
const String ColourPickerControls::LabEditBoxBName("__auto_Labeditboxb__");

const String ColourPickerControls::AlphaEditBoxName("__auto_alphaeditbox__");

const String ColourPickerControls::ColourPickerCursorName("__colourpickercursor__");

//----------------------------------------------------------------------------//
ColourPickerControls::ColourPickerControls(const String& type, const String& name) :
    Window(type, name),
    d_callingColourPicker(0),
    d_colourPickerCursor(0),
    d_sliderMode(SliderMode_Lab_L),
    d_selectedColour(0.75f, 0.75f, 0.75f),
    d_colourPickerControlsTextureTarget(0),
    d_colourPickerImageOffset(2),
    d_colourPickerPickingImageHeight(260),
    d_colourPickerPickingImageWidth(260),
    d_colourPickerColourSliderImageWidth(1),
    d_colourPickerColourSliderImageHeight(260),
    d_colourPickerAlphaSliderImageWidth(260),
    d_colourPickerAlphaSliderImageHeight(60),
    d_colourPickerControlsTextureSize(512),
    d_draggingColourPickerCursor(false),
    d_colourPickingTexture(new RGB_Colour[d_colourPickerControlsTextureSize *
                                          d_colourPickerControlsTextureSize]),
    d_ignoreEvents(false),
    d_regexMatcher(*System::getSingleton().createRegexMatcher())
{
}

//----------------------------------------------------------------------------//
ColourPickerControls::~ColourPickerControls()
{
    deinitColourPickerControlsTexture();

    if (d_colourPickerCursor)
        WindowManager::getSingleton().destroyWindow(d_colourPickerCursor);

    delete[] d_colourPickingTexture;

    System::getSingleton().destroyRegexMatcher(&d_regexMatcher);
}


//----------------------------------------------------------------------------//
Lab_Colour ColourPickerControls::getColourPickingPositionColourLAB(float xAbs,
                                                                   float yAbs)
{
    float L = 0.0f;
    float a = 0.0f;
    float b = 0.0f;

    float xRel = xAbs / (float)(d_colourPickerPickingImageWidth - 1);
    float yRel = yAbs / (float)(d_colourPickerPickingImageHeight - 1);

    if (d_sliderMode != SliderMode_Lab_L)
    {
        L = LAB_L_MAX - LAB_L_DIFF * yRel ;

        if (d_sliderMode != SliderMode_Lab_A)
        {
            b = d_selectedColourLAB.b;
            a = LAB_A_MAX - LAB_A_DIFF * xRel;
        }
        else
        {
            a = d_selectedColourLAB.a;
            b = LAB_B_MAX - LAB_B_DIFF * xRel;
        }
    }
    else
    {
        L = d_selectedColourLAB.L;
        a = LAB_A_MAX - LAB_A_DIFF * xRel;
        b = LAB_B_MAX - LAB_B_DIFF * yRel;
    }

    return Lab_Colour(L, a, b);
}

//----------------------------------------------------------------------------//
HSV_Colour ColourPickerControls::getColourPickingPositionColourHSV(float xAbs,
                                                                   float yAbs)
{
    HSV_Colour colour;

    float xRel = xAbs / (float)(d_colourPickerPickingImageWidth - 1);
    float yRel = yAbs / (float)(d_colourPickerPickingImageHeight - 1);

    if (d_sliderMode != SliderMode_HSV_H)
    {
        float xCoord = (xRel - 0.5f) * 2.0f;
        float yCoord = (yRel - 0.5f) * 2.0f;

        float angle = std::atan2(yCoord, xCoord);

        if (angle < 0.0f)
            angle += 2.0f * 3.1415926535897932384626433832795f;

        angle /= 2.0f * 3.1415926535897932384626433832795f;
        colour.H = angle;

        float length = std::sqrt(xCoord * xCoord + yCoord * yCoord);
        float value = ceguimin(length, 1.0f);

        if (d_sliderMode != SliderMode_HSV_S)
        {
            colour.V = d_selectedColourHSV.V;
            colour.S = value;
        }
        else
        {
            colour.S = d_selectedColourHSV.S;
            colour.V = value;
        }
    }
    else
    {
        colour.H = d_selectedColourHSV.H;
        colour.S = xRel;
        colour.V = 1.0f - yRel;
    }

    return colour;
}

//----------------------------------------------------------------------------//
Vector2f ColourPickerControls::getColourPickingColourPosition()
{
    float x = 0.0f;
    float y = 0.0f;

    switch (d_sliderMode)
    {
    case SliderMode_Lab_L:
        x = 1.0f - (d_selectedColourLAB.a - LAB_A_MIN) / LAB_A_DIFF;
        y = 1.0f - (d_selectedColourLAB.b - LAB_B_MIN) / LAB_B_DIFF;
        break;

    case SliderMode_Lab_A:
        x = 1.0f - (d_selectedColourLAB.b - LAB_B_MIN) / LAB_B_DIFF;
        y = 1.0f - (d_selectedColourLAB.L - LAB_L_MIN) / LAB_L_DIFF;
        break;

    case SliderMode_Lab_B:
        x = 1.0f - (d_selectedColourLAB.a - LAB_A_MIN) / LAB_A_DIFF;
        y = 1.0f - (d_selectedColourLAB.L - LAB_L_MIN) / LAB_L_DIFF;
        break;

    case SliderMode_HSV_H:
        x = d_selectedColourHSV.S;
        y = 1.0f - d_selectedColourHSV.V;
        break;

    case SliderMode_HSV_S:
        getColourPickingColourPositionHSV(x, y);
        break;

    case SliderMode_HSV_V:
        getColourPickingColourPositionHSV(x, y);
        break;

    default:
        break;
    }

    return Vector2f(x, y);
}

//----------------------------------------------------------------------------//
void ColourPickerControls::getColourPickingColourPositionHSV(float& x, float& y)
{
    float radius;

    switch (d_sliderMode)
    {
    case SliderMode_HSV_S:
        radius = d_selectedColourHSV.V;
        break;

    case SliderMode_HSV_V:
        radius = d_selectedColourHSV.S;
        break;

    default:
        radius = 0.0f; //This shouldn't happen.  (PDT: Why is it here, then?)
        break;
    }

    float angle = d_selectedColourHSV.H;
    angle *= 2.0f * 3.1415926535897932384626433832795f;

    x = radius * cos(angle) * 0.5f + 0.5f;
    y = radius * sin(angle) * 0.5f + 0.5f;
}

//----------------------------------------------------------------------------//
Lab_Colour ColourPickerControls::getColourSliderPositionColourLAB(float value)
{
    Lab_Colour colour = d_selectedColourLAB;

    switch (d_sliderMode)
    {
    case SliderMode_Lab_L:
        colour.L = LAB_L_MAX - LAB_L_DIFF * value;
        break;

    case SliderMode_Lab_A:
        colour.a = LAB_A_MAX - LAB_A_DIFF * value;
        break;

    case SliderMode_Lab_B:
        colour.b = LAB_B_MAX - LAB_B_DIFF * value;
        break;

    default:
        break;
    }

    return colour;
}

//----------------------------------------------------------------------------//
HSV_Colour ColourPickerControls::getColourSliderPositionColourHSV(float value)
{
    HSV_Colour colour = d_selectedColourHSV;

    switch (d_sliderMode)
    {
    case SliderMode_HSV_H:
        colour.H = 1.0f - value;
        break;

    case SliderMode_HSV_S:
        colour.S = 1.0f - value;
        break;

    case SliderMode_HSV_V:
        colour.V = 1.0f - value;
        break;

    default:
        break;
    }

    return colour;
}

//----------------------------------------------------------------------------//
void ColourPickerControls::initColourPickerControlsImageSet()
{
    d_colourPickerControlsTextureTarget =
        System::getSingleton().getRenderer()->createTextureTarget();

    const String baseName(
        d_colourPickerControlsTextureTarget->getTexture().getName());

    BasicImage* image = static_cast<BasicImage*>(
        &ImageManager::getSingleton().create("BasicImage", baseName + '/' +
            ColourPickerControlsPickingTextureImageName));

    image->setTexture(&d_colourPickerControlsTextureTarget->getTexture());
    image->setArea(
        Rectf(Vector2f(0.0f, 0.0f),
              Sizef(static_cast<float>(d_colourPickerPickingImageWidth),
                    static_cast<float>(d_colourPickerPickingImageHeight))));

    image = static_cast<BasicImage*>(
                &ImageManager::getSingleton().create("BasicImage", baseName + '/' +
                        ColourPickerControlsColourSliderTextureImageName));

    image->setTexture(&d_colourPickerControlsTextureTarget->getTexture());
    image->setArea(
        Rectf(Vector2f(static_cast<float>(d_colourPickerPickingImageWidth + d_colourPickerImageOffset), 0.0f),
              Sizef(static_cast<float>(d_colourPickerColourSliderImageWidth),
                    static_cast<float>(d_colourPickerColourSliderImageHeight))));

    image = static_cast<BasicImage*>(
                &ImageManager::getSingleton().create("BasicImage", baseName + '/' +
                        ColourPickerControlsAlphaSliderTextureImageName));

    image->setTexture(&d_colourPickerControlsTextureTarget->getTexture());
    image->setArea(
        Rectf(Vector2f(0.0f, static_cast<float>(d_colourPickerPickingImageHeight + d_colourPickerImageOffset)),
              Sizef(static_cast<float>(d_colourPickerAlphaSliderImageWidth),
                    static_cast<float>(d_colourPickerAlphaSliderImageHeight))));

    getColourPickerStaticImage()->setProperty(
        "Image", baseName + '/' + ColourPickerControlsPickingTextureImageName);
    getColourPickerImageSlider()->setProperty(
        "ScrollImage", baseName + '/' + ColourPickerControlsColourSliderTextureImageName);
    getColourPickerAlphaSlider()->setProperty(
        "ScrollImage", baseName + '/' + ColourPickerControlsAlphaSliderTextureImageName);

    refreshColourPickerControlsTextures();
}

//----------------------------------------------------------------------------//
void ColourPickerControls::deinitColourPickerControlsTexture()
{
    if (d_colourPickerControlsTextureTarget)
    {
        ImageManager::getSingleton().destroyImageCollection(
            d_colourPickerControlsTextureTarget->getTexture().getName(), false);

        System::getSingleton().getRenderer()->
        destroyTextureTarget(d_colourPickerControlsTextureTarget);
    }
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshColourPickerControlsTextures()
{
    Logger::getSingleton().logEvent(
        "[ColourPicker] Refreshing ColourPickerTexture");

    refreshColourPickingImage();
    refreshColourSliderImage();
    refreshAlphaSliderImage();

    reloadColourPickerControlsTexture();
}

//----------------------------------------------------------------------------//
void ColourPickerControls::reloadColourPickerControlsTexture()
{
    d_colourPickerControlsTextureTarget->getTexture().loadFromMemory(
        d_colourPickingTexture,
        Sizef(static_cast<float>(d_colourPickerControlsTextureSize),
              static_cast<float>(d_colourPickerControlsTextureSize)),
        Texture::PF_RGB);

    getColourPickerImageSlider()->invalidate();
    getColourPickerAlphaSlider()->invalidate();
    getColourPickerStaticImage()->invalidate();
}

//----------------------------------------------------------------------------//
void ColourPickerControls::initialiseComponents()
{
    getCancelButton()->subscribeEvent(
        PushButton::EventClicked,
        Event::Subscriber(&ColourPickerControls::handleCancelButtonClicked, this));

    getAcceptButton()->subscribeEvent(
        PushButton::EventClicked,
        Event::Subscriber(&ColourPickerControls::handleAcceptButtonClicked, this));

    getHexadecimalEditbox()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getColourEditBoxR()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getColourEditBoxG()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getColourEditBoxB()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getHSVEditBoxH()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getHSVEditBoxS()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getHSVEditBoxV()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getLabEditBoxL()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getLabEditBoxA()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getLabEditBoxB()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getAlphaEditBox()->subscribeEvent(
        Editbox::EventDeactivated,
        Event::Subscriber(&ColourPickerControls::handleEditboxDeactivated, this));

    getHSVRadioButtonH()->subscribeEvent(
        RadioButton::EventSelectStateChanged,
        Event::Subscriber(&ColourPickerControls::handleRadioButtonModeSelection, this));

    getHSVRadioButtonS()->subscribeEvent(
        RadioButton::EventSelectStateChanged,
        Event::Subscriber(&ColourPickerControls::handleRadioButtonModeSelection, this));

    getHSVRadioButtonV()->subscribeEvent(
        RadioButton::EventSelectStateChanged,
        Event::Subscriber(&ColourPickerControls::handleRadioButtonModeSelection, this));

    getLabRadioButtonL()->subscribeEvent(
        RadioButton::EventSelectStateChanged,
        Event::Subscriber(&ColourPickerControls::handleRadioButtonModeSelection, this));

    getLabRadioButtonA()->subscribeEvent(
        RadioButton::EventSelectStateChanged,
        Event::Subscriber(&ColourPickerControls::handleRadioButtonModeSelection, this));

    getLabRadioButtonB()->subscribeEvent(
        RadioButton::EventSelectStateChanged,
        Event::Subscriber(&ColourPickerControls::handleRadioButtonModeSelection, this));

    getHexadecimalEditbox()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleHexadecimalEditboxTextChanged, this));

    getColourEditBoxR()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleRGBEditboxTextChanged, this));

    getColourEditBoxG()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleRGBEditboxTextChanged, this));

    getColourEditBoxB()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleRGBEditboxTextChanged, this));

    getLabEditBoxL()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleLABEditboxTextChanged, this));

    getLabEditBoxA()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleLABEditboxTextChanged, this));

    getLabEditBoxB()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleLABEditboxTextChanged, this));

    getHSVEditBoxH()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleHSVEditboxTextChanged, this));

    getHSVEditBoxS()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleHSVEditboxTextChanged, this));

    getHSVEditBoxV()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleHSVEditboxTextChanged, this));

    getAlphaEditBox()->subscribeEvent(
        Editbox::EventTextChanged,
        Event::Subscriber(&ColourPickerControls::handleAlphaEditboxTextChanged, this));

    getColourPickerImageSlider()->subscribeEvent(
        Slider::EventValueChanged,
        Event::Subscriber(&ColourPickerControls::handleColourPickerSliderValueChanged, this));

    getColourPickerAlphaSlider()->subscribeEvent(
        Slider::EventValueChanged,
        Event::Subscriber(&ColourPickerControls::handleAlphaSliderValueChanged, this));

    getColourPickerStaticImage()->subscribeEvent(
        Window::EventMouseLeavesSurface,
        Event::Subscriber(&ColourPickerControls::handleColourPickerStaticImageMouseLeaves, this));

    getColourPickerStaticImage()->subscribeEvent(
        Window::EventMouseButtonUp,
        Event::Subscriber(&ColourPickerControls::handleColourPickerStaticImageMouseButtonUp, this));

    getColourPickerStaticImage()->subscribeEvent(
        Window::EventMouseButtonDown,
        Event::Subscriber(&ColourPickerControls::handleColourPickerStaticImageMouseButtonDown, this));

    getColourPickerStaticImage()->subscribeEvent(
        Window::EventMouseMove,
        Event::Subscriber(&ColourPickerControls::handleColourPickerStaticImageMouseMove, this));

    initColourPicker();

    performChildWindowLayout();
}

//----------------------------------------------------------------------------//
void ColourPickerControls::setPreviousColour(const Colour& previousColour)
{
    d_previouslySelectedColour = previousColour;
}

//----------------------------------------------------------------------------//
void ColourPickerControls::setColours(const Colour& newColour)
{
    d_selectedColourRGB = RGB_Colour(newColour);
    d_selectedColourLAB = Lab_Colour(d_selectedColourRGB);
    d_selectedColourHSV = HSV_Colour(d_selectedColourRGB);
    d_selectedColour = newColour;
}

//----------------------------------------------------------------------------//
void ColourPickerControls::setColours(const Lab_Colour& newColourLAB)
{
    d_selectedColourLAB = newColourLAB;
    d_selectedColourRGB = RGB_Colour(newColourLAB);
    d_selectedColourHSV = HSV_Colour(d_selectedColourRGB);

    float alpha = d_selectedColour.getAlpha();
    d_selectedColour = ColourPickerConversions::toCeguiColour(d_selectedColourRGB);
    d_selectedColour.setAlpha(alpha);
}

//----------------------------------------------------------------------------//
void ColourPickerControls::setColours(const RGB_Colour& newColourRGB)
{
    d_selectedColourLAB = Lab_Colour(newColourRGB);
    d_selectedColourRGB = newColourRGB;
    d_selectedColourHSV = HSV_Colour(newColourRGB);

    float alpha = d_selectedColour.getAlpha();
    d_selectedColour = ColourPickerConversions::toCeguiColour(d_selectedColourRGB);
    d_selectedColour.setAlpha(alpha);
}

//----------------------------------------------------------------------------//
void ColourPickerControls::setColours(const HSV_Colour& newColourHSV)
{
    d_selectedColourRGB = RGB_Colour(newColourHSV);
    d_selectedColourLAB = Lab_Colour(d_selectedColourRGB);
    d_selectedColourHSV = newColourHSV;

    float alpha = d_selectedColour.getAlpha();
    d_selectedColour = ColourPickerConversions::toCeguiColour(d_selectedColourRGB);
    d_selectedColour.setAlpha(alpha);
}

//----------------------------------------------------------------------------//
void ColourPickerControls::setCallingColourPicker(ColourPicker* colourPicker)
{
    d_callingColourPicker = colourPicker;
}

//----------------------------------------------------------------------------//
PushButton* ColourPickerControls::getCancelButton()
{
    return static_cast<PushButton*>(getChild(CancelButtonName));
}

//----------------------------------------------------------------------------//
PushButton* ColourPickerControls::getAcceptButton()
{
    return static_cast<PushButton*>(getChild(AcceptButtonName));
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getHexadecimalDescription()
{
    return getChild(HexaDecimalDescriptionName);
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getHexadecimalEditbox()
{
    return static_cast<Editbox*>(getChild(HexaDecimalEditBoxName));
}

//----------------------------------------------------------------------------//
Titlebar* ColourPickerControls::getTitleBar()
{
    return static_cast<Titlebar*>(getChild(TitleBarName));
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getColourPickerStaticImage()
{
    return getChild(ColourPickerStaticImageName);
}

//----------------------------------------------------------------------------//
Slider* ColourPickerControls::getColourPickerImageSlider()
{
    return static_cast<Slider*>(getChild(ColourPickerImageSliderName));
}

//----------------------------------------------------------------------------//
Slider* ColourPickerControls::getColourPickerAlphaSlider()
{
    return static_cast<Slider*>(getChild(ColourPickerAlphaSliderName));
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getNewColourDescription()
{
    return getChild(NewColourDescriptionName);
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getOldColourDescription()
{
    return getChild(OldColourDescriptionName);
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getNewColourRect()
{
    return getChild(NewColourRectName);
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getOldColourRect()
{
    return getChild(OldColourRectName);
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getColourEditBoxRDescription()
{
    return getChild(ColourEditBoxRDescriptionName);
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getColourEditBoxGDescription()
{
    return getChild(ColourEditBoxGDescriptionName);
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getColourEditBoxBDescription()
{
    return getChild(ColourEditBoxBDescriptionName);
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getColourEditBoxR()
{
    return static_cast<Editbox*>(getChild(ColourEditBoxRName));
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getColourEditBoxG()
{
    return static_cast<Editbox*>(getChild(ColourEditBoxGName));
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getColourEditBoxB()
{
    return static_cast<Editbox*>(getChild(ColourEditBoxBName));
}

//----------------------------------------------------------------------------//
RadioButton* ColourPickerControls::getHSVRadioButtonH()
{
    return static_cast<RadioButton*>(getChild(HSVRadioButtonHName));
}

//----------------------------------------------------------------------------//
RadioButton* ColourPickerControls::getHSVRadioButtonS()
{
    return static_cast<RadioButton*>(getChild(HSVRadioButtonSName));
}

//----------------------------------------------------------------------------//
RadioButton* ColourPickerControls::getHSVRadioButtonV()
{
    return static_cast<RadioButton*>(getChild(HSVRadioButtonVName));
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getHSVEditBoxH()
{
    return static_cast<Editbox*>(getChild(HSVEditBoxHName));
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getHSVEditBoxS()
{
    return static_cast<Editbox*>(getChild(HSVEditBoxSName));
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getHSVEditBoxV()
{
    return static_cast<Editbox*>(getChild(HSVEditBoxVName));
}


//----------------------------------------------------------------------------//
RadioButton* ColourPickerControls::getLabRadioButtonL()
{
    return static_cast<RadioButton*>(getChild(LabRadioButtonLName));
}

//----------------------------------------------------------------------------//
RadioButton* ColourPickerControls::getLabRadioButtonA()
{
    return static_cast<RadioButton*>(getChild(LabRadioButtonAName));
}

//----------------------------------------------------------------------------//
RadioButton* ColourPickerControls::getLabRadioButtonB()
{
    return static_cast<RadioButton*>(getChild(LabRadioButtonBName));
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getLabEditBoxL()
{
    return static_cast<Editbox*>(getChild(LabEditBoxLName));
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getLabEditBoxA()
{
    return static_cast<Editbox*>(getChild(LabEditBoxAName));
}

//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getLabEditBoxB()
{
    return static_cast<Editbox*>(getChild(LabEditBoxBName));
}


//----------------------------------------------------------------------------//
Editbox* ColourPickerControls::getAlphaEditBox()
{
    return static_cast<Editbox*>(getChild(AlphaEditBoxName));
}

//----------------------------------------------------------------------------//
Window* ColourPickerControls::getColourPickerCursorStaticImage()
{
    return d_colourPickerCursor;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleCancelButtonClicked(const EventArgs&)
{
    WindowEventArgs args(this);
    onCancelButtonClicked(args);

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleEditboxDeactivated(const EventArgs& args)
{
    Editbox* editbox = static_cast<Editbox*>(
        static_cast<const WindowEventArgs&>(args).window);
    editbox->setSelection(0, 0);

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleHexadecimalEditboxTextChanged(const EventArgs&)
{
    if (d_ignoreEvents == true)
        return true;

    String hexaText = getHexadecimalEditbox()->getText();

    if (hexaText.length() == 8)
    {
        Colour hexaColour = PropertyHelper<Colour>::fromString(hexaText);
        setColours(hexaColour);
        refreshAllElements();
    }

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleRGBEditboxTextChanged(const EventArgs&)
{
    if (d_ignoreEvents == true)
        return true;

    int colourR = PropertyHelper<int>::fromString(
                      getColourEditBoxR()->getText());
    int colourG = PropertyHelper<int>::fromString(
                      getColourEditBoxG()->getText());
    int colourB = PropertyHelper<int>::fromString(
                      getColourEditBoxB()->getText());
    RGB_Colour newColour(colourR, colourG, colourB);

    setColours(newColour);

    refreshAllElements();

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleLABEditboxTextChanged(const EventArgs&)
{
    if (d_ignoreEvents == true)
        return true;

    static const String labRegEx = "[-+]?[0-9]*\\.?[0-9]+";
    d_regexMatcher.setRegexString(labRegEx);

    String LabLString = getLabEditBoxL()->getText();
    String LabAString = getLabEditBoxA()->getText();
    String LabBString = getLabEditBoxB()->getText();

    bool matchingRegEx = true;
    matchingRegEx &= d_regexMatcher.getMatchStateOfString(LabLString) == RegexMatcher::MS_VALID;
    matchingRegEx &= d_regexMatcher.getMatchStateOfString(LabAString) == RegexMatcher::MS_VALID;
    matchingRegEx &= d_regexMatcher.getMatchStateOfString(LabBString) == RegexMatcher::MS_VALID;

    if (!matchingRegEx)
        return true;

    float LabColourL = PropertyHelper<float>::fromString(LabLString);
    float LabColourA = PropertyHelper<float>::fromString(LabAString);
    float LabColourB = PropertyHelper<float>::fromString(LabBString);

    LabColourL = ceguimin(ceguimax(LabColourL, LAB_L_MIN), LAB_L_MAX);
    LabColourA = ceguimin(ceguimax(LabColourA, LAB_A_MIN), LAB_A_MAX);
    LabColourB = ceguimin(ceguimax(LabColourB, LAB_B_MIN), LAB_B_MAX);

    Lab_Colour newColour(LabColourL, LabColourA, LabColourB);

    setColours(newColour);

    refreshAllElements();

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleHSVEditboxTextChanged(const EventArgs&)
{
    if (d_ignoreEvents == true)
        return true;

    static const String labRegEx = "[-+]?[0-9]*\\.?[0-9]+";
    d_regexMatcher.setRegexString(labRegEx);

    String HString = getHSVEditBoxH()->getText();
    String SString = getHSVEditBoxS()->getText();
    String VString = getHSVEditBoxV()->getText();

    bool matchingRegEx = true;
    matchingRegEx &= d_regexMatcher.getMatchStateOfString(HString) == RegexMatcher::MS_VALID;
    matchingRegEx &= d_regexMatcher.getMatchStateOfString(SString) == RegexMatcher::MS_VALID;
    matchingRegEx &= d_regexMatcher.getMatchStateOfString(VString) == RegexMatcher::MS_VALID;

    if (!matchingRegEx)
        return true;

    float Hue = PropertyHelper<float>::fromString(HString);
    float Saturation = PropertyHelper<float>::fromString(SString);
    float Value = PropertyHelper<float>::fromString(VString);

    Hue = ceguimin(ceguimax(Hue, 0.0f), 1.0f);
    Saturation = ceguimin(ceguimax(Saturation, 0.0f), 1.0f);
    Value = ceguimin(ceguimax(Value, 0.0f), 1.0f);

    HSV_Colour newColour(Hue, Saturation, Value);

    setColours(newColour);

    refreshAllElements();

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleAlphaEditboxTextChanged(const EventArgs&)
{
    if (d_ignoreEvents == true)
        return true;

    static const String labRegEx = "[-+]?[0-9]*\\.?[0-9]+";
    d_regexMatcher.setRegexString(labRegEx);

    String ValueString = getAlphaEditBox()->getText();
    bool matchingRegEx = d_regexMatcher.getMatchStateOfString(ValueString) == RegexMatcher::MS_VALID;

    if (!matchingRegEx)
        return true;

    float value = PropertyHelper<float>::fromString(ValueString);

    value = ceguimax(ceguimin(value, 1.0f), 0.0f);

    setColourAlpha(value);

    refreshAllElements();

    return true;
}

//----------------------------------------------------------------------------//
void ColourPickerControls::setColourAlpha(float alphaValue)
{
    d_selectedColour.setAlpha(alphaValue);
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleAcceptButtonClicked(const EventArgs&)
{
    WindowEventArgs args(this);
    onAcceptButtonClicked(args);

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleRadioButtonModeSelection(const EventArgs& args)
{
    RadioButton* radioButton = static_cast<RadioButton*>(
                                   static_cast<const WindowEventArgs&>(args).window);

    if (getLabRadioButtonL() == radioButton)
        d_sliderMode = SliderMode_Lab_L;
    else if (getLabRadioButtonA() == radioButton)
        d_sliderMode = SliderMode_Lab_A;
    else if (getLabRadioButtonB() == radioButton)
        d_sliderMode = SliderMode_Lab_B;
    else if (getHSVRadioButtonH() == radioButton)
        d_sliderMode = SliderMode_HSV_H;
    else if (getHSVRadioButtonS() == radioButton)
        d_sliderMode = SliderMode_HSV_S;
    else if (getHSVRadioButtonV() == radioButton)
        d_sliderMode = SliderMode_HSV_V;

    refreshColourPickerCursorPosition();

    refreshColourSliderPosition();

    refreshColourPickerControlsTextures();

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleColourPickerSliderValueChanged(
    const EventArgs& args)
{
    if (d_ignoreEvents == true)
        return true;

    Slider* imageSlider = static_cast<Slider*>(
        static_cast<const WindowEventArgs&>(args).window);

    float sliderValue = imageSlider->getCurrentValue();

    Lab_Colour colourLAB = d_selectedColourLAB;
    HSV_Colour colourHSV = d_selectedColourHSV;

    switch (d_sliderMode)
    {
    case SliderMode_Lab_L:
        colourLAB.L = LAB_L_MIN + LAB_L_DIFF * sliderValue;
        setColours(colourLAB);
        break;

    case SliderMode_Lab_A:
        colourLAB.a = LAB_A_MIN + LAB_A_DIFF * sliderValue;
        setColours(colourLAB);
        break;

    case SliderMode_Lab_B:
        colourLAB.b = LAB_B_MIN + LAB_B_DIFF * sliderValue;
        setColours(colourLAB);
        break;

    case SliderMode_HSV_H:
        colourHSV.H = sliderValue;
        setColours(colourHSV);
        break;

    case SliderMode_HSV_S:
        colourHSV.S = sliderValue;
        setColours(colourHSV);
        break;

    case SliderMode_HSV_V:
        colourHSV.V = sliderValue;
        setColours(colourHSV);
        break;

    default:
        break;
    }

    onColourSliderChanged();

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleAlphaSliderValueChanged(const EventArgs& args)
{
    if (d_ignoreEvents == true)
        return true;

    Slider* imageSlider = static_cast<Slider*>(
        static_cast<const WindowEventArgs&>(args).window);
    float sliderValue = 1.0f - imageSlider->getCurrentValue();

    setColourAlpha(sliderValue);

    refreshAlpha();

    refreshColourRects();


    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleColourPickerStaticImageMouseButtonUp(
    const EventArgs& args)
{
    const MouseEventArgs& mouseArgs = static_cast<const MouseEventArgs&>(args);

    if (mouseArgs.button == LeftButton)
        d_draggingColourPickerCursor = false;

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleColourPickerStaticImageMouseButtonDown(
    const EventArgs& args)
{
    const MouseEventArgs& mouseArgs = static_cast<const MouseEventArgs&>(args);

    if (mouseArgs.button == LeftButton)
    {
        d_draggingColourPickerCursor = true;

        refreshColourPickerCursorPosition(mouseArgs);
    }

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleColourPickerStaticImageMouseMove(
    const EventArgs& args)
{
    if (d_colourPickerCursor && d_draggingColourPickerCursor)
        refreshColourPickerCursorPosition(
            static_cast<const MouseEventArgs&>(args));

    return true;
}

//----------------------------------------------------------------------------//
bool ColourPickerControls::handleColourPickerStaticImageMouseLeaves(
    const EventArgs&)
{
    if (d_colourPickerCursor)
        d_draggingColourPickerCursor = false;

    return true;
}

//----------------------------------------------------------------------------//
void ColourPickerControls::onCancelButtonClicked(WindowEventArgs& e)
{
    if (this->getParent() != 0)
        this->getParent()->removeChild(this);

    fireEvent(EventClosed, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void ColourPickerControls::onAcceptButtonClicked(WindowEventArgs& e)
{
    d_callingColourPicker->setColour(d_selectedColour);

    if (this->getParent() != 0)
        this->getParent()->removeChild(this);

    fireEvent(EventColourAccepted, e, EventNamespace);
    fireEvent(EventClosed, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshColourPickingImage()
{
    if (d_sliderMode &
            (SliderMode_Lab_L | SliderMode_Lab_A | SliderMode_Lab_B))
    {
        for (int y = 0; y < d_colourPickerPickingImageHeight; ++y)
        {
            for (int x = 0; x < d_colourPickerPickingImageWidth; ++x)
            {
                int i = d_colourPickerControlsTextureSize * y + x;

                Lab_Colour colour =
                    getColourPickingPositionColourLAB(static_cast<float>(x),
                                                      static_cast<float>(y));

                d_colourPickingTexture[i] = RGB_Colour(colour);
            }
        }
    }
    else if (d_sliderMode &
             (SliderMode_HSV_H | SliderMode_HSV_S | SliderMode_HSV_V))
    {
        for (int y = 0; y < d_colourPickerPickingImageHeight; ++y)
        {
            for (int x = 0; x < d_colourPickerPickingImageWidth; ++x)
            {
                int i = d_colourPickerControlsTextureSize * y + x;

                HSV_Colour colour =
                    getColourPickingPositionColourHSV(static_cast<float>(x),
                                                      static_cast<float>(y));

                d_colourPickingTexture[i] = RGB_Colour(colour);
            }
        }
    }
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshColourSliderImage()
{
    if (d_sliderMode &
            (SliderMode_Lab_L | SliderMode_Lab_A | SliderMode_Lab_B))
    {
        for (int y = 0; y < d_colourPickerPickingImageHeight; ++y)
        {
            for (int x = 0; x < d_colourPickerColourSliderImageWidth; ++x)
            {
                int i = d_colourPickerControlsTextureSize * y +
                        (x + d_colourPickerPickingImageWidth + 2);

                Lab_Colour colour = getColourSliderPositionColourLAB(
                    y / static_cast<float>(d_colourPickerPickingImageHeight - 1));

                d_colourPickingTexture[i] = RGB_Colour(colour);
            }
        }
    }
    else if (d_sliderMode &
             (SliderMode_HSV_H | SliderMode_HSV_S | SliderMode_HSV_V))
    {
        for (int y = 0; y < d_colourPickerPickingImageHeight; ++y)
        {
            for (int x = 0; x < d_colourPickerColourSliderImageWidth; ++x)
            {
                int i = d_colourPickerControlsTextureSize * y +
                        (x + d_colourPickerPickingImageWidth + 2);

                HSV_Colour colour = getColourSliderPositionColourHSV(
                    y / static_cast<float>(d_colourPickerPickingImageHeight - 1));

                d_colourPickingTexture[i] = RGB_Colour(colour);
            }
        }
    }
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshAlphaSliderImage()
{
    for (int y = 0; y < d_colourPickerAlphaSliderImageHeight; ++y)
    {
        for (int x = 0; x < d_colourPickerAlphaSliderImageWidth; ++x)
        {
            int i = x + d_colourPickerControlsTextureSize *
                (y + d_colourPickerImageOffset + d_colourPickerPickingImageHeight);

            RGB_Colour curColour = getAlphaSliderPositionColour(x, y);
            d_colourPickingTexture[i] = curColour;
        }
    }
}

//----------------------------------------------------------------------------//
RGB_Colour ColourPickerControls::getAlphaSliderPositionColour(int x, int y)
{
    static const RGB_Colour background1(255, 255, 255);
    static const RGB_Colour background2(122, 122, 122);

    float xRel = x / static_cast<float>(d_colourPickerAlphaSliderImageWidth - 1);
    RGB_Colour final;

    bool isBackground1;

    if ((x % 30) >= 15)
        isBackground1 = false;
    else
        isBackground1 = true;

    if ((y % 30) >= 15)
        isBackground1 = !isBackground1;

    if (isBackground1)
        final = d_selectedColourRGB * (1.0f - xRel) + background1 * xRel;
    else
        final = d_selectedColourRGB * (1.0f - xRel) + background2 * xRel;

    return final;
}


//----------------------------------------------------------------------------//
void ColourPickerControls::initColourPicker()
{
    initColourPickerControlsImageSet();

    d_sliderMode = SliderMode_Lab_L;
    getLabRadioButtonL()->setSelected(true);

    d_colourPickerCursor = WindowManager::getSingleton().createWindow(
        getProperty("ColourPickerCursorStyle"),
        getName() + ColourPickerCursorName);

    d_colourPickerCursor->setProperty(
        "BackgroundEnabled",
        PropertyHelper<bool>::toString(false));

    d_colourPickerCursor->setProperty(
        "FrameEnabled",
        PropertyHelper<bool>::toString(false));

    d_colourPickerCursor->setProperty(
        "Image",
        getProperty("ColourPickerCursorImage"));

    d_colourPickerCursor->setProperty(
        "ImageColours",
        PropertyHelper<ColourRect>::toString(
            ColourRect(Colour(0.0f, 0.0f, 0.0f))));

    d_colourPickerCursor->setWidth(UDim(0.05f, 0));
    d_colourPickerCursor->setHeight(UDim(0.05f, 0));
    d_colourPickerCursor->setMousePassThroughEnabled(true);
    d_colourPickerCursor->setClippedByParent(false);

    getColourPickerImageSlider()->getThumb()->setHotTracked(false);

    getColourPickerStaticImage()->addChild(d_colourPickerCursor);
}

//----------------------------------------------------------------------------//
void ColourPickerControls::positionColourPickerCursorRelative(float x, float y)
{
    if (d_colourPickerCursor)
    {
        d_colourPickerCursor->setXPosition(
            UDim(d_colourPickerCursor->getWidth().d_scale * -0.5f + x, 0.0f));
        d_colourPickerCursor->setYPosition(
            UDim(d_colourPickerCursor->getHeight().d_scale * -0.5f + y, 0.0f));
    }
}

//----------------------------------------------------------------------------//
void ColourPickerControls::positionColourPickerCursorAbsolute(float x, float y)
{
    if (d_colourPickerCursor)
    {
        d_colourPickerCursor->setXPosition(
            UDim(d_colourPickerCursor->getWidth().d_scale * -0.5f, x));
        d_colourPickerCursor->setYPosition(
            UDim(d_colourPickerCursor->getHeight().d_scale * -0.5f, y));
    }
}

//----------------------------------------------------------------------------//
RGB_Colour ColourPickerControls::getSelectedColourRGB()
{
    return d_selectedColourRGB;
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshColourPickerCursorPosition()
{
    Vector2f pos = getColourPickingColourPosition();

    positionColourPickerCursorRelative(pos.d_x, pos.d_y);
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshColourPickerCursorPosition(
    const MouseEventArgs& mouseEventArgs)
{
    Vector2f position = mouseEventArgs.position;

    float x = CoordConverter::screenToWindowX(
                  *mouseEventArgs.window, position.d_x);
    float y = CoordConverter::screenToWindowY(
                  *mouseEventArgs.window, position.d_y);
    positionColourPickerCursorAbsolute(x, y);

    if (d_sliderMode &
        (SliderMode_Lab_L | SliderMode_Lab_A | SliderMode_Lab_B))
    {
        Lab_Colour col = getColourPickingPositionColourLAB(x, y);
        setColours(col);
    }
    else if (d_sliderMode &
             (SliderMode_HSV_H | SliderMode_HSV_S | SliderMode_HSV_V))
    {
        HSV_Colour col = getColourPickingPositionColourHSV(x, y);
        setColours(col);
    }

    onColourCursorPositionChanged();
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshColourSliderPosition()
{
    String editboxText;

    switch (d_sliderMode)
    {
    case SliderMode_Lab_L:
        editboxText = getLabEditBoxL()->getText();
        break;

    case SliderMode_Lab_A:
        editboxText = getLabEditBoxA()->getText();
        break;

    case SliderMode_Lab_B:
        editboxText = getLabEditBoxB()->getText();
        break;

    case SliderMode_HSV_H:
        editboxText = getHSVEditBoxH()->getText();
        break;

    case SliderMode_HSV_S:
        editboxText = getHSVEditBoxS()->getText();
        break;

    case SliderMode_HSV_V:
        editboxText = getHSVEditBoxV()->getText();
        break;

    default:
        Logger::getSingleton().logEvent(
            "[ColourPicker] Unknown slider mode - action not processed");
        break;
    }

    float value = PropertyHelper<float>::fromString(editboxText);
    float sliderValue = 0.0f;

    switch (d_sliderMode)
    {
    case SliderMode_Lab_L:
        sliderValue = 1.0f - (LAB_L_MAX - value) / LAB_L_DIFF;
        break;

    case SliderMode_Lab_A:
        sliderValue = 1.0f - (LAB_A_MAX - value) / LAB_A_DIFF;
        break;

    case SliderMode_Lab_B:
        sliderValue = 1.0f - (LAB_B_MAX - value) / LAB_B_DIFF;
        break;

    case SliderMode_HSV_H:
        sliderValue = value;
        break;

    case SliderMode_HSV_S:
        sliderValue = value;
        break;

    case SliderMode_HSV_V:
        sliderValue = value;
        break;

    default:
        Logger::getSingleton().logEvent(
            "[ColourPicker] Unknown slider mode - action not processed");
        break;
    }

    d_ignoreEvents = true;
    getColourPickerImageSlider()->setCurrentValue(sliderValue);
    d_ignoreEvents = false;
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshAlpha()
{
    d_ignoreEvents = true;

    getColourPickerAlphaSlider()->setCurrentValue(
        1.0f - d_selectedColour.getAlpha());

    std::stringstream floatStringStream;
    floatStringStream.precision(4);
    floatStringStream.setf(std::ios::fixed, std::ios::floatfield);
    floatStringStream << d_selectedColour.getAlpha();
    getAlphaEditBox()->setText(floatStringStream.str().c_str());

    d_ignoreEvents = false;
}

//----------------------------------------------------------------------------//
void ColourPickerControls::onColourSliderChanged()
{
    refreshEditboxesAndColourRects();

    refreshColourPickerCursorPosition();

    refreshColourPickerControlsTextures();
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshEditboxesAndColourRects()
{
    refreshColourRects();

    d_ignoreEvents = true;

    getColourEditBoxR()->setText(
        PropertyHelper<int>::toString(d_selectedColourRGB.r));
    getColourEditBoxG()->setText(
        PropertyHelper<int>::toString(d_selectedColourRGB.g));
    getColourEditBoxB()->setText(
        PropertyHelper<int>::toString(d_selectedColourRGB.b));

    std::stringstream floatStringStream;
    floatStringStream.precision(3);
    floatStringStream.setf(std::ios::fixed, std::ios::floatfield);

    floatStringStream << d_selectedColourLAB.L;
    getLabEditBoxL()->setText(floatStringStream.str().c_str());
    floatStringStream.str("");
    floatStringStream << d_selectedColourLAB.a;
    getLabEditBoxA()->setText(floatStringStream.str().c_str());
    floatStringStream.str("");
    floatStringStream << d_selectedColourLAB.b;
    getLabEditBoxB()->setText(floatStringStream.str().c_str());
    floatStringStream.str("");

    floatStringStream.precision(5);

    floatStringStream << d_selectedColourHSV.H;
    getHSVEditBoxH()->setText(floatStringStream.str().c_str());
    floatStringStream.str("");
    floatStringStream << d_selectedColourHSV.S;
    getHSVEditBoxS()->setText(floatStringStream.str().c_str());
    floatStringStream.str("");
    floatStringStream << d_selectedColourHSV.V;
    getHSVEditBoxV()->setText(floatStringStream.str().c_str());
    floatStringStream.str("");

    floatStringStream.precision(4);
    floatStringStream << d_selectedColour.getAlpha();
    getAlphaEditBox()->setText(floatStringStream.str().c_str());
    floatStringStream.str("");

    d_ignoreEvents = false;
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshAllElements()
{
    refreshEditboxesAndColourRects();

    refreshColourSliderPosition();

    refreshColourPickerCursorPosition();

    refreshAlpha();

    refreshColourPickerControlsTextures();
}

//----------------------------------------------------------------------------//
void ColourPickerControls::onColourCursorPositionChanged()
{
    refreshEditboxesAndColourRects();

    refreshAlphaSliderImage();

    refreshOnlyColourSliderImage();
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshOnlyColourSliderImage()
{
    refreshColourSliderImage();
    reloadColourPickerControlsTexture();
}

//----------------------------------------------------------------------------//
void ColourPickerControls::refreshColourRects()
{
    d_ignoreEvents = true;

    Colour newColourNoAlpha(d_selectedColour.getRed(),
                            d_selectedColour.getGreen(),
                            d_selectedColour.getBlue());

    String newColourRectString =
        PropertyHelper<ColourRect>::toString(ColourRect(newColourNoAlpha));

    Colour previousColourNoAlpha(d_previouslySelectedColour.getRed(),
                                 d_previouslySelectedColour.getGreen(),
                                 d_previouslySelectedColour.getBlue());

    String previousColourRectString =
        PropertyHelper<ColourRect>::toString(ColourRect(previousColourNoAlpha));

    getNewColourRect()->setProperty(
        ColourRectPropertyName,
        newColourRectString);

    getOldColourRect()->setProperty(
        ColourRectPropertyName,
        previousColourRectString);

    String colourString = PropertyHelper<Colour>::toString(d_selectedColour);
    getHexadecimalEditbox()->setText(colourString);

    d_ignoreEvents = false;
}

//----------------------------------------------------------------------------//

}


