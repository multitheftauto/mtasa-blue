/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaClasses.h
*  PURPOSE:     Lua general class functions
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

class CLuaClassDefs: public CLuaDefs
{
public:
    LUA_DECLARE ( Index );
    LUA_DECLARE ( NewIndex );
    LUA_DECLARE ( StaticNewIndex );
    LUA_DECLARE ( Call );

    LUA_DECLARE ( ReadOnly );
    LUA_DECLARE ( WriteOnly );

    LUA_DECLARE ( ToString );

    static const char*	GetObjectClass	    ( void* pObject );
    static const char*	GetResourceClass	( CResource* pResource );
    static const char*	GetTimerClass		( CLuaTimer* pTimer );
    static const char*	GetXmlNodeClass		( CXMLNode* pXmlNode );
    static const char*	GetACLClass	        ( CAccessControlList* pACL );
    static const char*	GetACLGroupClass    ( CAccessControlListGroup* pACLGroup );
    static const char*	GetAccountClass     ( CAccount* pAccount );
    static const char*	GetTextDisplayClass ( CTextDisplay* pDisplay );
    static const char*	GetTextItemClass    ( CTextItem* pItem );
    static const char*	GetBanClass         ( CBan* pBan );
    static const char*	GetQueryClass       ( CDbJobData* pJobData );
    static const char*	GetElementClass		( CElement* pElement );
};