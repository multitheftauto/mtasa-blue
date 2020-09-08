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

CLuaPhysicsShape::CLuaPhysicsShape(CClientPhysics* pPhysics) : CLuaPhysicsElement(pPhysics, EIdClass::SHAPE)
{
    m_pBtShape = nullptr;
    m_heightfieldTerrainData = nullptr;
}

CLuaPhysicsShape::~CLuaPhysicsShape()
{
    if (m_pBtShape != nullptr)
        delete m_pBtShape;
}

void CLuaPhysicsShape::FinalizeInitialization(btCollisionShape* pShape)
{
    pShape->setUserPointer((void*)this);
    m_pBtShape = pShape;
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

bool CLuaPhysicsShape::SetRadius(float fRadius)
{
    float fHeight;
    switch (m_pBtShape->getShapeType())
    {
        case SPHERE_SHAPE_PROXYTYPE:
            ((btSphereShape*)m_pBtShape)->setUnscaledRadius(fRadius);
            UpdateRigids();
            return true;
        case CAPSULE_SHAPE_PROXYTYPE:
            GetHeight(fHeight);
            ((btCapsuleShape*)m_pBtShape)->setImplicitShapeDimensions(btVector3(fRadius, fHeight / 2, fRadius));
            UpdateRigids();
            return true;
        case CONE_SHAPE_PROXYTYPE:
            ((btConeShape*)m_pBtShape)->setRadius(fRadius);
            UpdateRigids();
            return true;
        case CYLINDER_SHAPE_PROXYTYPE:
            GetHeight(fHeight);
            ((btCylinderShape*)m_pBtShape)->setImplicitShapeDimensions(btVector3(fRadius, fHeight / 2, fRadius));
            UpdateRigids();
            return true;
    }
    return false;
}

bool CLuaPhysicsShape::GetRadius(float& fRadius)
{
    btVector3 implicityShapeDimension;
    switch (m_pBtShape->getShapeType())
    {
        case SPHERE_SHAPE_PROXYTYPE:
            fRadius = ((btSphereShape*)m_pBtShape)->getRadius();
            return true;
        case CAPSULE_SHAPE_PROXYTYPE:
            implicityShapeDimension = ((btSphereShape*)m_pBtShape)->getImplicitShapeDimensions();
            fRadius = implicityShapeDimension.getX();
            return true;
        case CONE_SHAPE_PROXYTYPE:
            fRadius = ((btConeShape*)m_pBtShape)->getRadius();
            return true;
        case CYLINDER_SHAPE_PROXYTYPE:
            fRadius = ((btCylinderShape*)m_pBtShape)->getRadius();
            return true;
    }
    return false;
}

bool CLuaPhysicsShape::SetHeight(float fHeight)
{
    if (m_pBtShape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
    {
        btCapsuleShape* pCapsule = (btCapsuleShape*)m_pBtShape;
        float           fRadius;
        GetRadius(fRadius);
        fHeight /= 2;
        pCapsule->setImplicitShapeDimensions(btVector3(fRadius, fHeight, fRadius));
        UpdateRigids();
        return true;
    }
    else if (m_pBtShape->getShapeType() == CONE_SHAPE_PROXYTYPE)
    {
        btConeShape* pCone = (btConeShape*)m_pBtShape;
        pCone->setHeight(fHeight);
        return true;
    }
    else if (m_pBtShape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
    {
        btCylinderShape* pCylinder = (btCylinderShape*)m_pBtShape;
        float            fRadius;
        GetRadius(fRadius);
        fRadius += pCylinder->getMargin();
        fHeight -= pCylinder->getMargin();
        pCylinder->setImplicitShapeDimensions(btVector3(fRadius, fHeight, fRadius));
        return true;
    }
    return false;
}

bool CLuaPhysicsShape::GetHeight(float& fHeight)
{
    if (m_pBtShape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
    {
        btCapsuleShape* pCapsule = (btCapsuleShape*)m_pBtShape;
        fHeight = pCapsule->getHalfHeight();
        fHeight *= 2;
        return true;
    }
    else if (m_pBtShape->getShapeType() == CONE_SHAPE_PROXYTYPE)
    {
        btConeShape* pCone = (btConeShape*)m_pBtShape;
        fHeight = pCone->getHeight();
        return true;
    }
    else if (m_pBtShape->getShapeType() == CYLINDER_SHAPE_PROXYTYPE)
    {
        btCylinderShape* pCylinder = (btCylinderShape*)m_pBtShape;
        fHeight = pCylinder->getImplicitShapeDimensions().getY();
        fHeight += pCylinder->getMargin();
        return true;
    }
    return false;
}

void CLuaPhysicsShape::GetMargin(float& fMargin)
{
    fMargin = m_pBtShape->getMargin();
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

BroadphaseNativeTypes CLuaPhysicsShape::GetType()
{
    return (BroadphaseNativeTypes)m_pBtShape->getShapeType();
}

const char* CLuaPhysicsShape::GetName()
{
    return CLuaPhysicsSharedLogic::GetShapeName(m_pBtShape);
}

// Call after shape change, makes all rigid bodies update their position
void CLuaPhysicsShape::UpdateRigids()
{
    for (auto const& rigidBody : m_vecRigidBodyList)
    {
        rigidBody->UpdateAABB();
        rigidBody->Activate();
        // rigidBody->ApplyForce(CVector(0, 0, 0), CVector(0, 0, 0.01));
    }
}
