/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CServerInterface.h
 *  PURPOSE:     Core server interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "net/CNetServer.h"
#include "CModManager.h"
#include <xml/CXML.h>

class CServerInterface
{
public:
    virtual CNetServer*  GetNetwork() = 0;
    virtual CModManager* GetModManager() = 0;
    virtual CXML*        GetXML() = 0;

    virtual const char* GetServerModPath() = 0;
    virtual SString     GetAbsolutePath(const char* szRelative) = 0;

    virtual void Printf(const char* szFormat, ...) = 0;
    virtual bool IsRequestingExit() = 0;

    // Clears input buffer
    virtual bool ClearInput() = 0;
    // Prints current input buffer on a new line, clears the input buffer and resets history selection
    virtual bool ResetInput() = 0;
};
