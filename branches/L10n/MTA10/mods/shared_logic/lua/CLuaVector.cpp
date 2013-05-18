/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaVector.cpp
*  PURPOSE:     Lua vector class
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CLuaVector3D::CLuaVector3D ( void ) :
    CVector ()
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::VECTOR );
}

CLuaVector3D::CLuaVector3D ( float fX, float fY, float fZ ) :
    CVector ( fX, fY, fZ )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::VECTOR );
}

CLuaVector3D::CLuaVector3D ( const CVector & vector ) :
    CVector ( vector )
{
    m_uiScriptID = CIdArray::PopUniqueId ( this, EIdClass::VECTOR );
}

CLuaVector3D::~CLuaVector3D ( void )
{
    CIdArray::PushUniqueId ( this, EIdClass::VECTOR, m_uiScriptID );
    m_uiScriptID = INVALID_ARRAY_ID;
}

CLuaVector3D* CLuaVector3D::GetFromScriptID ( unsigned int uiScriptID )
{
    CLuaVector3D* pLuaVector = ( CLuaVector3D* ) CIdArray::FindEntry ( uiScriptID, EIdClass::VECTOR );
    return pLuaVector;
}