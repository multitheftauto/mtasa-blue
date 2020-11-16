/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIListItem_Impl.h
 *  PURPOSE:     List widget item class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui_new/CGUIListItem.h>
#include <CEGUI/CEGUI.h>

class CGUIStaticImage;
class CGUIStaticImage_Impl;

// Create custom ListboxItem types
//namespace CEGUI
//{
//    class ImageListboxItem : public ItemEntry
//    {
//    protected:
//    public:
//        ImageListboxItem(const String &type, const String &name);
//        virtual ~ImageListboxItem();
//        static const String WidgetTypeName;
//    };
//    CEGUI_DECLARE_WINDOW_FACTORY(ImageListboxItem);
//}

class CGUIListItem_Impl : public CGUIListItem
{
public:
    enum Type
    {
        TextItem = 0,
        NumberItem = 1,
        ImageItem = 2
    };

    CGUIListItem_Impl(CGUI_Impl* pGUI, const char* szText = "", unsigned int uiType = 0, CGUIStaticImage_Impl* pImage = NULL);
    ~CGUIListItem_Impl();

    std::string GetText() const;
    void        SetText(const char* pszText);

    void* GetData() const { return m_pData; }
    void  SetData(void* pData, CGUICallback<void, void*> deleteDataCallback = NULL)
    {
        m_pData = pData;
        m_deleteDataCallback = deleteDataCallback;
    }
    void SetData(const char* pszData);

    void SetDisabled(bool bDisabled);
    void SetFont(const char* szFontName);
    void SetImage(CGUIStaticImage* Image);

    bool GetSelectedState();
    void SetSelectedState(bool bState);

    bool GetColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha);
    void SetColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha);

    CEGUI::ListboxItem* GetListItem();

    unsigned int ItemType;

private:
    CEGUI::ListboxItem*       m_pListItem;
    void*                     m_pData;
    std::string               m_strData;
    CGUICallback<void, void*> m_deleteDataCallback;
};
