/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaVector3.h
*  PURPOSE:     Lua vector class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAVECTOR3_H
#define __CLUAVECTOR3_H

class CLuaVector3D : public CVector
{
public:

                            CLuaVector3D        ( void );
                            CLuaVector3D        ( const CVector & vector );
                            CLuaVector3D        ( float fX, float fY, float fZ );

                            ~CLuaVector3D       ( void );

    unsigned int            GetScriptID         ( void ) const                      { return m_uiScriptID; }

    static CLuaVector3D*    GetFromScriptID     ( unsigned int uiScriptID );

private:

    unsigned int    m_uiScriptID;

};

#endif
