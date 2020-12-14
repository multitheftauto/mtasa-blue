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
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsShape.h"
#include "CLuaPhysicsCompoundShape.h"

CLuaPhysicsCompoundShape::CLuaPhysicsCompoundShape(CClientPhysics* pPhysics, int iInitialChildCapacity) : CLuaPhysicsShape(pPhysics, std::move(CLuaPhysicsSharedLogic::CreateCompound()))
{
}

CLuaPhysicsCompoundShape::~CLuaPhysicsCompoundShape()
{

}

void CLuaPhysicsCompoundShape::AddShape(std::shared_ptr<CLuaPhysicsShape> pShape, CVector vecPosition, CVector vecRotation)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    btTransform      transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
    pCompound->addChildShape(transform, pShape->GetBtShape());
    pCompound->recalculateLocalAabb();
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

bool CLuaPhysicsCompoundShape::GetChildShapeOffsets(int index, CVector& vecPosition, CVector& vecRotation)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    if (index >= 0 && index < pCompound->getNumChildShapes())
    {
        btTransform transform = pCompound->getChildTransform(index);
        CLuaPhysicsSharedLogic::GetPosition(transform, vecPosition);
        CLuaPhysicsSharedLogic::GetRotation(transform, vecRotation);
        return true;
    }
    return false;
}

bool CLuaPhysicsCompoundShape::SetChildShapeOffsets(int index, CVector& vecPosition, CVector& vecRotation)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    if (index >= 0 && index < pCompound->getNumChildShapes())
    {
        btTransform transform;
        transform.setIdentity();
        if (vecPosition.Length() == 0)
        {
            btTransform compoundTransform = pCompound->getChildTransform(index);
            CLuaPhysicsSharedLogic::GetPosition(transform, vecPosition);
        }
        if (vecRotation.Length() == 0)
        {
            btTransform compoundTransform = pCompound->getChildTransform(index);
            CLuaPhysicsSharedLogic::GetRotation(transform, vecRotation);
        }
        CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
        CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
        pCompound->updateChildTransform(index, transform);
        UpdateRigids();
        return true;
    }
    return false;
}
