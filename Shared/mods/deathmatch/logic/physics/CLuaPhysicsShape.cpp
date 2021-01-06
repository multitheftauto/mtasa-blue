/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShape.cpp
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsShape::CLuaPhysicsShape(CBulletPhysics* pPhysics, btCollisionShape* pShape) : CLuaPhysicsElement(pPhysics, EIdClass::SHAPE)
{
    m_pBtShape.reset(pShape);
    m_pBtShape->setUserPointer((void*)this);
}

CLuaPhysicsShape::CLuaPhysicsShape(CBulletPhysics* pPhysics, heightfieldTerrainShape* pHeightfieldTerrainShape) : CLuaPhysicsElement(pPhysics, EIdClass::SHAPE)
{
    m_vecHeightfieldData = std::move(pHeightfieldTerrainShape->data);
    m_pBtShape.reset(pHeightfieldTerrainShape->pHeightfieldTerrainShape);
    m_pBtShape->setUserPointer((void*)this);
}

void CLuaPhysicsShape::Unlink()
{
    if (GetBtType() == BroadphaseNativeTypes::COMPOUND_SHAPE_PROXYTYPE)
    {
    }

    for (auto const& pRigidBody : GetRigidBodies())
        pRigidBody->Destroy();
    for (auto const& pStaticCollision : GetStaticCollisions())
        pStaticCollision->Destroy();

    m_vecRigidBodyList.clear();
    m_vecStaticCollisions.clear();
}

CLuaPhysicsShape::~CLuaPhysicsShape()
{
}

void CLuaPhysicsShape::AddRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    if (ListContains(m_vecRigidBodyList, pRigidBody))
        return;

    m_vecRigidBodyList.push_back(pRigidBody);
}

void CLuaPhysicsShape::RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    if (!ListContains(m_vecRigidBodyList, pRigidBody))
        return;

    ListRemove(m_vecRigidBodyList, pRigidBody);
}

void CLuaPhysicsShape::AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    if (ListContains(m_vecStaticCollisions, pStaticCollision))
        return;

    m_vecStaticCollisions.push_back(pStaticCollision);
}

void CLuaPhysicsShape::RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    if (!ListContains(m_vecStaticCollisions, pStaticCollision))
        return;

    ListRemove(m_vecStaticCollisions, pStaticCollision);
}

void CLuaPhysicsShape::GetMargin(float& fMargin)
{
    fMargin = m_pBtShape->getMargin();
}

bool CLuaPhysicsShape::SetScale(CVector scale)
{
    m_pBtShape->setLocalScaling(scale);
    UpdateRigids();
    return true;
}

const CVector& CLuaPhysicsShape::GetScale()
{
    btVector3 btScale = m_pBtShape->getLocalScaling();
    return btScale;
}

bool CLuaPhysicsShape::GetBoundingBox(CVector& vecMin, CVector& vecMax)
{
    btTransform transform = btTransform::getIdentity();
    btVector3   min, max;
    m_pBtShape->getAabb(transform, min, max);
    vecMin = min;
    vecMax = max;
    return true;
}

bool CLuaPhysicsShape::GetBoundingSphere(CVector& vecCenter, float& fRadius)
{
    btVector3 center;
    btScalar  radius;
    m_pBtShape->getBoundingSphere(center, radius);
    fRadius = radius;
    vecCenter = center;
    return true;
}

BroadphaseNativeTypes CLuaPhysicsShape::GetBtType() const
{
    return (BroadphaseNativeTypes)m_pBtShape->getShapeType();
}

const char* CLuaPhysicsShape::GetBtName()
{
    return CLuaPhysicsSharedLogic::GetShapeName(GetBtShape());
}

// Call after shape change, makes all rigid bodies update their position
void CLuaPhysicsShape::UpdateRigids()
{
    for (auto const& rigidBody : m_vecRigidBodyList)
    {
        rigidBody->NeedsAABBUpdate();
        rigidBody->NeedsActivation();
        // rigidBody->ApplyForce(CVector(0, 0, 0), CVector(0, 0, 0.01));
    }
}

// Return true fo shapes that can be used as rigid body.
bool CLuaPhysicsShape::SupportRigidBody() const
{
    switch (GetBtType())
    {
        case BroadphaseNativeTypes::TERRAIN_SHAPE_PROXYTYPE:
        case BroadphaseNativeTypes::TRIANGLE_MESH_SHAPE_PROXYTYPE:
            return false;
    }
    return true;
}
