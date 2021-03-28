/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaShared.cpp
 *
 *****************************************************************************/

#include "StdInc.h"

void CJsShared::AddFunctions()
{
    CJsCryptDefs::LoadFunctions();
    CJsUtilDefs::LoadFunctions();
}

void CJsShared::AddClasses()
{
    CJsVector2Defs::AddClass();
}
