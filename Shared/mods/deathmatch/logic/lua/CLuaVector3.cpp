/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaVector3.cpp
 *  PURPOSE:     Lua vector3 class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaVector3.h"
#include "CVector.h"
#include "CIdArray.h"

CLuaVector3D::CLuaVector3D() : CVector()
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::VECTOR3);
}

CLuaVector3D::CLuaVector3D(float fX, float fY, float fZ) : CVector(fX, fY, fZ)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::VECTOR3);
}

CLuaVector3D::CLuaVector3D(const CVector& vector) : CVector(vector)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::VECTOR3);
}

CLuaVector3D::~CLuaVector3D()
{
    CIdArray::PushUniqueId(this, EIdClass::VECTOR3, m_uiScriptID);
    m_uiScriptID = INVALID_ARRAY_ID;
}

CLuaVector3D* CLuaVector3D::GetFromScriptID(unsigned int uiScriptID)
{
    CLuaVector3D* pLuaVector = (CLuaVector3D*)CIdArray::FindEntry(uiScriptID, EIdClass::VECTOR3);
    return pLuaVector;
}
