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

enum class CTrayIconType
{
    Default,
    Info,
    Warning,
    Error
};

class CTrayIconInterface
{
public:
    virtual bool        DoesTrayIconExist       ( void ) = 0;
    virtual bool        CreateTrayIcon          ( void ) = 0;
    virtual bool        DestroyTrayIcon         ( void ) = 0;
    virtual bool        CreateTrayBallon        ( SString strText, CTrayIconType trayIconType, bool useSound ) = 0;
};
