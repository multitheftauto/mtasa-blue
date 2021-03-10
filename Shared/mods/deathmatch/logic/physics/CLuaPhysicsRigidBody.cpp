/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsRigidBody.cpp
 *  PURPOSE:     Lua rigid body class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsRigidBody::CLuaPhysicsRigidBody(CLuaPhysicsShape* pShape, float fMass, CVector vecLocalInertia, CVector vecCenterOfMass)
    : CLuaPhysicsWorldElement(pShape->GetPhysics(), EIdClass::RIGID_BODY), m_pShape(pShape)
{
    m_pMotionState = std::make_unique<MotionState>();

    m_pRigidBodyProxy = CPhysicsRigidBodyProxy::New(m_pShape, fMass, vecLocalInertia, vecCenterOfMass, m_pMotionState.get());
    m_pRigidBodyProxy->setUserPointer((void*)this);
    m_pRigidBodyProxy->setUserIndex(EIdClass::RIGID_BODY);
    SetLinearDumping(BulletPhysics::Defaults::RigidBodyLinearDumping);
    SetAngularDumping(BulletPhysics::Defaults::RigidBodyAngularDumping);
    SetLinearSleepingThreshold(BulletPhysics::Defaults::RigidBodyLinearSleepingThreshold);
    SetAngularSleepingThreshold(BulletPhysics::Defaults::RigidBodyAngularSleepingThreshold);
    pShape->AddRigidBody(this);
}

CLuaPhysicsRigidBody::~CLuaPhysicsRigidBody()
{
    while (!m_constraintList.empty())
    {
        auto const& constraint = m_constraintList.back();
        RemoveConstraintRef(constraint);
        GetPhysics()->DestroyElement(constraint);
    }
    m_constraintList.clear();
}

bool CLuaPhysicsRigidBody::Destroy()
{
    GetPhysics()->DestroyElement(this);
    m_pShape->RemoveRigidBody(this);
    return true;
}

void CLuaPhysicsRigidBody::SetPosition(CVector vecPosition)
{
    btTransform transform;
    {
        ElementLock lk(this);
        btTransform transform = m_pRigidBodyProxy->getWorldTransform();
        CPhysicsSharedLogic::SetPosition(transform, vecPosition);
        // m_pRigidBodyProxy->getMotionState()->setWorldTransform(transform);
        m_pRigidBodyProxy->setWorldTransform(transform);
        m_pRigidBodyProxy->proceedToTransform(transform);
    }
    NeedsActivation();
}

const CVector CLuaPhysicsRigidBody::GetPosition() const
{
    btTransform& transform = m_pRigidBodyProxy->getWorldTransform();
    return CPhysicsSharedLogic::GetPosition(transform);
}

void CLuaPhysicsRigidBody::SetRotation(CVector vecRotation)
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    CPhysicsSharedLogic::SetRotation(transform, vecRotation);
    {
        ElementLock lk(this);
        btTransform& transform = m_pRigidBodyProxy->getWorldTransform();
        CPhysicsSharedLogic::SetRotation(transform, vecRotation);
        m_pRigidBodyProxy->proceedToTransform(transform);
    }
    NeedsActivation();
}

const CVector CLuaPhysicsRigidBody::GetRotation() const
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    return CPhysicsSharedLogic::GetRotation(transform);
}

void CLuaPhysicsRigidBody::SetScale(const CVector& vecScale)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->getCollisionShape()->setLocalScaling(vecScale);
    NeedsActivation();
}

const CVector CLuaPhysicsRigidBody::GetScale() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getCollisionShape()->getLocalScaling();
}

void CLuaPhysicsRigidBody::SetMatrix(const CMatrix& matrix)
{
    {
        ElementLock lk(this);
        btTransform& transform = m_pRigidBodyProxy->getWorldTransform();
        CPhysicsSharedLogic::SetPosition(transform, matrix.GetRotation());
        CPhysicsSharedLogic::SetRotation(transform, matrix.GetRotation());
        m_pRigidBodyProxy->getCollisionShape()->setLocalScaling(matrix.GetScale());
        m_pRigidBodyProxy->proceedToTransform(transform);
    }
    NeedsActivation();
}

const CMatrix CLuaPhysicsRigidBody::GetMatrix() const
{
    CMatrix matrix;

    return matrix;
}

void CLuaPhysicsRigidBody::SetMass(float fMass)
{
    ElementLock  lk(this);
    const btVector3 localInertia = m_pRigidBodyProxy->getLocalInertia();
    m_pRigidBodyProxy->setMassProps(fMass, localInertia);
}

void CLuaPhysicsRigidBody::SetLinearDumping(float fLinearDamping)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->setDamping(fLinearDamping, m_pRigidBodyProxy->getAngularDamping());
}

void CLuaPhysicsRigidBody::SetAngularDumping(float fAngularDamping)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->setDamping(m_pRigidBodyProxy->getLinearDamping(), fAngularDamping);
}

void CLuaPhysicsRigidBody::UpdateAABB()
{
    ElementLock lk(this);

    CBulletPhysics::WorldContext world(GetPhysics());
    world->updateSingleAabb(m_pRigidBodyProxy.get());
}

void CLuaPhysicsRigidBody::NeedsActivation() const
{
    if (!m_bActivationRequested)
    {
        m_bActivationRequested = true;
        GetPhysics()->AddToActivationStack((CLuaPhysicsRigidBody*)this);
    }
}

void CLuaPhysicsRigidBody::NeedsAABBUpdate() const
{
    if (!m_bAABBUpdateRequested)
    {
        m_bAABBUpdateRequested = true;
        GetPhysics()->AddToActivationStack((CLuaPhysicsRigidBody*)this);
    }
}

bool CLuaPhysicsRigidBody::Activate() const
{
    ElementLock lk(this);
    if (!m_pRigidBodyProxy->isActive())            // prevent activation when you eg. set position directly after create
    {
        m_pRigidBodyProxy->setCollisionFlags(m_pRigidBodyProxy->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
        m_pRigidBodyProxy->setActivationState(ACTIVE_TAG);
        m_pRigidBodyProxy->activate(true);
        m_bActivationRequested = false;
        return true;
    }
    m_bActivationRequested = false;
    return false;
}

void CLuaPhysicsRigidBody::SetCcdMotionThreshold(float fThreshold)
{
    ElementLock        lk(this);
    m_pRigidBodyProxy->setCcdMotionThreshold(fThreshold);
}

float CLuaPhysicsRigidBody::GetCcdMotionThreshold() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getCcdMotionThreshold();
}

void CLuaPhysicsRigidBody::SetSweptSphereRadius(float fSphereRadius)
{
    assert(fSphereRadius >= 0);

    ElementLock lk(this);
    m_pRigidBodyProxy->setCcdSweptSphereRadius(fSphereRadius);
}

float CLuaPhysicsRigidBody::GetSweptSphereRadius() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getCcdSweptSphereRadius();
}

bool CLuaPhysicsRigidBody::IsSleeping() const
{
    ElementLock lk(this);
    return !m_pRigidBodyProxy->isActive();
}

bool CLuaPhysicsRigidBody::WantsSleeping() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->wantsSleeping();
}

float CLuaPhysicsRigidBody::GetMass() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getMass();
}

void CLuaPhysicsRigidBody::SetLinearVelocity(const CVector& vecVelocity)
{
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->setLinearVelocity(vecVelocity);
    }
    NeedsActivation();
}

CVector CLuaPhysicsRigidBody::GetLinearVelocity() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getLinearVelocity();
}

void CLuaPhysicsRigidBody::SetAngularVelocity(const CVector& vecVelocity)
{
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->setAngularVelocity(vecVelocity);
    }
    NeedsActivation();
}

CVector CLuaPhysicsRigidBody::GetAngularVelocity() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getAngularVelocity();
}

void CLuaPhysicsRigidBody::ApplyCentralForce(const CVector& vecForce)
{
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->applyCentralForce(vecForce);
    }
    NeedsActivation();
}

void CLuaPhysicsRigidBody::ApplyDamping(float fDamping)
{
    assert(fDamping >= 0);
    
    ElementLock lk(this);
    m_pRigidBodyProxy->applyDamping(fDamping);
}

void CLuaPhysicsRigidBody::ApplyForce(const CVector& vecFrom, const CVector& vecTo)
{
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->applyForce(vecFrom, vecTo);
    }
    NeedsActivation();
}

void CLuaPhysicsRigidBody::ApplyImpulse(const CVector& vecFrom, const CVector& vecTo)
{
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->applyImpulse(vecFrom, vecTo);
    }
    NeedsActivation();
}

void CLuaPhysicsRigidBody::ApplyCentralImpulse(const CVector& vecForce)
{
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->applyCentralImpulse(vecForce);
    }
    NeedsActivation();
}

void CLuaPhysicsRigidBody::ApplyTorque(const CVector& vecTraque)
{
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->applyTorque(vecTraque);
    }
    NeedsActivation();

}

void CLuaPhysicsRigidBody::ApplyTorqueImpulse(const CVector& vecTraque)
{
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->applyTorqueImpulse(vecTraque);
    }
    NeedsActivation();
}

void CLuaPhysicsRigidBody::SetRestitution(float fRestitution)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->setRestitution(fRestitution);
}

float CLuaPhysicsRigidBody::GetRestitution() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getRestitution();
}

void CLuaPhysicsRigidBody::SetFilterMask(int iMask)
{
    ElementLock        lk(this);
    m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterMask = iMask;
}

int CLuaPhysicsRigidBody::GetFilterMask() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterMask;
}

void CLuaPhysicsRigidBody::SetDebugColor(const SColor& color)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->setCustomDebugColor(btVector3((float)color.R / 255, (float)color.G / 255, (float)color.B / 255));
}

void CLuaPhysicsRigidBody::SetFilterGroup(int iGroup)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterGroup = iGroup;
}

void CLuaPhysicsRigidBody::SetGravity(CVector vecGravity)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->setGravity(vecGravity);
}

CVector CLuaPhysicsRigidBody::GetGravity() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getGravity();
}

int CLuaPhysicsRigidBody::GetFilterGroup() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterGroup;
}

void CLuaPhysicsRigidBody::RemoveDebugColor()
{
    ElementLock lk(this);
    m_pRigidBodyProxy->removeCustomDebugColor();
}

const SColor CLuaPhysicsRigidBody::GetDebugColor() const
{
    SColor    color;
    btVector3 btColor;
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->getCustomDebugColor(btColor);
    }
    color.R = btColor.getX() * 255;
    color.G = btColor.getY() * 255;
    color.B = btColor.getZ() * 255;
    color.A = 255;
    return color;
}

void CLuaPhysicsRigidBody::SetLinearSleepingThreshold(float fLinear)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->setSleepingThresholds(fLinear, m_pRigidBodyProxy->getAngularSleepingThreshold());
}

void CLuaPhysicsRigidBody::SetAngularSleepingThreshold(float fAngular)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->setSleepingThresholds(m_pRigidBodyProxy->getLinearSleepingThreshold(), fAngular);
}

float CLuaPhysicsRigidBody::GetLinearSleepingThreshold() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getLinearSleepingThreshold();
}

float CLuaPhysicsRigidBody::GetAngularSleepingThreshold() const
{
    ElementLock lk(this);
    return m_pRigidBodyProxy->getAngularSleepingThreshold();
}

btTransform& CLuaPhysicsRigidBody::PredictTransform(float time) const
{
    btTransform predictedTransform = btTransform::getIdentity();
    {
        ElementLock lk(this);
        m_pRigidBodyProxy->predictIntegratedTransform(time, predictedTransform);
    }
    return predictedTransform;
}

void CLuaPhysicsRigidBody::ClearForces()
{
    ElementLock lk(this);
    m_pRigidBodyProxy->setLinearVelocity(btVector3(0, 0, 0));
    m_pRigidBodyProxy->setAngularVelocity(btVector3(0, 0, 0));
    m_pRigidBodyProxy->setInterpolationAngularVelocity(btVector3(0, 0, 0));
    m_pRigidBodyProxy->setInterpolationLinearVelocity(btVector3(0, 0, 0));
    m_pRigidBodyProxy->clearForces();
}

void CLuaPhysicsRigidBody::RemoveConstraintRef(CLuaPhysicsConstraint* pConstraint)
{
    ElementLock lk(this);
    m_pRigidBodyProxy->removeConstraintRef(pConstraint->GetConstraint());
    ListRemove(m_constraintList, pConstraint);
}

int CLuaPhysicsRigidBody::GetIslandTag()
{
    return m_pRigidBodyProxy->getIslandTag();
}
