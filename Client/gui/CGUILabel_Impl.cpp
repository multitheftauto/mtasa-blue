/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUILabel_Impl.cpp
 *  PURPOSE:     Label widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUILABEL_NAME "CGUI/StaticText"

CGUILabel_Impl::CGUILabel_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, const char* szText)
{
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager()->createWindow(CGUILABEL_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);

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
    reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->setBackgroundEnabled(false);

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
    // Set the new text and size the text field after it
    m_pWindow->setText(CGUI_Impl::GetUTFString(Text));
}

void CGUILabel_Impl::SetVerticalAlign(CGUIVerticalAlign eAlign)
{
    reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->setVerticalFormatting(static_cast<CEGUI::StaticText::VertFormatting>(eAlign));
}

CGUIVerticalAlign CGUILabel_Impl::GetVerticalAlign()
{
    return static_cast<CGUIVerticalAlign>(reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->getVerticalFormatting());
}

void CGUILabel_Impl::SetHorizontalAlign(CGUIHorizontalAlign eAlign)
{
    reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->setHorizontalFormatting(static_cast<CEGUI::StaticText::HorzFormatting>(eAlign));
}

CGUIHorizontalAlign CGUILabel_Impl::GetHorizontalAlign()
{
    return static_cast<CGUIHorizontalAlign>(reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->getHorizontalFormatting());
}

void CGUILabel_Impl::SetTextColor(CGUIColor Color)
{
    reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->setTextColours(CEGUI::colour(1.0f / 255.0f * Color.R, 1.0f / 255.0f * Color.G, 1.0f / 255.0f * Color.B));
}

void CGUILabel_Impl::SetTextColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->setTextColours(CEGUI::colour(1.0f / 255.0f * ucRed, 1.0f / 255.0f * ucGreen, 1.0f / 255.0f * ucBlue));
}

CGUIColor CGUILabel_Impl::GetTextColor()
{
    CGUIColor temp;
    GetTextColor(temp.R, temp.G, temp.B);
    return temp;
}

void CGUILabel_Impl::GetTextColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue)
{
    CEGUI::colour r = (reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->getTextColours()).getColourAtPoint(0, 0);

    ucRed = (unsigned char)(r.getRed() * 255);
    ucGreen = (unsigned char)(r.getGreen() * 255);
    ucBlue = (unsigned char)(r.getBlue() * 255);
}

void CGUILabel_Impl::SetFrameEnabled(bool bFrameEnabled)
{
    reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->setFrameEnabled(bFrameEnabled);
}

bool CGUILabel_Impl::IsFrameEnabled()
{
    return reinterpret_cast<CEGUI::StaticText*>(m_pWindow)->isFrameEnabled();
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
