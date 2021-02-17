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
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    btTransform      transform = btTransform::getIdentity();
    CPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CPhysicsSharedLogic::SetRotation(transform, vecRotation);
    pCompound->addChildShape(transform, pShape->GetBtShape());
    pCompound->recalculateLocalAabb();
    m_vecChildShapes.push_back(pShape);
}

bool CLuaPhysicsCompoundShape::RemoveChildShape(int index)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    if (index >= 0 && index < pCompound->getNumChildShapes())
    {
        pCompound->removeChildShapeByIndex(index);
        m_vecChildShapes.erase(m_vecChildShapes.begin() + index);
        UpdateRigids();
        return true;
    }
    return false;
}

const CVector& CLuaPhysicsCompoundShape::GetChildShapePosition(int index)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    btTransform      transform = pCompound->getChildTransform(index);
    return CPhysicsSharedLogic::GetPosition(transform);
}

const CVector& CLuaPhysicsCompoundShape::GetChildShapeRotation(int index)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    btTransform      transform = pCompound->getChildTransform(index);
    return CPhysicsSharedLogic::GetRotation(transform);
}

void CLuaPhysicsCompoundShape::SetChildShapePosition(int index, const CVector& vecPosition)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    btTransform      childOffset = pCompound->getChildTransform(index);
    CPhysicsSharedLogic::SetPosition(childOffset, vecPosition);
    pCompound->updateChildTransform(index, childOffset);
    UpdateRigids();
}

void CLuaPhysicsCompoundShape::SetChildShapeRotation(int index, const CVector& vecRotation)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    btTransform      childOffset = pCompound->getChildTransform(index);
    CPhysicsSharedLogic::SetRotation(childOffset, vecRotation);
    pCompound->updateChildTransform(index, childOffset);
    UpdateRigids();
}
