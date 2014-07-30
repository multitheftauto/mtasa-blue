/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaVector4.h
*  PURPOSE:     Lua vector class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAVECTOR4_H
#define __CLUAVECTOR4_H

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

#endif
