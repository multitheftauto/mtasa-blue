/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsElement.h
 *  PURPOSE:     Base class for physics elements
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsElement;

#pragma once

#include "ConcurrentStack.hpp"
#include <unordered_map>
#include <variant>

class CBulletPhysics;

class ILuaPhysicsElement
{
public:
    virtual void Update() = 0;
};

class CLuaPhysicsElement : public ILuaPhysicsElement
{
protected:
    CLuaPhysicsElement(CBulletPhysics* pPhysics, EIdClass::EIdClassType classType);
    ~CLuaPhysicsElement();

public:
    enum class eTempDataKey
    {
        Mass,
        LinearDumping,
        AngularDumping,
        CcdMotionThreshold,
        SweptSphereRadius,
        LinearVelocity,
        AngularVelocity,
        Restitution,
        Mask,
        Group,
        SleepingThresholdLinear,
        SleepingThresholdAngular,
        LocalInertia,
        CenterOfMass,
        DebugColor,
        Gravity,
    };

    struct SPhysicsCollisionContact
    {
        CVector vecPositionWorldOn;
        CVector vecLocalPoint;
        CVector vecLateralFrictionDir;
        int     contactTriangle;
        float   appliedImpulse;
        float   appliedImpulseLiteral;
    };

    struct SPhysicsCollisionReport
    {
        std::shared_ptr<CLuaPhysicsElement>                    pElement;
        std::vector<std::shared_ptr<SPhysicsCollisionContact>> m_vecContacts;
    };


    CBulletPhysics*        GetPhysics() const { return m_pPhysics; }
    uint                   GetScriptID() const { return m_uiScriptID; }
    EIdClass::EIdClassType GetClassType() const { return m_classType; }
    void                   Destroy();
    bool                   IsReady() const { return m_isReady; }
    bool                   IsSafeToAccess() const;
    void                   ApplyChanges();

    // Run changes on worker thread, let you modify element before get created
    void CommitChange(std::function<void()> change);

    virtual void ClearCollisionReport() {}
    virtual void ReportCollision(std::unique_ptr<SPhysicsCollisionReport> pCollisionReport) {}
    void         NeedsUpdate();

    template<typename T>
    bool GetTempData(eTempDataKey key, T& out) const // Multithread safe
    {
        std::lock_guard guard(m_tempDataLock);
        auto            iter = m_mapTempData.find(key);
        if (iter != m_mapTempData.end())
        {
            out = std::get<T>((*iter).second);
            return true;
        }
        return false;
    }

    template <typename T>
    void SetTempData(eTempDataKey key, const T& value) // Multithread safe
    {
        std::lock_guard guard(m_tempDataLock);
        m_mapTempData[key] = value;
    }

protected:
    std::atomic<bool> m_bNeedsUpdate = false;
    void Ready() { m_isReady = true; }
private:

    void ClearTempData();            // Multithread safe
    void RemoveScriptID();

    std::atomic<bool>                                  m_isReady;
    std::atomic<bool>                                  m_bHasEnqueuedChanges = false;
    CBulletPhysics*                                    m_pPhysics;
    EIdClass::EIdClassType                             m_classType;
    unsigned int                                       m_uiScriptID;
    SharedUtil::ConcurrentStack<std::function<void()>> m_stackChanges;

    mutable std::mutex                                         m_tempDataLock;
    // Stores information user set for get function while they are being permanently applied into specific element
    std::unordered_map<eTempDataKey, std::variant<int, float, CVector, SColor>> m_mapTempData;
};
