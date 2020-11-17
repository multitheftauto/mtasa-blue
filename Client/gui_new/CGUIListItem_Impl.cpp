/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIListItem_Impl.cpp
 *  PURPOSE:     List widget item class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// Define custom ListboxItem stuff
//namespace CEGUI
//{
//    CEGUI_DEFINE_WINDOW_FACTORY(ImageListboxItem)
//    const String ImageListboxItem::WidgetTypeName("CEGUI/CheckListboxItem");
//    ImageListboxItem::ImageListboxItem(const String &type, const String &name) :
//            CEGUI::ItemEntry(type, name)
//    {
//    }
//    ImageListboxItem::~ImageListboxItem()
//    {
//    }
//}

using namespace GUINew;

CGUIListItem_Impl::CGUIListItem_Impl(CGUI_Impl* pGUI, const char* szText, unsigned int uiType, CGUIStaticImage_Impl* pImage)
{
    ItemType = uiType;

    // Get an unique identifier for CEGUI
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the requested list item type
    switch (uiType)
    {
        case TextItem:
            m_pListItem = new CEGUI::ListboxTextItem(CGUI_Impl::GetUTFString(szText));
            break;
        case ImageItem:
            m_pListItem = new CEGUI::ListboxTextItem(CGUI_Impl::GetUTFString(szText));
            //m_pListItem = reinterpret_cast<CEGUI::ImageListboxItem*>(pGUI->GetWindowManager()->createWindow("CGUI/ImageListboxItem", szUnique));
            break;
        case NumberItem:
            m_pListItem = new CEGUI::ListboxTextItem(CGUI_Impl::GetUTFString(szText));
            break;
    }

    if (m_pListItem)
    {
        // Set flags and properties
        m_pListItem->setAutoDeleted(false);
        m_pListItem->setSelectionBrushImage("CGUI/ListboxSelectionBrush");
    }

    m_pData = NULL;
}

CGUIListItem_Impl::~CGUIListItem_Impl()
{
    if (m_deleteDataCallback)
        m_deleteDataCallback(m_pData);
    delete m_pListItem;
}

void CGUIListItem_Impl::SetDisabled(bool bDisabled)
{
    reinterpret_cast<CEGUI::ListboxItem*>(m_pListItem)->setDisabled(bDisabled);
}

void CGUIListItem_Impl::SetFont(const char* szFontName)
{
    if (szFontName)
        reinterpret_cast<CEGUI::ListboxTextItem*>(m_pListItem)->setFont(CEGUI::String(szFontName));
}

void CGUIListItem_Impl::SetText(const char* pszText)
{
    m_pListItem->setText(CGUI_Impl::GetUTFString(pszText));
}

void CGUIListItem_Impl::SetData(const char* pszData)
{
    if (pszData)
    {
        m_strData = pszData;
        m_pData = (void*)m_strData.c_str();
    }
    else
    {
        m_pData = NULL;
    }
}

void CGUIListItem_Impl::SetImage(CGUIStaticImage* pImage)
{
    if (ItemType == ImageItem)
    {
        //CGUIStaticImage_Impl* pImageImpl = (CGUIStaticImage_Impl*)pImage;
        //m_pListItem->setImage(pImageImpl ? pImageImpl->GetDirectImage() : NULL);
    }
}

std::string CGUIListItem_Impl::GetText() const
{
    return CGUI_Impl::GetUTFString(m_pListItem->getText().c_str()).c_str();
}

CEGUI::ListboxItem* CGUIListItem_Impl::GetListItem()
{
    return m_pListItem;
}

bool CGUIListItem_Impl::GetSelectedState()
{
    return m_pListItem->isSelected();
}

void CGUIListItem_Impl::SetSelectedState(bool bState)
{
    m_pListItem->setSelected(bState);
}

void CGUIListItem_Impl::SetColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha)
{
    if (ItemType == TextItem || ItemType == NumberItem)
    {
        reinterpret_cast<CEGUI::ListboxTextItem*>(m_pListItem)
            ->setTextColours(CEGUI::Colour((float)ucRed / 255.0f, (float)ucGreen / 255.0f, (float)ucBlue / 255.0f, (float)ucAlpha / 255.0f));
    }
}

bool CGUIListItem_Impl::GetColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha)
{
    if (ItemType == TextItem || ItemType == NumberItem)
    {
        CEGUI::Colour color = reinterpret_cast<CEGUI::ListboxTextItem*>(m_pListItem)->getTextColours().d_top_left;
        ucRed = static_cast<unsigned char>(color.getRed() * 255);
        ucGreen = static_cast<unsigned char>(color.getGreen() * 255);
        ucBlue = static_cast<unsigned char>(color.getBlue() * 255);
        ucAlpha = static_cast<unsigned char>(color.getAlpha() * 255);
        return true;
    }
    return false;
}
