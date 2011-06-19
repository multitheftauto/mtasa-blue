/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10/mods/shared_logic/lua/CLuaFunctionParseHelpers.cpp
*  PURPOSE:
*  DEVELOPERS:  Nobody knows
*
*****************************************************************************/

#include "StdInc.h"

//
// enum values <-> script strings
//

IMPLEMENT_ENUM_BEGIN( eLuaType )
    ADD_ENUM ( LUA_TNONE,           "none" )
    ADD_ENUM ( LUA_TNIL,            "nil" )
    ADD_ENUM ( LUA_TBOOLEAN,        "boolean" )
    ADD_ENUM ( LUA_TLIGHTUSERDATA,  "lightuserdata" )
    ADD_ENUM ( LUA_TNUMBER,         "number" )
    ADD_ENUM ( LUA_TSTRING,         "string" )
    ADD_ENUM ( LUA_TTABLE,          "table" )
    ADD_ENUM ( LUA_TFUNCTION,       "function" )
    ADD_ENUM ( LUA_TUSERDATA,       "userdata" )
    ADD_ENUM ( LUA_TTHREAD,         "thread" )
IMPLEMENT_ENUM_END( "lua-type" )


IMPLEMENT_ENUM_BEGIN( CGUIVerticalAlign )
    ADD_ENUM ( CGUI_ALIGN_TOP,              "top" )
    ADD_ENUM ( CGUI_ALIGN_BOTTOM,           "bottom" )
    ADD_ENUM ( CGUI_ALIGN_VERTICALCENTER,   "center" )
IMPLEMENT_ENUM_END( "vertical-align" )


IMPLEMENT_ENUM_BEGIN( CGUIHorizontalAlign )
    ADD_ENUM ( CGUI_ALIGN_LEFT,             "left" )
    ADD_ENUM ( CGUI_ALIGN_RIGHT,            "right" )
    ADD_ENUM ( CGUI_ALIGN_HORIZONTALCENTER, "center" )
IMPLEMENT_ENUM_END( "horizontal-align" )


IMPLEMENT_ENUM_BEGIN( eInputMode )
    ADD_ENUM ( INPUTMODE_ALLOW_BINDS,       "allow_binds" )
    ADD_ENUM ( INPUTMODE_NO_BINDS,          "no_binds" )
    ADD_ENUM ( INPUTMODE_NO_BINDS_ON_EDIT,  "no_binds_when_editing" )
IMPLEMENT_ENUM_END( "input-mode" )


IMPLEMENT_ENUM_BEGIN( eAccessType )
    ADD_ENUM ( ACCESS_PUBLIC,               "public" )
    ADD_ENUM ( ACCESS_PRIVATE,              "private" )
IMPLEMENT_ENUM_END( "access-type" )


IMPLEMENT_ENUM_BEGIN( eDXHorizontalAlign )
    ADD_ENUM ( DX_ALIGN_LEFT,           "left" )
    ADD_ENUM ( DX_ALIGN_HCENTER,        "center" )
    ADD_ENUM ( DX_ALIGN_RIGHT,          "right" )
IMPLEMENT_ENUM_END( "horizontal-align" )


IMPLEMENT_ENUM_BEGIN( eDXVerticalAlign )
    ADD_ENUM ( DX_ALIGN_TOP,            "top" )
    ADD_ENUM ( DX_ALIGN_VCENTER,        "center" )
    ADD_ENUM ( DX_ALIGN_BOTTOM,         "bottom" )
IMPLEMENT_ENUM_END( "vertical-align" )



//
// Reading mixed types
//

//
// DxFont/string
//
bool MixedReadDxFontString ( CScriptArgReader& argStream, SString& strFontName, const char* szDefaultFontName, CClientDxFont*& pDxFontElement )
{
    pDxFontElement = NULL;
    if ( argStream.NextIsString () || argStream.NextIsNone () )
        return argStream.ReadString ( strFontName, szDefaultFontName );
    else
        return argStream.ReadUserData ( pDxFontElement );
}


//
// GuiFont/string
//
bool MixedReadGuiFontString ( CScriptArgReader& argStream, SString& strFontName, const char* szDefaultFontName, CClientGuiFont*& pGuiFontElement )
{
    pGuiFontElement = NULL;
    if ( argStream.NextIsString () || argStream.NextIsNone () )
        return argStream.ReadString ( strFontName, szDefaultFontName );
    else
        return argStream.ReadUserData ( pGuiFontElement );
}


//
// Material/string
//
bool MixedReadMaterialString ( CScriptArgReader& argStream, CClientMaterial*& pMaterialElement )
{
    pMaterialElement = NULL;
    if ( !argStream.NextIsString () )
        return argStream.ReadUserData ( pMaterialElement );
    else
    {
        SString strFilePath;
        argStream.ReadString ( strFilePath );

        // If no element, auto find/create one
        CLuaMain* pLuaMain = g_pClientGame->GetLuaManager ()->GetVirtualMachine ( argStream.m_luaVM );
        CResource* pParentResource = pLuaMain ? pLuaMain->GetResource () : NULL;
        if ( pParentResource )
        {
            CResource* pFileResource = pParentResource;
            SString strPath, strMetaPath;
            if ( CResourceManager::ParseResourcePathInput( strFilePath, pFileResource, strPath, strMetaPath ) )
            {
                SString strUniqueName = SString ( "%s*%s*%s", pParentResource->GetName (), pFileResource->GetName (), strMetaPath.c_str () ).Replace ( "\\", "/" );
                pMaterialElement = g_pClientGame->GetManager ()->GetRenderElementManager ()->FindAutoTexture ( strPath, strUniqueName );

                // Check if brand new
                if ( pMaterialElement && !pMaterialElement->GetParent () )
                {
                    // Make it a child of the resource's file root ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                    pMaterialElement->SetParent ( pParentResource->GetResourceDynamicEntity() );
                }
            }
        }
        return pMaterialElement != NULL;
    }
}
