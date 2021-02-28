/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsCompoundShape.cpp
 *  PURPOSE:     Lua physics compound shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsCompoundShape::CLuaPhysicsCompoundShape(CBulletPhysics* pPhysics, int iInitialChildCapacity)
    : CLuaPhysicsShape(pPhysics, std::move(CPhysicsSharedLogic::CreateCompound()))
{
}

CLuaPhysicsCompoundShape::~CLuaPhysicsCompoundShape()
{
}

void CLuaPhysicsCompoundShape::AddShape(CLuaPhysicsShape* pShape, CVector vecPosition, CVector vecRotation)
{
    btTransform      transform = btTransform::getIdentity();
    CPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CPhysicsSharedLogic::SetRotation(transform, vecRotation);
    GetBtShape()->addChildShape(transform, pShape->InternalGetBtShape());
    GetBtShape()->recalculateLocalAabb();
    m_vecChildShapes.push_back(pShape);
}

bool CLuaPhysicsCompoundShape::RemoveChildShape(int index)
{
    if (index >= 0 && index < GetBtShape()->getNumChildShapes())
    {
        GetBtShape()->removeChildShapeByIndex(index);
        m_vecChildShapes.erase(m_vecChildShapes.begin() + index);
        UpdateRigids();
        return true;
    }
    return false;
}

const CVector& CLuaPhysicsCompoundShape::GetChildShapePosition(int index)
{
    btTransform transform = GetBtShape()->getChildTransform(index);
    return CPhysicsSharedLogic::GetPosition(transform);
}

const CVector& CLuaPhysicsCompoundShape::GetChildShapeRotation(int index)
{
    btTransform transform = GetBtShape()->getChildTransform(index);
    return CPhysicsSharedLogic::GetRotation(transform);
}

void CLuaPhysicsCompoundShape::SetChildShapePosition(int index, const CVector& vecPosition)
{
    btTransform childOffset = GetBtShape()->getChildTransform(index);
    CPhysicsSharedLogic::SetPosition(childOffset, vecPosition);
    GetBtShape()->updateChildTransform(index, childOffset);
    UpdateRigids();
}

void CLuaPhysicsCompoundShape::SetChildShapeRotation(int index, const CVector& vecRotation)
{
    btTransform childOffset = GetBtShape()->getChildTransform(index);
    CPhysicsSharedLogic::SetRotation(childOffset, vecRotation);
    GetBtShape()->updateChildTransform(index, childOffset);
    UpdateRigids();
}
