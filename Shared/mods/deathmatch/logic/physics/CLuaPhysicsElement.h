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

#include "ConcurrentList.hpp"
#include <unordered_map>
#include <variant>

class CBulletPhysics;

struct SBoundingBox
{
    CVector vecMin;
    CVector vecMax;
};

struct SBoundingSphere
{
    CVector vecCenter;
    float   fRadius;
};

class ILuaPhysicsElement
{
public:
    virtual void Update() = 0;
    virtual SBoundingBox GetBoundingBox() = 0;
    virtual SBoundingSphere GetBoundingSphere() = 0;
};

class CLuaPhysicsElement : public ILuaPhysicsElement
{
protected:
    CLuaPhysicsElement(CBulletPhysics* pPhysics, EIdClass::EIdClassType classType);
    ~CLuaPhysicsElement();

public:

    CBulletPhysics*        GetPhysics() const { return m_pPhysics; }
    uint                   GetScriptID() const { return m_uiScriptID; }
    EIdClass::EIdClassType GetClassType() const { return m_classType; }
    bool                   Destroy();
    bool                   IsSafeToAccess() const;
    void                   ApplyChanges();

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::Shape; }

    const char* GetName() { return EnumToString(GetType()); }

    void NeedsUpdate();

    // Thread-safe access to bullet physics element
    template <typename T>
    class ElementLock
    {
        friend CBulletPhysics;

        std::unique_lock<std::mutex> m_lock;

    public:
        // static void* operator new(size_t) = delete;

        ElementLock(T pElement) : m_pElement(pElement), m_pPhysics(pElement->GetPhysics()), m_lock(pElement->m_lockElement, std::try_to_lock)
        {
            assert(m_lock.owns_lock() && "Element is already locked");
        }

        ~ElementLock() {}
        bool IsLocked() { return m_lock.owns_lock(); }

    private:
        const CBulletPhysics*     m_pPhysics;
        const T m_pElement;
    };

    mutable std::mutex m_lockElement;

protected:
    std::atomic<bool> m_bNeedsUpdate = false;

private:
    void RemoveScriptID();

    std::atomic<bool>                                 m_bHasEnqueuedChanges = false;
    CBulletPhysics*                                   m_pPhysics;
    EIdClass::EIdClassType                            m_classType;
    unsigned int                                      m_uiScriptID;
    SharedUtil::ConcurrentList<std::function<void()>> m_listChanges;
};
