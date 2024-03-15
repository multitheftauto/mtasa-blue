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

#include <gui/CGUIListItem.h>
#include <CEGUI.h>

class CGUIStaticImage;
class CGUIStaticImage_Impl;

class CGUIListItem_Impl : public CGUIListItem
{
public:
    enum Type
    {
        TextItem = 0,
        NumberItem = 1,
        ImageItem = 2
    };

    CGUIListItem_Impl(const char* szText = "", std::uint32_t uiType = 0, CGUIStaticImage_Impl* pImage = NULL);
    ~CGUIListItem_Impl();

    std::string GetText() const;
    void        SetText(const char* pszText, const char* pszSortText = NULL);

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

    bool GetColor(std::uint8_t& ucRed, std::uint8_t& ucGreen, std::uint8_t& ucBlue, std::uint8_t& ucAlpha);
    void SetColor(std::uint8_t ucRed, std::uint8_t ucGreen, std::uint8_t ucBlue, std::uint8_t ucAlpha);

    CEGUI::ListboxItem* GetListItem();

    std::uint32_t ItemType;

private:
    CEGUI::ListboxItem*       m_pListItem;
    void*                     m_pData;
    std::string               m_strData;
    CGUICallback<void, void*> m_deleteDataCallback;
};
