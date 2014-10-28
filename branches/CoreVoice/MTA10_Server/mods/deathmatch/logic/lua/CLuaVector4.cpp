/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaVector4.cpp
*  PURPOSE:     Lua vector4 class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CLuaVector4D::CLuaVector4D ( void ) :
    CVector4D ()
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::VECTOR4 );
}

CLuaVector4D::CLuaVector4D ( float fX, float fY, float fZ, float fW ) :
    CVector4D ( fX, fY, fZ, fW )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::VECTOR4 );
}

CLuaVector4D::CLuaVector4D ( const CVector4D & vector ) :
    CVector4D ( vector )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::VECTOR4 );
}

CLuaVector4D::~CLuaVector4D ( void )
{
    CIdArray::PushUniqueId ( this, EIdClass::VECTOR4, m_uiScriptID );
    m_uiScriptID = INVALID_ARRAY_ID;
}

CLuaVector4D* CLuaVector4D::GetFromScriptID ( unsigned int uiScriptID )
{
    CLuaVector4D* pLuaVector = ( CLuaVector4D* ) CIdArray::FindEntry ( uiScriptID, EIdClass::VECTOR4 );
    return pLuaVector;
}