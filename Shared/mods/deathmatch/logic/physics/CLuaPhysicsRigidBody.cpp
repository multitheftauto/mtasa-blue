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
    SetTempData(eTempDataKey::Mass, fMass);
    SetTempData(eTempDataKey::LocalInertia, vecLocalInertia);
    SetTempData(eTempDataKey::CenterOfMass, vecCenterOfMass);
}

CLuaPhysicsRigidBody::~CLuaPhysicsRigidBody()
{
    Unlink();
}

void CLuaPhysicsRigidBody::Initialize()
{
    assert(!IsReady());            // in case something goes wrong, or element get initialized twice

    float   fMass;
    CVector vecLocalInertia, vecCenterOfMass;
    GetTempData(eTempDataKey::Mass, fMass);
    GetTempData(eTempDataKey::LocalInertia, vecLocalInertia);
    GetTempData(eTempDataKey::CenterOfMass, vecCenterOfMass);

    m_pRigidBodyProxy = CPhysicsRigidBodyProxy::Create(m_pShape, fMass, vecLocalInertia, vecCenterOfMass, m_pMotionState.get());
    m_pRigidBodyProxy->setUserPointer((void*)this);

    SetDumping(BulletPhysics::Defaults::RigidBodyLinearDumping, BulletPhysics::Defaults::RigidBodyAngularDumping);
    SetSleepingThresholds(BulletPhysics::Defaults::RigidBodyLinearSleepingThreshold, BulletPhysics::Defaults::RigidBodyAngularSleepingThreshold);

    Ready();
    // SetMatrix(m_matrix);
}

void CLuaPhysicsRigidBody::SetPosition(CVector vecPosition, bool dontCommitChanges)
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
    if (m_pRigidBodyProxy)
        m_pRigidBodyProxy->getMotionState()->setWorldTransform(transform);

    if (dontCommitChanges)
        return;

    std::function<void()> change([&, vecPosition]() {
        btTransform transform = m_pRigidBodyProxy->getWorldTransform();
        CLuaPhysicsSharedLogic::SetPosition(transform, vecPosition);
        // m_pRigidBodyProxy->getMotionState()->setWorldTransform(transform);
        m_pRigidBodyProxy->setWorldTransform(transform);
        m_pRigidBodyProxy->proceedToTransform(transform);
        NeedsActivation();
    });

    CommitChange(change);
}

const CVector CLuaPhysicsRigidBody::GetPosition() const
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    return CLuaPhysicsSharedLogic::GetPosition(transform);
}

void CLuaPhysicsRigidBody::SetRotation(CVector vecRotation, bool dontCommitChanges)
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
    m_pRigidBodyProxy->getMotionState()->setWorldTransform(transform);

    if (dontCommitChanges)
        return;

    std::function<void()> change([&, vecRotation]() {
        btTransform& transform = m_pRigidBodyProxy->getWorldTransform();
        CLuaPhysicsSharedLogic::SetRotation(transform, vecRotation);
        m_pRigidBodyProxy->proceedToTransform(transform);
        NeedsActivation();
    });

    CommitChange(change);
}

const CVector CLuaPhysicsRigidBody::GetRotation() const
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    return CLuaPhysicsSharedLogic::GetRotation(transform);
}

void CLuaPhysicsRigidBody::SetScale(const CVector& vecScale)
{
    SetTempData(eTempDataKey::Scale, vecScale);

    std::function<void()> change([&, vecScale]() {
        m_pRigidBodyProxy->getCollisionShape()->setLocalScaling(vecScale);
        NeedsActivation();
    });

    CommitChange(change);
}

const CVector CLuaPhysicsRigidBody::GetScale() const
{
    CVector vecScale;
    if (GetTempData(eTempDataKey::Scale, vecScale))
        return vecScale;

    return m_pRigidBodyProxy->getCollisionShape()->getLocalScaling();
}

void CLuaPhysicsRigidBody::SetMatrix(const CMatrix& matrix)
{
    btTransform transform;
    m_pMotionState->getWorldTransform(transform);
    CLuaPhysicsSharedLogic::SetPosition(transform, matrix.GetPosition());
    CLuaPhysicsSharedLogic::SetRotation(transform, matrix.GetRotation());
    SetTempData(eTempDataKey::Scale, matrix.GetScale());
    m_pRigidBodyProxy->getMotionState()->setWorldTransform(transform);

    std::function<void()> change([&, matrix]() {
        btTransform& transform = m_pRigidBodyProxy->getWorldTransform();
        CLuaPhysicsSharedLogic::SetPosition(transform, matrix.GetRotation());
        CLuaPhysicsSharedLogic::SetRotation(transform, matrix.GetRotation());
        m_pRigidBodyProxy->getCollisionShape()->setLocalScaling(matrix.GetScale());
        m_pRigidBodyProxy->proceedToTransform(transform);
        NeedsActivation();
    });

    CommitChange(change);
}

const CMatrix CLuaPhysicsRigidBody::GetMatrix() const
{
    CMatrix matrix;

    return matrix;
}

void CLuaPhysicsRigidBody::SetMass(float fMass)
{
    SetTempData(eTempDataKey::Mass, fMass);

    std::function<void()> change([&, fMass]() {
        const btVector3 localInertia = m_pRigidBodyProxy->getLocalInertia();
        m_pRigidBodyProxy->setMassProps(fMass, localInertia);
    });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::SetDumping(float fLinearDamping, float fAngularDamping)
{
    SetTempData(eTempDataKey::LinearDumping, fLinearDamping);
    SetTempData(eTempDataKey::AngularDumping, fAngularDamping);

    std::function<void()> change([&, fLinearDamping, fAngularDamping]() { m_pRigidBodyProxy->setDamping(fLinearDamping, fAngularDamping); });

    CommitChange(change);
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
    SetTempData(eTempDataKey::CcdMotionThreshold, fThreshold);

    std::function<void()> change([&, fThreshold]() { m_pRigidBodyProxy->setCcdMotionThreshold(fThreshold); });

    CommitChange(change);
}

float CLuaPhysicsRigidBody::GetCcdMotionThreshold() const
{
    float fCcdMotionThreshold;
    if (GetTempData<float>(eTempDataKey::CcdMotionThreshold, fCcdMotionThreshold))
        return fCcdMotionThreshold;

    if (IsReady())
        return m_pRigidBodyProxy->getCcdMotionThreshold();
    return 0;
}

void CLuaPhysicsRigidBody::SetSweptSphereRadius(float fSphereRadius)
{
    assert(fSphereRadius >= 0);

    SetTempData(eTempDataKey::SweptSphereRadius, fSphereRadius);

    std::function<void()> change([&, fSphereRadius]() { m_pRigidBodyProxy->setCcdSweptSphereRadius(fSphereRadius); });

    CommitChange(change);
}

float CLuaPhysicsRigidBody::GetSweptSphereRadius() const
{
    float fCcdMotionThreshold;
    if (GetTempData(eTempDataKey::SweptSphereRadius, fCcdMotionThreshold))
        return fCcdMotionThreshold;

    return m_pRigidBodyProxy->getCcdSweptSphereRadius();
}

bool CLuaPhysicsRigidBody::IsSleeping() const
{
    if (IsReady())
        return !m_pRigidBodyProxy->isActive();
    return true;
}

bool CLuaPhysicsRigidBody::WantsSleeping() const
{
    if (IsReady())
        m_pRigidBodyProxy->wantsSleeping();
    return false;
}

float CLuaPhysicsRigidBody::GetMass() const
{
    float fMass;
    if (GetTempData(eTempDataKey::Mass, fMass))
        return fMass;

    if (IsReady())
        return m_pRigidBodyProxy->getMass();
    return BulletPhysics::Defaults::RigidBodyMass;
}

void CLuaPhysicsRigidBody::SetLinearVelocity(const CVector& vecVelocity)
{
    SetTempData(eTempDataKey::LinearVelocity, vecVelocity);

    std::function<void()> change([&, vecVelocity]() { m_pRigidBodyProxy->setLinearVelocity(vecVelocity); });

    CommitChange(change);
}

CVector CLuaPhysicsRigidBody::GetLinearVelocity() const
{
    CVector fLinearVelocity;
    if (GetTempData(eTempDataKey::LinearVelocity, fLinearVelocity))
        return fLinearVelocity;

    if (IsReady())
        return m_pRigidBodyProxy->getLinearVelocity();
    return {0, 0, 0};
}

void CLuaPhysicsRigidBody::SetAngularVelocity(const CVector& vecVelocity)
{
    SetTempData(eTempDataKey::AngularVelocity, vecVelocity);

    std::function<void()> change([&, vecVelocity]() {
        m_pRigidBodyProxy->setAngularVelocity(vecVelocity);
        NeedsActivation();
    });

    CommitChange(change);
}

CVector CLuaPhysicsRigidBody::GetAngularVelocity() const
{
    CVector fLinearVelocity;
    if (GetTempData(eTempDataKey::LinearVelocity, fLinearVelocity))
        return fLinearVelocity;

    if (IsReady())
        return m_pRigidBodyProxy->getAngularVelocity();
    return {0, 0, 0};
}

void CLuaPhysicsRigidBody::ApplyCentralForce(const CVector& vecForce)
{
    std::function<void()> change([&, vecForce]() {
        m_pRigidBodyProxy->applyCentralForce(vecForce);
        NeedsActivation();
    });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::ApplyDamping(float fDamping)
{
    assert(fDamping >= 0);

    std::function<void()> change([&, fDamping]() { m_pRigidBodyProxy->applyDamping(fDamping); });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::ApplyForce(const CVector& vecFrom, const CVector& vecTo)
{
    std::function<void()> change([&, vecFrom, vecTo]() {
        m_pRigidBodyProxy->applyForce(vecFrom, vecTo);
        NeedsActivation();
    });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::ApplyImpulse(const CVector& vecFrom, const CVector& vecTo)
{
    std::function<void()> change([&, vecFrom, vecTo]() {
        m_pRigidBodyProxy->applyImpulse(vecFrom, vecTo);
        NeedsActivation();
    });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::ApplyCentralImpulse(const CVector& vecForce)
{
    std::function<void()> change([&, vecForce]() {
        m_pRigidBodyProxy->applyCentralImpulse(vecForce);
        NeedsActivation();
    });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::ApplyTorque(const CVector& vecTraque)
{
    std::function<void()> change([&, vecTraque]() {
        m_pRigidBodyProxy->applyTorque(vecTraque);
        NeedsActivation();
    });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::ApplyTorqueImpulse(const CVector& vecTraque)
{
    std::function<void()> change([&, vecTraque]() {
        m_pRigidBodyProxy->applyTorqueImpulse(vecTraque);
        NeedsActivation();
    });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::SetRestitution(float fRestitution)
{
    SetTempData(eTempDataKey::Restitution, fRestitution);

    std::function<void()> change([&, fRestitution]() { m_pRigidBodyProxy->setRestitution(fRestitution); });

    CommitChange(change);
}

float CLuaPhysicsRigidBody::GetRestitution() const
{
    float fRestitution;
    if (GetTempData(eTempDataKey::Restitution, fRestitution))
        return fRestitution;

    if (IsReady())
        return m_pRigidBodyProxy->getRestitution();
    return 0;
}

void CLuaPhysicsRigidBody::SetFilterMask(int iMask)
{
    SetTempData(eTempDataKey::Mask, iMask);

    std::function<void()> change([&, iMask]() { m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterMask = iMask; });

    CommitChange(change);
}

int CLuaPhysicsRigidBody::GetFilterMask() const
{
    int iMask;
    if (GetTempData(eTempDataKey::Mask, iMask))
        return iMask;

    if (IsReady())
        return m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterMask;
    return -1;
}

void CLuaPhysicsRigidBody::SetDebugColor(const SColor& color)
{
    SetTempData(eTempDataKey::DebugColor, color);

    std::function<void()> change(
        [&, color]() { m_pRigidBodyProxy->setCustomDebugColor(btVector3((float)color.R / 255, (float)color.G / 255, (float)color.B / 255)); });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::SetFilterGroup(int iGroup)
{
    SetTempData(eTempDataKey::Group, iGroup);

    std::function<void()> change([&, iGroup]() { m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterGroup = iGroup; });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::SetGravity(CVector vecGravity)
{
    SetTempData(eTempDataKey::Gravity, vecGravity);

    std::function<void()> change([&, vecGravity]() { m_pRigidBodyProxy->setGravity(vecGravity); });

    CommitChange(change);
}

CVector CLuaPhysicsRigidBody::GetGravity() const
{
    CVector vecGravity;
    if (GetTempData(eTempDataKey::Gravity, vecGravity))
        return vecGravity;

    if (IsReady())
        return m_pRigidBodyProxy->getGravity();
    return {1, 1, 1};
}

int CLuaPhysicsRigidBody::GetFilterGroup() const
{
    SColor iGroup;
    if (GetTempData(eTempDataKey::Group, iGroup))
        return iGroup;

    if (IsReady())
        return m_pRigidBodyProxy->getBroadphaseHandle()->m_collisionFilterGroup;
    return BulletPhysics::Defaults::FilterGroup;
}

void CLuaPhysicsRigidBody::RemoveDebugColor()
{
    std::function<void()> change([&]() { m_pRigidBodyProxy->removeCustomDebugColor(); });
}

const SColor CLuaPhysicsRigidBody::GetDebugColor() const
{
    SColor color;
    if (GetTempData(eTempDataKey::DebugColor, color))
        return color;

    if (IsReady())
    {
        btVector3 btColor;
        m_pRigidBodyProxy->getCustomDebugColor(btColor);
        color.R = btColor.getX() * 255;
        color.G = btColor.getY() * 255;
        color.B = btColor.getZ() * 255;
        color.A = 255;
        return color;
    }
    return 0;            // #TODO, set white
}

void CLuaPhysicsRigidBody::SetSleepingThresholds(float fLinear, float fAngular)
{
    assert(fLinear >= 0);
    assert(fAngular >= 0);

    SetTempData(eTempDataKey::SleepingThresholdLinear, fLinear);
    SetTempData(eTempDataKey::SleepingThresholdAngular, fAngular);

    std::function<void()> change([&, fLinear, fAngular]() { m_pRigidBodyProxy->setSleepingThresholds(fLinear, fAngular); });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::GetSleepingThresholds(float& fLinear, float& fAngular) const
{
    if (GetTempData(eTempDataKey::SleepingThresholdLinear, fLinear))
        if (GetTempData(eTempDataKey::SleepingThresholdAngular, fAngular))
            return;

    if (IsReady())
    {
        fLinear = m_pRigidBodyProxy->getLinearSleepingThreshold();
        fAngular = m_pRigidBodyProxy->getAngularSleepingThreshold();
        return;
    }
    fLinear = 0;
    fAngular = 0;
}

btTransform& CLuaPhysicsRigidBody::PredictTransform(float time) const
{
    btTransform predictedTransform = btTransform::getIdentity();
    if (IsReady())
    {
        m_pRigidBodyProxy->predictIntegratedTransform(time, predictedTransform);
    }
    return predictedTransform;
}

void CLuaPhysicsRigidBody::ClearForces()
{
    std::function<void()> change([&]() {
        m_pRigidBodyProxy->setLinearVelocity(btVector3(0, 0, 0));
        m_pRigidBodyProxy->setAngularVelocity(btVector3(0, 0, 0));
        m_pRigidBodyProxy->setInterpolationAngularVelocity(btVector3(0, 0, 0));
        m_pRigidBodyProxy->setInterpolationLinearVelocity(btVector3(0, 0, 0));
        m_pRigidBodyProxy->clearForces();
    });

    CommitChange(change);
}

void CLuaPhysicsRigidBody::RemoveConstraintRef(CLuaPhysicsConstraint* pConstraint)
{
    m_pRigidBodyProxy->removeConstraintRef(pConstraint->GetConstraint());
    ListRemove(m_constraintList, pConstraint);
}

void CLuaPhysicsRigidBody::Unlink()
{
    if (IsReady())
    {
        while (!m_constraintList.empty())
        {
            auto const& constraint = m_constraintList.back();
            RemoveConstraintRef(constraint);
            GetPhysics()->DestroyElement(constraint);
        }
        m_constraintList.clear();

        m_pRigidBodyProxy.reset();
        // m_pShape->RemoveRigidBody(this);
        m_pShape = nullptr;
    }
}
