/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/mods/logic/lua/CLuaVector2.h
*  PURPOSE:     Lua vector2 class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CLuaVector2D : public CVector2D
{
public:

                            CLuaVector2D        ( void );
                            CLuaVector2D        ( const CVector2D & vector );
                            CLuaVector2D        ( float fX, float fY );

                            ~CLuaVector2D       ( void );

    unsigned int            GetScriptID         ( void ) const                      { return m_uiScriptID; }

    static CLuaVector2D*    GetFromScriptID     ( unsigned int uiScriptID );

private:

    unsigned int    m_uiScriptID;

};
