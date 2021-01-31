/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaUtilDefs.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"

class CJsUtilDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

#ifndef MTA_CLIENT
    static void GetTickCount(CV8FunctionCallbackBase* callback);
    static void Print(CV8FunctionCallbackBase* callback);
#endif
};
