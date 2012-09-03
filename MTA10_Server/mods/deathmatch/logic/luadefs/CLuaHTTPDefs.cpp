/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaHTTPDefs.cpp
*  PURPOSE:     Lua HTTP webserver definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaHTTPDefs::LoadFunctions ( lua_State* luaVM )
{
    // Register these funcs for that VM
    lua_register ( luaVM, "httpWrite", CLuaHTTPDefs::httpWrite );
    lua_register ( luaVM, "httpSetResponseHeader", CLuaHTTPDefs::httpSetResponseHeader );
    lua_register ( luaVM, "httpSetResponseCookie", CLuaHTTPDefs::httpSetResponseCookie );
    lua_register ( luaVM, "httpSetResponseCode", CLuaHTTPDefs::httpSetResponseCode );
    lua_register ( luaVM, "httpClear", CLuaHTTPDefs::httpClear );
    lua_register ( luaVM, "httpRequestLogin", CLuaHTTPDefs::httpRequestLogin );
}


int CLuaHTTPDefs::httpWrite ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResourceFile * file = pLuaMain->GetResourceFile();
            if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
            {
                CResourceHTMLItem * html = (CResourceHTMLItem *)file;
                unsigned long length = 0;
                if ( lua_type ( luaVM, 2 ) == LUA_TNUMBER )
                    length = static_cast < unsigned long > ( lua_tonumber ( luaVM, 2 ) );
                else
                    length = lua_objlen ( luaVM, 1 );
                const char* szBuffer = lua_tolstring ( luaVM, 1, NULL );
                html->AppendToPageBuffer ( szBuffer, length );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "httpWrite: Can only be used in HTML scripts" );
        } 
        else
            m_pScriptDebugging->LogError ( luaVM, "httpWrite" );
    }   
    else
        m_pScriptDebugging->LogBadType ( luaVM, "httpWrite" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpSetResponseHeader ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResourceFile * file = pLuaMain->GetResourceFile();
            if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
            {
                CResourceHTMLItem * html = (CResourceHTMLItem *)file;
                const char* szHeaderName = lua_tostring ( luaVM, 1 );
                const char* szHeaderValue = lua_tostring ( luaVM, 2 );
                html->SetResponseHeader ( szHeaderName, szHeaderValue );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "httpSetResponseHeader: Can only be used in HTML scripts" );
        } 
        else
            m_pScriptDebugging->LogError ( luaVM, "httpSetResponseHeader" );
    }   
    else
        m_pScriptDebugging->LogBadType ( luaVM, "httpSetResponseHeader" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpSetResponseCookie ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING && lua_type ( luaVM, 2 ) == LUA_TSTRING )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResourceFile * file = pLuaMain->GetResourceFile();
            if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
            {
                CResourceHTMLItem * html = (CResourceHTMLItem *)file;
                const char* szHeaderName = lua_tostring ( luaVM, 1 );
                const char* szHeaderValue = lua_tostring ( luaVM, 2 );
                html->SetResponseCookie ( szHeaderName, szHeaderValue );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "httpSetResponseCookie: Can only be used in HTML scripts" );
        } 
        else
            m_pScriptDebugging->LogError ( luaVM, "httpSetResponseCookie" );
    }   
    else
        m_pScriptDebugging->LogBadType ( luaVM, "httpSetResponseCookie" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpSetResponseCode ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER  )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResourceFile * file = pLuaMain->GetResourceFile();
            if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
            {
                CResourceHTMLItem * html = (CResourceHTMLItem *)file;
                unsigned int responseCode = static_cast < unsigned int > ( lua_tonumber ( luaVM, 1 ) );
                html->SetResponseCode ( responseCode );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "httpSetResponseCode: Can only be used in HTML scripts" );
        } 
        else
            m_pScriptDebugging->LogError ( luaVM, "httpSetResponseCode" );
    }   
    else
        m_pScriptDebugging->LogBadType ( luaVM, "httpSetResponseCode" );
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpClear ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CResourceFile * file = pLuaMain->GetResourceFile();
        if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
        {
            CResourceHTMLItem * html = (CResourceHTMLItem *)file;
            html->ClearPageBuffer();
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "httpClear: Can only be used in HTML scripts" );
    } 
    else
        m_pScriptDebugging->LogError ( luaVM, "httpClear" );   
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpRequestLogin ( lua_State* luaVM )
{
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        CResourceFile * file = pLuaMain->GetResourceFile();
        if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
        {
            CResourceHTMLItem * html = (CResourceHTMLItem *)file;

            char szName[255];
            sprintf ( szName, "Basic realm=\"%s\"", m_pMainConfig->GetServerName ().c_str () );
            html->SetResponseHeader("WWW-Authenticate", szName);
            html->SetResponseCode ( 401 );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogError ( luaVM, "httpRequestLogin: Can only be used in HTML scripts" );
    } 
    else
        m_pScriptDebugging->LogError ( luaVM, "httpRequestLogin" );   
    lua_pushboolean ( luaVM, false );
    return 1;
}
