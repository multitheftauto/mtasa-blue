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
    m_pBtShape->setUserIndex(EIdClass::SHAPE);
}

CLuaPhysicsShape::CLuaPhysicsShape(CBulletPhysics* pPhysics, heightfieldTerrainShape* pHeightfieldTerrainShape) : CLuaPhysicsElement(pPhysics, EIdClass::SHAPE)
{
    m_vecHeightfieldData = std::move(pHeightfieldTerrainShape->data);
    m_pBtShape.reset(pHeightfieldTerrainShape->pHeightfieldTerrainShape);
    m_pBtShape->setUserPointer((void*)this);
    m_pBtShape->setUserIndex(EIdClass::SHAPE);
}

bool CLuaPhysicsShape::Destroy()
{
    GetPhysics()->DestroyElement(this);
    return true;
}

void CLuaPhysicsShape::Unlink()
{
    if (GetBtType() != BroadphaseNativeTypes::COMPOUND_SHAPE_PROXYTYPE)
    {
        // copy vector, "Destroy" method below modyfing original, may be made better in the future
        std::vector<CLuaPhysicsCompoundShape*>   shapes = m_vecCompoundShapes;

        for (auto const& pShape : shapes)
            pShape->RemoveChildShape(this);
    }
    std::vector<CLuaPhysicsRigidBody*>       bodies = m_vecRigidBodyList;
    std::vector<CLuaPhysicsStaticCollision*> staticCollisions = m_vecStaticCollisions;

    for (auto const& pRigidBody : bodies)
        pRigidBody->Destroy();
    for (auto const& pStaticCollision : staticCollisions)
        pStaticCollision->Destroy();
}

CLuaPhysicsShape::~CLuaPhysicsShape()
{
    assert(m_vecRigidBodyList.empty());
    assert(m_vecStaticCollisions.empty());
    assert(m_vecCompoundShapes.empty());
}

void CLuaPhysicsShape::AddRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    ElementLock lk(this);
    if (ListContains(m_vecRigidBodyList, pRigidBody))
        return;

    m_vecRigidBodyList.push_back(pRigidBody);
}

void CLuaPhysicsShape::RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody)
{
    ElementLock lk(this);
    if (!ListContains(m_vecRigidBodyList, pRigidBody))
        return;

    ListRemove(m_vecRigidBodyList, pRigidBody);
}

void CLuaPhysicsShape::AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    ElementLock lk(this);
    if (ListContains(m_vecStaticCollisions, pStaticCollision))
        return;

    m_vecStaticCollisions.push_back(pStaticCollision);
}

void CLuaPhysicsShape::RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision)
{
    ElementLock lk(this);
    if (!ListContains(m_vecStaticCollisions, pStaticCollision))
        return;

    ListRemove(m_vecStaticCollisions, pStaticCollision);
}


void CLuaPhysicsShape::AddCompoundShape(CLuaPhysicsCompoundShape* pCompoundShape)
{
    ElementLock lk(this);
    if (ListContains(m_vecCompoundShapes, pCompoundShape))
        return;

    m_vecCompoundShapes.push_back(pCompoundShape);
}

void CLuaPhysicsShape::RemoveCompoundShape(CLuaPhysicsCompoundShape* pCompoundShape)
{
    ElementLock lk(this);
    if (!ListContains(m_vecCompoundShapes, pCompoundShape))
        return;

    ListRemove(m_vecCompoundShapes, pCompoundShape);
}

void CLuaPhysicsShape::GetMargin(float& fMargin)
{
    ElementLock lk(this);
    fMargin = m_pBtShape->getMargin();
}

bool CLuaPhysicsShape::SetScale(CVector scale)
{
    ElementLock lk(this);
    m_pBtShape->setLocalScaling(scale);
    UpdateRigids();
    return true;
}

const CVector& CLuaPhysicsShape::GetScale()
{
    ElementLock lk(this);
    btVector3 btScale = m_pBtShape->getLocalScaling();
    return btScale;
}

SBoundingBox CLuaPhysicsShape::GetBoundingBox()
{
    ElementLock lk(this);
    btTransform transform = btTransform::getIdentity();
    btVector3   min, max;
    m_pBtShape->getAabb(transform, min, max);
    SBoundingBox sBox = SBoundingBox();
    sBox.vecMin = min;
    sBox.vecMax = max;
    return sBox;
}

SBoundingSphere CLuaPhysicsShape::GetBoundingSphere()
{
    ElementLock lk(this);
    btVector3 center;
    btScalar  radius;
    m_pBtShape->getBoundingSphere(center, radius);
    SBoundingSphere sSphere = SBoundingSphere();
    sSphere.fRadius = radius;
    sSphere.vecCenter = center;
    return sSphere;
}

BroadphaseNativeTypes CLuaPhysicsShape::GetBtType() const
{
    ElementLock lk(this);
    return (BroadphaseNativeTypes)m_pBtShape->getShapeType();
}

const char* CLuaPhysicsShape::GetBtName()
{
    ElementLock lk(this);
    return CPhysicsSharedLogic::GetShapeName(GetBtShape());
}

// Call after shape change, makes all rigid bodies update their position
void CLuaPhysicsShape::UpdateRigids()
{
    ElementLock lk(this);
    for (auto const& rigidBody : m_vecRigidBodyList)
    {
        rigidBody->NeedsAABBUpdate();
        rigidBody->NeedsActivation();
        // rigidBody->ApplyForce(CVector{0, 0, 0}, CVector(0, 0, 0.01));
    }
}

// Return true fo shapes that can be used as rigid body. Checks compatibility of that shape for rigidbody purpose.
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
