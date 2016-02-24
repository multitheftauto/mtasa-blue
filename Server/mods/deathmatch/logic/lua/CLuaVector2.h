/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaVector2.h
*  PURPOSE:     Lua vector class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAVECTOR2_H
#define __CLUAVECTOR2_H

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

#endif
