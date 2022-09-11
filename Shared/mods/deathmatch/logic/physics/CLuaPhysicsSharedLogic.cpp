/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShapeManager.cpp
 *  PURPOSE:     Physics shared logic with utility functions
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"


void CPhysicsSharedLogic::SetRotation(btTransform& transform, CVector vecRotation)
{
    btQuaternion quaternion = btQuaternion::getIdentity();
    EulerToQuaternion(ConvertVector(vecRotation), quaternion);
    transform.setRotation(quaternion);
}

CVector CPhysicsSharedLogic::GetRotation(const btTransform& transform)
{
    btQuaternion quanternion = transform.getRotation();
    btVector3    rotation;
    CPhysicsSharedLogic::QuaternionToEuler(quanternion, rotation);
    return ConvertVector(rotation);
}

CVector CPhysicsSharedLogic::GetPosition(const btTransform& transform)
{
    return ConvertVector(transform.getOrigin());
}

void CPhysicsSharedLogic::SetPosition(btTransform& transform, const CVector vecPosition)
{
    transform.setOrigin(ConvertVector(vecPosition));
}

void CPhysicsSharedLogic::SetRotation(btCollisionObject* pCollisionObject, const CVector vecRotation)
{
    btTransform& transform = pCollisionObject->getWorldTransform();
    SetRotation(transform, vecRotation);
    pCollisionObject->setWorldTransform(transform);
}

void CPhysicsSharedLogic::SetPosition(btCollisionObject* pCollisionObject, const CVector vecPosition)
{
    btTransform& transform = pCollisionObject->getWorldTransform();
    SetPosition(transform, vecPosition);
    pCollisionObject->setWorldTransform(transform);
}

CVector CPhysicsSharedLogic::GetRotation(btCollisionObject* pCollisionObject)
{
    btTransform& transform = pCollisionObject->getWorldTransform();
    return GetRotation(transform);
}

CVector CPhysicsSharedLogic::GetPosition(btCollisionObject* pCollisionObject)
{
    btTransform& transform = pCollisionObject->getWorldTransform();
    return GetPosition(transform);
}

btBoxShape* CPhysicsSharedLogic::CreateBox(const CVector half)
{
    return new btBoxShape(ConvertVector(half));
}

std::unique_ptr<btRigidBody> CPhysicsSharedLogic::CreateRigidBody(btCollisionShape* pBtShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
{
    btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform::getIdentity());
    motionstate->m_centerOfMassOffset.setOrigin(ConvertVector(vecCenterOfMass));

    btVector3 localInertia(vecLocalInertia.fX, vecLocalInertia.fY, vecLocalInertia.fZ);
    pBtShape->calculateLocalInertia(fMass, localInertia);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(fMass, motionstate, pBtShape, localInertia);
    std::unique_ptr<btRigidBody>             pRigidBody = std::make_unique<btRigidBody>(rigidBodyCI);
    return std::move(pRigidBody);
}

void CPhysicsSharedLogic::EulerToQuaternion(const btVector3& rotation, btQuaternion& result)
{
    float c1 = cos(rotation.getX() / 2 * PI / 180);
    float c2 = cos(rotation.getY() / 2 * PI / 180);
    float c3 = cos(rotation.getZ() / 2 * PI / 180);
    float s1 = sin(rotation.getX() / 2 * PI / 180);
    float s2 = sin(rotation.getY() / 2 * PI / 180);
    float s3 = sin(rotation.getZ() / 2 * PI / 180);
    float x = s1 * c2 * c3 + c1 * s2 * s3;
    float y = c1 * s2 * c3 - s1 * c2 * s3;
    float z = c1 * c2 * s3 + s1 * s2 * c3;
    float w = c1 * c2 * c3 - s1 * s2 * s3;

    result.setW(w);
    result.setX(x);
    result.setY(y);
    result.setZ(z);
}

void CPhysicsSharedLogic::QuaternionToEuler(btQuaternion rotation, btVector3& result)
{
    rotation.setX(-rotation.getX());
    rotation.setY(-rotation.getY());
    rotation.setZ(-rotation.getZ());
    rotation.normalize();

    const double eps = 1e-7;
    double       x, y, z;
    double       qw = rotation.getW(), qx = rotation.getX(), qy = rotation.getY(), qz = rotation.getZ();

    double sqx = qx * qx, sqy = qy * qy, sqz = qz * qz;

    double test = (2.0 * qy * qz) - (2.0 * qx * qw);
    if (test >= 1 - eps)
    {
        x = PI / 2.0;
        y = -atan2(qy, qw);
        z = -atan2(qz, qw);
    }
    else if (-test >= 1 - eps)
    {
        x = -PI / 2.0;
        y = -atan2(qy, qw);
        z = -atan2(qz, qw);
    }
    else
    {
        x = asin(test);
        y = -atan2((qx * qz) + (qy * qw), 0.5 - sqx - sqy);
        z = -atan2((qx * qy) + (qz * qw), 0.5 - sqx - sqz);
    }

    x *= 180.0 / PI;
    y *= 180.0 / PI;
    z *= 180.0 / PI;

#pragma warning(push)
#pragma warning(disable : 4244)
    result.setX(x);
    result.setY(y);
    result.setZ(z);
#pragma warning(pop)
}

void CPhysicsSharedLogic::CheckMaximumPrimitiveSize(const CVector vector)
{
    if (abs(vector.fX) > BulletPhysics::Limits::MaximumPrimitiveSize || abs(vector.fY) > BulletPhysics::Limits::MaximumPrimitiveSize ||
        abs(vector.fZ) > BulletPhysics::Limits::MaximumPrimitiveSize)
        throw std::invalid_argument(SString("Maximum x,y,z must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());
}

void CPhysicsSharedLogic::CheckMinimumPrimitiveSize(const CVector vector)
{
    if (vector.fX < BulletPhysics::Limits::MinimumPrimitiveSize || vector.fY < BulletPhysics::Limits::MinimumPrimitiveSize ||
        vector.fZ < BulletPhysics::Limits::MinimumPrimitiveSize)
        throw std::invalid_argument(SString("Minimum x,y,z must be equal or greater than %.03f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());
}

void CPhysicsSharedLogic::CheckMinimumPrimitiveSize(const float value)
{
    if (BulletPhysics::Limits::MinimumPrimitiveSize > value)
        throw std::invalid_argument(SString("Minumum value must be equal or smaller than %.03f units", BulletPhysics::Limits::MinimumPrimitiveSize).c_str());
}

void CPhysicsSharedLogic::CheckMaximumPrimitiveSize(const float value)
{
    if (BulletPhysics::Limits::MaximumPrimitiveSize < value)
        throw std::invalid_argument(SString("Maximum value must be equal or smaller than %.02f units", BulletPhysics::Limits::MaximumPrimitiveSize).c_str());
}

void CPhysicsSharedLogic::CheckPrimitiveSize(const float value)
{
    CheckMinimumPrimitiveSize(value);
    CheckMaximumPrimitiveSize(value);
}

void CPhysicsSharedLogic::CheckPrimitiveSize(const CVector vector)
{
    CheckMinimumPrimitiveSize(vector);
    CheckMaximumPrimitiveSize(vector);
}

CVector CPhysicsSharedLogic::ConvertVector(const btVector3& vector)
{
    return CVector(vector.x(), vector.y(), vector.z());
}

btVector3 CPhysicsSharedLogic::ConvertVector(const CVector& vector)
{
    return btVector3{vector.fX, vector.fY, vector.fZ};
}

