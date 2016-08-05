/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/mods/logic/lua/CLuaMatrix.h
*  PURPOSE:     Lua matrix class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

class CLuaMatrix : public CMatrix
{
public:

    CLuaMatrix ( void );
    CLuaMatrix ( const CMatrix& matrix );

    ~CLuaMatrix ( void );

    unsigned int       GetScriptID ( void ) const { return m_uiScriptID; }

    static CLuaMatrix* GetFromScriptID ( unsigned int uiScriptID );

private:
    unsigned int       m_uiScriptID;

};
