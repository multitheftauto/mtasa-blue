/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CTrayIconInterface.h
 *  PURPOSE:     Tray icon interface class
 *
 *****************************************************************************/

#pragma once

enum eTrayIconType
{
    ICON_TYPE_DEFAULT,
    ICON_TYPE_INFO,
    ICON_TYPE_WARNING,
    ICON_TYPE_ERROR
};

class CTrayIconInterface
{
public:
    virtual bool DoesTrayIconExist() = 0;
    virtual bool CreateTrayIcon() = 0;
    virtual void DestroyTrayIcon() = 0;
    virtual bool CreateTrayBallon(SString strText, eTrayIconType trayIconType, bool useSound) = 0;
};
