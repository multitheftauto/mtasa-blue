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
    : CLuaPhysicsShape(pPhysics, std::move(CPhysicsSharedLogic::CreateCompound(iInitialChildCapacity)))
{
}

CLuaPhysicsCompoundShape::~CLuaPhysicsCompoundShape()
{
    for (const auto& pShape : m_vecChildShapes)
        pShape->RemoveCompoundShape(this);
}

void CLuaPhysicsCompoundShape::AddShape(CLuaPhysicsShape* pShape)
{
    //ConvertDegreesToRadians(vecRotation);
    //ConvertRadiansToDegrees(vecRotation);
    GetBtShape()->addChildShape(btTransform::getIdentity(), pShape->InternalGetBtShape());
    pShape->AddCompoundShape(this);
    m_vecChildShapes.push_back(pShape);
}

bool CLuaPhysicsCompoundShape::RemoveChildShape(CLuaPhysicsShape* pShape)
{
    bool bUpdate = false;
    for (;;)
    {
        if (m_vecChildShapes.empty())
            break;

        auto result = std::find(std::begin(m_vecChildShapes), std::end(m_vecChildShapes), pShape);
        if (result == m_vecChildShapes.end())
            break;

        (*result)->RemoveCompoundShape(this);
        m_vecChildShapes.erase(result);
        bUpdate = true;
    }

    if (bUpdate)
    {
        GetBtShape()->recalculateLocalAabb();
        UpdateRigids();
    }
    return bUpdate;
}

bool CLuaPhysicsCompoundShape::RemoveChildShape(int index)
{
    if (index >= 0 && index < GetBtShape()->getNumChildShapes())
    {
        GetBtShape()->removeChildShapeByIndex(index);
        m_vecChildShapes[index]->RemoveCompoundShape(this);
        m_vecChildShapes.erase(m_vecChildShapes.begin() + index);
        GetBtShape()->recalculateLocalAabb();
        UpdateRigids();
        return true;
    }
    return false;
}

void CLuaPhysicsCompoundShape::SetChildShapePosition(int index, const CVector vecPosition)
{
    btTransform childOffset = GetBtShape()->getChildTransform(index);
    CPhysicsSharedLogic::SetPosition(childOffset, vecPosition);
    GetBtShape()->updateChildTransform(index, childOffset);
    UpdateRigids();
}

void CLuaPhysicsCompoundShape::SetChildShapeRotation(int index, const CVector vecRotation)
{
    btTransform childOffset = GetBtShape()->getChildTransform(index);
    CPhysicsSharedLogic::SetRotation(childOffset, vecRotation);
    GetBtShape()->updateChildTransform(index, childOffset);
    UpdateRigids();
}

CVector CLuaPhysicsCompoundShape::GetChildShapePosition(int index)
{
    const btTransform& transform = GetBtShape()->getChildTransform(index);
    return CPhysicsSharedLogic::GetPosition(transform);
}

CVector CLuaPhysicsCompoundShape::GetChildShapeRotation(int index)
{
    const btTransform& transform = GetBtShape()->getChildTransform(index);
    CVector            vecRotation = CPhysicsSharedLogic::GetRotation(transform);
    //ConvertDegreesToRadians(vecRotation);
    //ConvertRadiansToDegrees(vecRotation);
    return vecRotation;
}
