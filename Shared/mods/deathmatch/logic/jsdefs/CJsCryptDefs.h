/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CJsCryptDefs.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "jsdefs/CJsDefs.h"

class CJsCryptDefs : public CJsDefs
{
public:
    static void LoadFunctions();

#ifndef MTA_CLIENT
    static void Md5(CV8FunctionCallbackBase* callback);
    static void BCrypt(CV8FunctionCallbackBase* callback);
#endif
};
