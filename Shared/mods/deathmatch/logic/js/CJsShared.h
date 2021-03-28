/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CJsShared.h
 *
 *****************************************************************************/
#include "v8/CV8Base.h"
#include "jsdefs/CJsDefs.h"
#include "jsdefs/CJsClassConstructionInfo.h"
#include "jsdefs/CJsVector2Defs.h"
#include "jsdefs/CJsCryptDefs.h"
#include "jsdefs/CJsUtilDefs.h"

class CJsShared
{
public:
    static void AddFunctions();
    static void AddClasses();
};
