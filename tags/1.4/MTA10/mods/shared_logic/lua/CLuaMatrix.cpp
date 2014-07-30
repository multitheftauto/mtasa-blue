/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaMatrix.cpp
*  PURPOSE:     Lua matrix class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CLuaMatrix::CLuaMatrix ( void ) :
    CMatrix ()
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::MATRIX );
}

CLuaMatrix::CLuaMatrix ( const CMatrix& matrix ) :
    CMatrix ( matrix )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::MATRIX );
}

CLuaMatrix::~CLuaMatrix ( void )
{
    CIdArray::PushUniqueId ( this, EIdClass::MATRIX, m_uiScriptID );
    m_uiScriptID = INVALID_ARRAY_ID;
}

CLuaMatrix* CLuaMatrix::GetFromScriptID ( unsigned int uiScriptID )
{
    CLuaMatrix* pLuaMatrix = ( CLuaMatrix* ) CIdArray::FindEntry ( uiScriptID, EIdClass::MATRIX );
    return pLuaMatrix;
}