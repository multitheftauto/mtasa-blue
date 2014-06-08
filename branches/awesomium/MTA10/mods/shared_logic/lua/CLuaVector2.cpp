/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaVector2.cpp
*  PURPOSE:     Lua vector2 class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CLuaVector2D::CLuaVector2D ( void ) :
    CVector2D ()
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::VECTOR2 );
}

CLuaVector2D::CLuaVector2D ( float fX, float fY ) :
    CVector2D ( fX, fY )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::VECTOR2 );
}

CLuaVector2D::CLuaVector2D ( const CVector2D & vector ) :
    CVector2D ( vector )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::VECTOR2 );
}

CLuaVector2D::~CLuaVector2D ( void )
{
    CIdArray::PushUniqueId ( this, EIdClass::VECTOR2, m_uiScriptID );
    m_uiScriptID = INVALID_ARRAY_ID;
}

CLuaVector2D* CLuaVector2D::GetFromScriptID ( unsigned int uiScriptID )
{
    CLuaVector2D* pLuaVector = ( CLuaVector2D* ) CIdArray::FindEntry ( uiScriptID, EIdClass::VECTOR2 );
    return pLuaVector;
}