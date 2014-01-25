/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaMatrix.h
*  PURPOSE:     Lua matrix class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAMATRIX_H
#define __CLUAMATRIX_H

class CLuaMatrix : public CMatrix
{
public:

                            CLuaMatrix          ( void );
                            CLuaMatrix          ( const CMatrix& matrix );

                            ~CLuaMatrix         ( void );

    unsigned int            GetScriptID         ( void ) const                      { return m_uiScriptID; }

    static CLuaMatrix*      GetFromScriptID     ( unsigned int uiScriptID );

private:

    unsigned int            m_uiScriptID;

};

#endif
