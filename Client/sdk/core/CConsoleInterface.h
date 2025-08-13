/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CConsoleInterface.h
 *  PURPOSE:     Console interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CConsoleInterface
{
public:
    virtual void Echo(const char* szText) = 0;
    virtual void Print(const char* szText) = 0;
    virtual void Printf(const char* szFormat, ...) = 0;

    virtual void Clear() = 0;

    virtual bool IsEnabled() = 0;
    virtual void SetEnabled(bool bEnabled) = 0;

    virtual bool IsVisible() = 0;
    virtual void SetVisible(bool bVisible) = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual bool IsInputActive() = 0;
    virtual void ActivateInput() = 0;
};
