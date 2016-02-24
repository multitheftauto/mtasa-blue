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
//  bool httpWrite ( string data [, int length] )
    SString strData;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strData );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResourceFile * file = pLuaMain->GetResourceFile();
            if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
            {
                CResourceHTMLItem * html = (CResourceHTMLItem *)file;
                unsigned long ulLength;
                if ( argStream.NextIsNumber () )
                    argStream.ReadNumber ( ulLength );
                else
                    ulLength = strData.length ();

                html->AppendToPageBuffer ( strData, ulLength );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "%s: Can only be used in HTML scripts", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
        } 
        else
            m_pScriptDebugging->LogError ( luaVM, lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
    }   
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpSetResponseHeader ( lua_State* luaVM )
{
//  bool httpSetResponseHeader ( string headerName, string headerValue )
    SString strHeaderName; SString strHeaderValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strHeaderName );
    argStream.ReadString ( strHeaderValue );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResourceFile * file = pLuaMain->GetResourceFile();
            if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
            {
                CResourceHTMLItem * html = (CResourceHTMLItem *)file;
                html->SetResponseHeader ( strHeaderName, strHeaderValue );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "%s: Can only be used in HTML scripts", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
        } 
        else
            m_pScriptDebugging->LogError ( luaVM, lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpSetResponseCookie ( lua_State* luaVM )
{
//  bool httpSetResponseCookie ( string cookieName, string cookieValue )
    SString strCookieName; SString strCookieValue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strCookieName );
    argStream.ReadString ( strCookieValue );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResourceFile * file = pLuaMain->GetResourceFile();
            if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
            {
                CResourceHTMLItem * html = (CResourceHTMLItem *)file;
                html->SetResponseCookie ( strCookieName, strCookieValue );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "%s: Can only be used in HTML scripts", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
        } 
        else
            m_pScriptDebugging->LogError ( luaVM, lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpSetResponseCode ( lua_State* luaVM )
{
//  bool httpSetResponseCode ( int code )
    unsigned int uiResponseCode;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( uiResponseCode );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResourceFile * file = pLuaMain->GetResourceFile();
            if ( file && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
            {
                CResourceHTMLItem * html = (CResourceHTMLItem *)file;
                html->SetResponseCode ( uiResponseCode );
                lua_pushboolean ( luaVM, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError ( luaVM, "%s: Can only be used in HTML scripts", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
        } 
        else
            m_pScriptDebugging->LogError ( luaVM, lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpClear ( lua_State* luaVM )
{
//  bool httpClear ( )

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
            m_pScriptDebugging->LogError ( luaVM, "%s: Can only be used in HTML scripts", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
    } 
    else
        m_pScriptDebugging->LogError ( luaVM, lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );   
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaHTTPDefs::httpRequestLogin ( lua_State* luaVM )
{
//  bool httpRequestLogin ( )
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
            m_pScriptDebugging->LogError ( luaVM, "%s: Can only be used in HTML scripts", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );
    } 
    else
        m_pScriptDebugging->LogError ( luaVM, lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );   
    lua_pushboolean ( luaVM, false );
    return 1;
}
