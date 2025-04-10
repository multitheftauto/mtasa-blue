/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaVector4.cpp
 *  PURPOSE:     Lua vector4 class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaVector4.h"
#include "CVector4D.h"
#include "CIdArray.h"

CLuaVector4D::CLuaVector4D() : CVector4D()
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::VECTOR4);
}

CLuaVector4D::CLuaVector4D(float fX, float fY, float fZ, float fW) : CVector4D(fX, fY, fZ, fW)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::VECTOR4);
}

CLuaVector4D::CLuaVector4D(const CVector4D& vector) : CVector4D(vector)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::VECTOR4);
}

CLuaVector4D::~CLuaVector4D()
{
    CIdArray::PushUniqueId(this, EIdClass::VECTOR4, m_uiScriptID);
    m_uiScriptID = INVALID_ARRAY_ID;
}

CLuaVector4D* CLuaVector4D::GetFromScriptID(unsigned int uiScriptID)
{
    CLuaVector4D* pLuaVector = (CLuaVector4D*)CIdArray::FindEntry(uiScriptID, EIdClass::VECTOR4);
    return pLuaVector;
}
