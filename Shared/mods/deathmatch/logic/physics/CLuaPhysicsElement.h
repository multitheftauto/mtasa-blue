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

class CLuaPhysicsElement
{
protected:
    CLuaPhysicsElement(EIdClass::EIdClassType classType);
    ~CLuaPhysicsElement();

public:
    uint                   GetScriptID() const { return m_uiScriptID; }
    EIdClass::EIdClassType GetClassType() const { return m_classType; }
    virtual bool           Destroy() = 0;

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::Shape; }

    const char* GetName() { return EnumToString(GetType()); }


    // Thread-safe access to bullet physics element
    template <typename T>
    class ElementLock
    {
        friend CBulletPhysics;

        std::unique_lock<std::mutex> m_lock;
        std::unique_lock<std::recursive_mutex> m_elementsLock;

    public:
        // static void* operator new(size_t) = delete;

        ElementLock(T pElement) : m_pElement(pElement), m_lock(pElement->m_lockElement, std::try_to_lock), m_elementsLock(g_pGame->GetPhysics()->m_elementsLock)
        {
            assert(m_lock.owns_lock() && "Element is already locked");
        }

    private:
        const T               m_pElement;
    };

    mutable std::mutex m_lockElement;

protected:
    std::atomic<bool> m_bNeedsUpdate = false;

private:
    void RemoveScriptID();

    std::atomic<bool>                                 m_bHasEnqueuedChanges = false;
    EIdClass::EIdClassType                            m_classType;
    unsigned int                                      m_uiScriptID;
    SharedUtil::ConcurrentList<std::function<void()>> m_listChanges;
};
