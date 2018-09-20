/*********************************************************
 *
 *  Multi Theft Auto: San Andreas - Deathmatch
 *
 *  ml_base, External lua add-on module
 *
 *  Copyright © 2003-2018 MTA.  All Rights Reserved.
 *
 *  Grand Theft Auto is � 2002-2018 Rockstar North
 *
 *  THE FOLLOWING SOURCES ARE PART OF THE MULTI THEFT
 *  AUTO SOFTWARE DEVELOPMENT KIT AND ARE RELEASED AS
 *  OPEN SOURCE FILES. THESE FILES MAY BE USED AS LONG
 *  AS THE DEVELOPER AGREES TO THE LICENSE THAT IS
 *  PROVIDED WITH THIS PACKAGE.
 *
 *********************************************************/

#pragma once

extern "C"
{
    #include <lua.h>
}

class CLuaArgument
{
public:
    CLuaArgument(void);
    CLuaArgument(bool bBool);
    CLuaArgument(double dNumber);
    CLuaArgument(const char* szString);
    CLuaArgument(void* pUserData);
    CLuaArgument(const CLuaArgument& Argument);
    CLuaArgument(lua_State* luaVM, unsigned int uiArgument);
    ~CLuaArgument(void);

    const CLuaArgument& operator=(const CLuaArgument& Argument);
    bool                operator==(const CLuaArgument& Argument);
    bool                operator!=(const CLuaArgument& Argument);

    void Read(lua_State* luaVM, unsigned int uiArgument);
    void Push(lua_State* luaVM) const;

    int GetType(void) const { return m_iType; };

    bool        GetBoolean(void) const { return m_bBoolean; };
    lua_Number  GetNumber(void) const { return m_Number; };
    const char* GetString(void) const { return m_szString; };
    void*       GetLightUserData(void) const { return m_pLightUserData; };

private:
    int        m_iType;
    bool       m_bBoolean;
    lua_Number m_Number;
    char*      m_szString;
    void*      m_pLightUserData;
};
