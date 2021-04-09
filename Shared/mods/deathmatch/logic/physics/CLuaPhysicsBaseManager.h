/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBaseManager.h
 *  PURPOSE:     Lua physics base manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

template <class T>
class CLuaPhysicsBaseManager;

#pragma once

template <class T>
class CLuaPhysicsBaseManager
{
public:
    CLuaPhysicsBaseManager(EIdClassType idClass) : m_IdClass(idClass){};
    ~CLuaPhysicsBaseManager() {}

    void RemoveAll()
    {
        for (auto it = m_elementsList.rbegin(); it != m_elementsList.rend(); ++it)
        {
            Remove(*it);
        }
    }

    T GetFromScriptID(unsigned int uiScriptID)
    {
        T pElement = (T)CIdArray::FindEntry(uiScriptID, m_IdClass);
        if (!pElement)
            return nullptr;
        
        if (!ListContains(m_elementsList, pElement))
            return nullptr;
        return pElement;
    }

    virtual void Remove(T pElement) = 0;

    void Add(T pElement) { m_elementsList.push_back(pElement); }

    unsigned long Count() const { return m_elementsList.size(); }
    auto          IterBegin() { return m_elementsList.begin(); }
    auto          IterEnd() { return m_elementsList.end(); }

protected:
    std::vector<T> m_elementsList;

private:
    EIdClassType m_IdClass;
};
