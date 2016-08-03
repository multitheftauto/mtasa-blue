/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/mods/logic/lua/CLuaVector4.cpp
*  PURPOSE:     Lua vector4 class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CLuaVector4D : public CVector4D
{
public:

                            CLuaVector4D        ( void );
                            CLuaVector4D        ( const CVector4D & vector );
                            CLuaVector4D        ( float fX, float fY, float fZ, float fW );

                            ~CLuaVector4D       ( void );

    unsigned int            GetScriptID         ( void ) const                      { return m_uiScriptID; }

    static CLuaVector4D*    GetFromScriptID     ( unsigned int uiScriptID );

private:

    unsigned int    m_uiScriptID;

};
