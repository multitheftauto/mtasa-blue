/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaVector2.cpp
 *  PURPOSE:     Lua vector2 class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaVector2.h"
#include "CVector2D.h"
#include "CIdArray.h"

CLuaVector2D::CLuaVector2D() : CVector2D()
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::VECTOR2);
}

CLuaVector2D::CLuaVector2D(float fX, float fY) : CVector2D(fX, fY)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::VECTOR2);
}

CLuaVector2D::CLuaVector2D(const CVector2D& vector) : CVector2D(vector)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::VECTOR2);
}

CLuaVector2D::~CLuaVector2D()
{
    CIdArray::PushUniqueId(this, EIdClass::VECTOR2, m_uiScriptID);
    m_uiScriptID = INVALID_ARRAY_ID;
}

CLuaVector2D* CLuaVector2D::GetFromScriptID(unsigned int uiScriptID)
{
    CLuaVector2D* pLuaVector = (CLuaVector2D*)CIdArray::FindEntry(uiScriptID, EIdClass::VECTOR2);
    return pLuaVector;
}
