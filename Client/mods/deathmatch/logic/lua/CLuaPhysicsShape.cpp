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
#include "CLuaPhysicsSharedLogic.h"
#include "bulletphysics3d/BulletCollision/CollisionShapes/btConvexPolyhedron.h"

CLuaPhysicsShape::CLuaPhysicsShape()
{
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::SHAPE);
}

CLuaPhysicsShape::~CLuaPhysicsShape()
{
    delete m_pBtShape;
    RemoveScriptID();
}

void CLuaPhysicsShape::RemoveScriptID()
{
    if (m_uiScriptID != INVALID_ARRAY_ID)
    {
        CIdArray::PushUniqueId(this, EIdClass::SHAPE, m_uiScriptID);
        m_uiScriptID = INVALID_ARRAY_ID;
    }
}

btBoxShape* CLuaPhysicsShape::InitializeWithBox(CVector& half)
{
    btBoxShape* boxCollisionShape = CLuaPhysicsSharedLogic::CreateBox(half);
    boxCollisionShape->setUserPointer((void*)this);
    boxCollisionShape->setUserIndex(3);
    m_pBtShape = boxCollisionShape;
    return boxCollisionShape;
}

btSphereShape* CLuaPhysicsShape::InitializeWithSphere(float fRadius)
{
    btSphereShape* sphereCollisionShape = CLuaPhysicsSharedLogic::CreateSphere(fRadius);
    sphereCollisionShape->setUserPointer((void*)this);
    sphereCollisionShape->setUserIndex(3);
    m_pBtShape = sphereCollisionShape;
    return sphereCollisionShape;
}

btCapsuleShape* CLuaPhysicsShape::InitializeWithCapsule(float fRadius, float fHeight)
{
    btCapsuleShape* capsuleCollisionShape = CLuaPhysicsSharedLogic::CreateCapsule(fRadius, fHeight);
    capsuleCollisionShape->setUserPointer((void*)this);
    capsuleCollisionShape->setUserIndex(3);
    m_pBtShape = capsuleCollisionShape;
    return capsuleCollisionShape;
}

btConeShape* CLuaPhysicsShape::InitializeWithCone(float fRadius, float fHeight)
{
    btConeShape* coneCollisionShape = CLuaPhysicsSharedLogic::CreateCone(fRadius, fHeight);
    coneCollisionShape->setUserPointer((void*)this);
    coneCollisionShape->setUserIndex(3);
    m_pBtShape = coneCollisionShape;
    return coneCollisionShape;
}

btCylinderShape* CLuaPhysicsShape::InitializeWithCylinder(CVector& half)
{
    btCylinderShape* cylinderCollisionShape = CLuaPhysicsSharedLogic::CreateCylinder(half);
    cylinderCollisionShape->setUserPointer((void*)this);
    cylinderCollisionShape->setUserIndex(3);
    m_pBtShape = cylinderCollisionShape;
    return cylinderCollisionShape;
}

btCompoundShape* CLuaPhysicsShape::InitializeWithCompound(int initialChildCapacity)
{
    btCompoundShape* pCompoundShape = new btCompoundShape(true);
    pCompoundShape->setUserPointer((void*)this);
    pCompoundShape->setUserIndex(3);
    m_pBtShape = pCompoundShape;
    return pCompoundShape;
}

btConvexHullShape* CLuaPhysicsShape::InitializeWithConvexHull(std::vector<CVector>& vecPoints)
{
    btConvexHullShape* pConvexHull = CLuaPhysicsSharedLogic::CreateConvexHull(vecPoints);
    pConvexHull->setUserPointer((void*)this);
    pConvexHull->setUserIndex(3);
    m_pBtShape = pConvexHull;
    return pConvexHull;
}

bool CLuaPhysicsShape::SetSize(CVector size)
{
    if (m_pBtShape->getShapeType() == BOX_SHAPE_PROXYTYPE)
    {
        btConvexInternalShape* pInternalShape = (btConvexInternalShape*)m_pBtShape;
        pInternalShape->setImplicitShapeDimensions(reinterpret_cast<btVector3&>(size));
        for (int i = 0; i < GetRigidBodyNum(); i++)
        {
            GetRigidBody(i)->UpdateAABB();
            GetRigidBody(i)->Activate();
            GetRigidBody(i)->ApplyForce(CVector(0, 0, 0), CVector(0, 0, 0.01));
        }
        return true;
    }
    return false;
}

const char* CLuaPhysicsShape::GetType()
{
    return CLuaPhysicsSharedLogic::GetShapeName(m_pBtShape);
}
