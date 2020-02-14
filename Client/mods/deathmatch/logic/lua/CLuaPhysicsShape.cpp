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

CLuaPhysicsShape::CLuaPhysicsShape(CClientPhysics* pPhysics)
{
    m_pPhysics = pPhysics;
    m_uiScriptID = CIdArray::PopUniqueId(this, EIdClass::SHAPE);
    m_pBtShape = nullptr;
}

CLuaPhysicsShape::~CLuaPhysicsShape()
{
    for (int i = m_vecRigidBodyList.size() - 1; i >= 0; i--)
    {
        m_pPhysics->DestroyRigidBody(m_vecRigidBodyList[i]);
    }
    for (int i = m_vecStaticCollisions.size() - 1; i >= 0; i--)
    {
        m_pPhysics->DestroyStaticCollision(m_vecStaticCollisions[i]);
    }
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

void CLuaPhysicsShape::AddShape(CLuaPhysicsShape* pShape, CVector vecPosition, CVector vecRotation)
{
    btCompoundShape* pCompound = (btCompoundShape*)m_pBtShape;
    btTransform      transform;
    transform.setIdentity();
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
    pCompound->addChildShape(transform, pShape->GetBtShape());
    pCompound->recalculateLocalAabb();
}

std::vector<CLuaPhysicsShape*> CLuaPhysicsShape::GetChildShapes()
{
    btCompoundShape*               pCompound = (btCompoundShape*)m_pBtShape;
    std::vector<CLuaPhysicsShape*> vecChildShapes;
    btCollisionShape*              pCollisionShape;
    for (int i = 0; i < pCompound->getNumChildShapes(); i++)
    {
        pCollisionShape = pCompound->getChildShape(i);
        vecChildShapes.push_back((CLuaPhysicsShape*)pCollisionShape->getUserPointer());
    }
    return vecChildShapes;
}

bool CLuaPhysicsShape::RemoveChildShape(int index)
{
    btCompoundShape* pCompound = (btCompoundShape*)m_pBtShape;
    if (index >= 0 && index < pCompound->getNumChildShapes())
    {
        pCompound->removeChildShapeByIndex(index);
        UpdateRigids();
        return true;
    }
    return false;
}

bool CLuaPhysicsShape::GetChildShapeOffsets(int index, CVector& vecPosition, CVector& vecRotation)
{
    btCompoundShape* pCompound = (btCompoundShape*)m_pBtShape;
    if (index >= 0 && index < pCompound->getNumChildShapes())
    {
        btTransform transform = pCompound->getChildTransform(index);
        CLuaPhysicsSharedLogic::GetPosition(transform, vecPosition);
        CLuaPhysicsSharedLogic::GetRotation(transform, vecRotation);
        return true;
    }
    return false;
}

bool CLuaPhysicsShape::SetChildShapeOffsets(int index, CVector& vecPosition, CVector& vecRotation)
{
    btCompoundShape* pCompound = (btCompoundShape*)m_pBtShape;
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

void CLuaPhysicsShape::FinalizeInitialization(btCollisionShape* pShape)
{
    if (pShape)
    {
        pShape->setUserPointer((void*)this);
        m_pBtShape = pShape;
    }
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
    FinalizeInitialization(pConvexHull);
    return pConvexHull;
}

btBvhTriangleMeshShape* CLuaPhysicsShape::InitializeWithTriangleMesh(std::vector<CVector>& vecIndices)
{
    btBvhTriangleMeshShape* pTriangleMeshShape = CLuaPhysicsSharedLogic::CreateTriangleMesh(vecIndices);
    FinalizeInitialization(pTriangleMeshShape);
    return pTriangleMeshShape;
}

heightfieldTerrainShape* CLuaPhysicsShape::InitializeWithHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData)
{
    heightfieldTerrainShape* pHeightfieldTerrain = CLuaPhysicsSharedLogic::CreateHeightfieldTerrain(iSizeX, iSizeY, vecHeightData);
    pHeightfieldTerrain->pHeightfieldTerrainShape->setUserPointer((void*)this);
    pHeightfieldTerrain->pHeightfieldTerrainShape->setUserIndex(1);
    return pHeightfieldTerrain;
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

bool CLuaPhysicsShape::SetSize(CVector vecSize)
{
    if (m_pBtShape->getShapeType() == BOX_SHAPE_PROXYTYPE)
    {
        btConvexInternalShape* pInternalShape = (btConvexInternalShape*)m_pBtShape;
        vecSize.fX -= pInternalShape->getMargin();
        vecSize.fY -= pInternalShape->getMargin();
        vecSize.fZ -= pInternalShape->getMargin();
        pInternalShape->setImplicitShapeDimensions(reinterpret_cast<btVector3&>(vecSize));
        UpdateRigids();
        return true;
    }
    return false;
}

bool CLuaPhysicsShape::GetSize(CVector& vecSize)
{
    if (m_pBtShape->getShapeType() == BOX_SHAPE_PROXYTYPE)
    {
        btConvexInternalShape* pInternalShape = (btConvexInternalShape*)m_pBtShape;
        const btVector3        pSize = pInternalShape->getImplicitShapeDimensions();
        vecSize.fX = pSize.getX();
        vecSize.fY = pSize.getY();
        vecSize.fZ = pSize.getZ();
        return true;
    }
    return false;
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

void CLuaPhysicsShape::UpdateRigids()
{
    for (auto const& rigidBody : m_vecRigidBodyList)
    {
        rigidBody->UpdateAABB();
        rigidBody->Activate();
        // rigidBody->ApplyForce(CVector(0, 0, 0), CVector(0, 0, 0.01));
    }
}
