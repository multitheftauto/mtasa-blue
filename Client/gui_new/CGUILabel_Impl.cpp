/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUILabel_Impl.cpp
 *  PURPOSE:     Label widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUILABEL_NAME "StaticText"

using namespace GUINew;

CGUILabel_Impl::CGUILabel_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, const char* szText)
{
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager()->createWindow(pGUI->GetElementPrefix() + "/" + CGUILABEL_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);

    m_pWindow->setText(szText);
    
    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    AddEvents();

    // Do some hardcore disabling on the labels
    // m_pWindow->moveToBack ( );
    // m_pWindow->disable ( );

    // not sure what that was for, disabled
    // m_pWindow->setZOrderingEnabled ( false );
    // m_pWindow->setAlwaysOnTop ( true );

    SetFrameEnabled(false);
    SetHorizontalAlign(CGUI_ALIGN_LEFT);
    SetVerticalAlign(CGUI_ALIGN_TOP);
    SetText(szText);
    m_pWindow->setProperty("BackgroundEnabled", "false");

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if (pParent)
    {
        SetParent(pParent);
    }
    else
    {
        pGUI->AddChild(this);
        SetParent(NULL);
    }
}

CGUILabel_Impl::~CGUILabel_Impl()
{
    DestroyElement();
}

void CGUILabel_Impl::SetText(const char* Text)
{
    m_pWindow->setText(CGUI_Impl::GetUTFString(Text));
}

void CGUILabel_Impl::SetVerticalAlign(CGUIVerticalAlign eAlign)
{
    m_pWindow->setProperty("VertFormatting", CGUIVerticalAlignValues[eAlign]);
}

CGUIVerticalAlign CGUILabel_Impl::GetVerticalAlign()
{
    SString verticalAlign = m_pWindow->getProperty("VertFormatting").c_str();

    if (verticalAlign == CGUIVerticalAlignValues[1])
        return CGUIVerticalAlign::CGUI_ALIGN_TOP;

    if (verticalAlign == CGUIVerticalAlignValues[2])
        return CGUIVerticalAlign::CGUI_ALIGN_BOTTOM;

    if (verticalAlign == CGUIVerticalAlignValues[3])
        return CGUIVerticalAlign::CGUI_ALIGN_VERTICALCENTER;

    // Fallback
    return CGUIVerticalAlign::CGUI_ALIGN_TOP;
}

void CGUILabel_Impl::SetHorizontalAlign(CGUIHorizontalAlign eAlign)
{
    m_pWindow->setProperty("HorzFormatting", CGUIHorizontalAlignValues[eAlign]);
}

CGUIHorizontalAlign CGUILabel_Impl::GetHorizontalAlign()
{
    SString horizontalAlign = m_pWindow->getProperty("HorzFormatting").c_str();

    if (horizontalAlign == CGUIHorizontalAlignValues[1])
        return CGUIHorizontalAlign::CGUI_ALIGN_LEFT;

    if (horizontalAlign == CGUIHorizontalAlignValues[2])
        return CGUIHorizontalAlign::CGUI_ALIGN_RIGHT;

    if (horizontalAlign == CGUIHorizontalAlignValues[3])
        return CGUIHorizontalAlign::CGUI_ALIGN_HORIZONTALCENTER;

    if (horizontalAlign == CGUIHorizontalAlignValues[4])
        return CGUIHorizontalAlign::CGUI_ALIGN_LEFT_WORDWRAP;

    if (horizontalAlign == CGUIHorizontalAlignValues[5])
        return CGUIHorizontalAlign::CGUI_ALIGN_RIGHT_WORDWRAP;

    if (horizontalAlign == CGUIHorizontalAlignValues[6])
        return CGUIHorizontalAlign::CGUI_ALIGN_HORIZONTALCENTER_WORDWRAP;

    // Fallback
    return CGUIHorizontalAlign::CGUI_ALIGN_LEFT;
}

void CGUILabel_Impl::SetTextColor(CGUIColor Color)
{
    std::uint32_t argb = 0xFF000000; // alpha 255
    argb |= static_cast<std::uint32_t>(Color.R) << 16;
    argb |= static_cast<std::uint32_t>(Color.G) << 8;
    argb |= static_cast<std::uint32_t>(Color.B);
    SString hexString = SString("%08x", argb).ToUpper();

    m_pWindow->setProperty("TextColours", SString("tl:%s tr:%s bl:%s br:%s", *hexString, *hexString, *hexString, *hexString));
}

void CGUILabel_Impl::SetTextColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    std::uint32_t argb = 0xFF000000; // alpha 255
    argb |= static_cast<std::uint32_t>(ucRed) << 16;
    argb |= static_cast<std::uint32_t>(ucGreen) << 8;
    argb |= static_cast<std::uint32_t>(ucBlue);
    SString hexString = SString("%08x", argb).ToUpper();

    m_pWindow->setProperty("TextColours", SString("tl:%s tr:%s bl:%s br:%s", *hexString, *hexString, *hexString, *hexString));
}

CGUIColor CGUILabel_Impl::GetTextColor()
{
    CGUIColor temp;
    GetTextColor(temp.R, temp.G, temp.B);
    return temp;
}

void CGUILabel_Impl::GetTextColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue)
{
    //CEGUI::Colour r = (m_pLabel->getColours()).getColourAtPoint(0, 0);
    CEGUI::Colour r = CEGUI::Colour(255, 255, 255);

    ucRed = (unsigned char)(r.getRed() * 255);
    ucGreen = (unsigned char)(r.getGreen() * 255);
    ucBlue = (unsigned char)(r.getBlue() * 255);
}

void CGUILabel_Impl::SetFrameEnabled(bool bFrameEnabled)
{
    m_pWindow->setProperty("FrameEnabled", bFrameEnabled ? "True" : "False");
}

bool CGUILabel_Impl::IsFrameEnabled()
{
    return m_pWindow->getProperty("FrameEnabled") == "True" ? true : false;
}

float CGUILabel_Impl::GetCharacterWidth(int iCharIndex)
{
    if (true)
        return true;
}

float CGUILabel_Impl::GetFontHeight()
{
    const CEGUI::Font* pFont = m_pWindow->getFont();
    if (pFont)
        return pFont->getFontHeight();
    return 14.0f;
}

float CGUILabel_Impl::GetTextExtent()
{
    const CEGUI::Font* pFont = m_pWindow->getFont();
    if (pFont)
    {
        try
        {
            // Retrieve the longest line's extent
            std::stringstream ssText(m_pWindow->getText().c_str());
            std::string       sLineText;
            float             fMax = 0.0f, fLineExtent = 0.0f;

            while (std::getline(ssText, sLineText))
            {
                fLineExtent = pFont->getTextExtent(CGUI_Impl::GetUTFString(sLineText));
                if (fLineExtent > fMax)
                    fMax = fLineExtent;
            }
            return fMax;
        }
        catch (CEGUI::Exception e)
        {
        }
    }

    return 0.0f;
}
