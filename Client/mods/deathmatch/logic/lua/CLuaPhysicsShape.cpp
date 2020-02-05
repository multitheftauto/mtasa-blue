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
#include "CLuaPhysicsRigidBodyManager.h"
#include "CLuaPhysicsStaticCollisionManager.h"
#include "CLuaPhysicsConstraintManager.h"
#include "CLuaPhysicsShapeManager.h"
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

void CLuaPhysicsShape::FinalizeInitialization(btCollisionShape* pShape)
{
    //assert(m_pBtShape == nullptr);
    pShape->setUserPointer((void*)this);
    pShape->setUserIndex(3);
    m_pBtShape = pShape;
}

btBoxShape* CLuaPhysicsShape::InitializeWithBox(CVector& half)
{
    btBoxShape* boxCollisionShape = CLuaPhysicsSharedLogic::CreateBox(half);
    FinalizeInitialization(boxCollisionShape);
    return boxCollisionShape;
}

btSphereShape* CLuaPhysicsShape::InitializeWithSphere(float fRadius)
{
    btSphereShape* sphereCollisionShape = CLuaPhysicsSharedLogic::CreateSphere(fRadius);
    FinalizeInitialization(sphereCollisionShape);
    return sphereCollisionShape;
}

btCapsuleShape* CLuaPhysicsShape::InitializeWithCapsule(float fRadius, float fHeight)
{
    btCapsuleShape* capsuleCollisionShape = CLuaPhysicsSharedLogic::CreateCapsule(fRadius, fHeight);
    FinalizeInitialization(capsuleCollisionShape);
    return capsuleCollisionShape;
}

btConeShape* CLuaPhysicsShape::InitializeWithCone(float fRadius, float fHeight)
{
    btConeShape* coneCollisionShape = CLuaPhysicsSharedLogic::CreateCone(fRadius, fHeight);
    FinalizeInitialization(coneCollisionShape);
    return coneCollisionShape;
}

btCylinderShape* CLuaPhysicsShape::InitializeWithCylinder(CVector& half)
{
    btCylinderShape* cylinderCollisionShape = CLuaPhysicsSharedLogic::CreateCylinder(half);
    FinalizeInitialization(cylinderCollisionShape);
    return cylinderCollisionShape;
}

btCompoundShape* CLuaPhysicsShape::InitializeWithCompound(int initialChildCapacity)
{
    btCompoundShape* pCompoundShape = new btCompoundShape(true);
    FinalizeInitialization(pCompoundShape);
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


btBvhTriangleMeshShape* CLuaPhysicsShape::InitializeWithTriangleMesh(std::vector<CVector>& vecIndices)
{
    btBvhTriangleMeshShape* trimeshShape = CLuaPhysicsSharedLogic::CreateTriangleMesh(vecIndices);
    trimeshShape->setUserPointer((void*)this);
    trimeshShape->setUserIndex(1);
    return trimeshShape;
}

heightfieldTerrainShape* CLuaPhysicsShape::InitializeWithHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData)
{
    heightfieldTerrainShape* pHeightfieldTerrain = CLuaPhysicsSharedLogic::CreateHeightfieldTerrain(iSizeX, iSizeY, vecHeightData);
    pHeightfieldTerrain->pHeightfieldTerrainShape->setUserPointer((void*)this);
    pHeightfieldTerrain->pHeightfieldTerrainShape->setUserIndex(1);
    return pHeightfieldTerrain;
}

bool CLuaPhysicsShape::SetSize(CVector size)
{
    if (m_pBtShape->getShapeType() == BOX_SHAPE_PROXYTYPE)
    {
        btConvexInternalShape* pInternalShape = (btConvexInternalShape*)m_pBtShape;
        pInternalShape->setImplicitShapeDimensions(reinterpret_cast<btVector3&>(size));
        UpdateRigids();
        return true;
    }
    return false;
}

bool CLuaPhysicsShape::GetSize(CVector& size)
{
    if (m_pBtShape->getShapeType() == BOX_SHAPE_PROXYTYPE)
    {
        btConvexInternalShape* pInternalShape = (btConvexInternalShape*)m_pBtShape;
        const btVector3        pSize = pInternalShape->getImplicitShapeDimensions();
        size.fX = pSize.getX();
        size.fY = pSize.getY();
        size.fZ = pSize.getZ();
        return true;
    }
    return false;
}

bool CLuaPhysicsShape::SetScale(CVector scale)
{
    m_pBtShape->setLocalScaling(reinterpret_cast<btVector3&>(scale));
    UpdateRigids();
    return true;
}

bool CLuaPhysicsShape::GetScale(CVector& scale)
{
    btVector3 btScale = m_pBtShape->getLocalScaling();
    scale = reinterpret_cast<CVector&>(btScale);
    return true;
}

bool CLuaPhysicsShape::GetBoundingBox(CVector& vecMin, CVector& vecMax)
{
    btTransform transform;
    transform.setIdentity();
    btVector3 min, max;
    m_pBtShape->getAabb(transform, min, max);
    vecMin = reinterpret_cast<CVector&>(min);
    vecMax = reinterpret_cast<CVector&>(max);
    return true;
}

bool CLuaPhysicsShape::GetBoundingSphere(CVector& vecCenter, float& fRadius)
{
    btTransform transform;
    transform.setIdentity();
    btVector3 center;
    btScalar  radius;
    m_pBtShape->getBoundingSphere(center, radius);
    fRadius = radius;
    vecCenter = reinterpret_cast<CVector&>(center);
    return true;
}

const char* CLuaPhysicsShape::GetType()
{
    return CLuaPhysicsSharedLogic::GetShapeName(m_pBtShape);
}

void CLuaPhysicsShape::UpdateRigids()
{
    for (int i = 0; i < GetRigidBodyNum(); i++)
    {
        GetRigidBody(i)->UpdateAABB();
        GetRigidBody(i)->Activate();
        GetRigidBody(i)->ApplyForce(CVector(0, 0, 0), CVector(0, 0, 0.01));
    }
}
