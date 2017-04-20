/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/mods/logic/luadefs/CLuaCryptDefs.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "luadefs/CLuaDefs.h"

class CLuaCryptDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );

    LUA_DECLARE ( Md5 );
    LUA_DECLARE ( Sha256 );
    LUA_DECLARE ( Hash );
    LUA_DECLARE ( TeaEncode );
    LUA_DECLARE ( TeaDecode );
    LUA_DECLARE ( Base64encode );
    LUA_DECLARE ( Base64decode );
    LUA_DECLARE(PasswordHash);
    LUA_DECLARE(PasswordVerify);
};
