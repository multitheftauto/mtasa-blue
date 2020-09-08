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

CLuaPhysicsCompoundShape::CLuaPhysicsCompoundShape(CClientPhysics* pPhysics, int iInitialChildCapacity) : CLuaPhysicsShape(pPhysics)
{
    std::unique_ptr<btCompoundShape> pCompoundShape = CLuaPhysicsSharedLogic::CreateCompound();
    Initialization(std::move(pCompoundShape));
}

CLuaPhysicsCompoundShape::~CLuaPhysicsCompoundShape()
{

}

void CLuaPhysicsCompoundShape::AddShape(CLuaPhysicsShape* pShape, CVector vecPosition, CVector vecRotation)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    btTransform      transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
    pCompound->addChildShape(transform, pShape->GetBtShape());
    pCompound->recalculateLocalAabb();
}

std::vector<CLuaPhysicsShape*> CLuaPhysicsCompoundShape::GetChildShapes()
{
    btCompoundShape*               pCompound = (btCompoundShape*)GetBtShape();
    std::vector<CLuaPhysicsShape*> vecChildShapes;
    btCollisionShape*              pCollisionShape;
    for (int i = 0; i < pCompound->getNumChildShapes(); i++)
    {
        pCollisionShape = pCompound->getChildShape(i);
        vecChildShapes.push_back((CLuaPhysicsShape*)pCollisionShape->getUserPointer());
    }
    return vecChildShapes;
}

bool CLuaPhysicsCompoundShape::RemoveChildShape(int index)
{
    btCompoundShape* pCompound = (btCompoundShape*)GetBtShape();
    if (index >= 0 && index < pCompound->getNumChildShapes())
    {
        pCompound->removeChildShapeByIndex(index);
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
