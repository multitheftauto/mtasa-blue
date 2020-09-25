/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/lua/CLuaMatrix.cpp
 *  PURPOSE:     Lua matrix class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaQuaternion::CLuaQuaternion() : CQuaternion()
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::QUATERNION);
}

CLuaQuaternion::CLuaQuaternion(const CQuaternion& matrix) : CQuaternion(matrix)
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::QUATERNION);
}

CLuaQuaternion::~CLuaQuaternion()
{
    CIdArray::PushUniqueId(this, EIdClass::QUATERNION, m_uiScriptID);
    m_uiScriptID = INVALID_ARRAY_ID;
}

CLuaQuaternion* CLuaQuaternion::GetFromScriptID(unsigned int uiScriptID)
{
    CLuaQuaternion* pLuaQuat = (CLuaQuaternion*)CIdArray::FindEntry(uiScriptID, EIdClass::QUATERNION);
    return pLuaQuat;
}
