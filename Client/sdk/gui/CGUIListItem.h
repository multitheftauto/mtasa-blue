/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIListItem.h
 *  PURPOSE:     List widget item interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIStaticImage.h"
#include <string>

class CGUIListItem
{
public:
    virtual ~CGUIListItem(){};

    virtual std::string GetText() const = 0;
    virtual void        SetText(const char* pszText, const char* pszSortText = NULL) = 0;

    virtual void* GetData() const = 0;
    virtual void  SetData(void* pData, CGUICallback<void, void*> deleteDataCallback = NULL) = 0;
    virtual void  SetData(const char* pszData) = 0;

    virtual void SetDisabled(bool bDisabled) = 0;
    virtual void SetFont(const char* szFontName) = 0;
    virtual void SetImage(CGUIStaticImage* Image) = 0;

    virtual bool GetSelectedState() = 0;
    virtual void SetSelectedState(bool bState) = 0;

    virtual bool GetColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha) = 0;
    virtual void SetColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha) = 0;
};
