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

class CResource;

template <class T>
class CLuaPhysicsBaseManager
{
public:
    CLuaPhysicsBaseManager(EIdClassType idClass) : m_IdClass(idClass){};
    ~CLuaPhysicsBaseManager() {}

    T GetFromScriptID(unsigned int uiScriptID)
    {
        T pElement = (T)CIdArray::FindEntry(uiScriptID, m_IdClass);
        if (!pElement)
            return nullptr;
        
        if (!ListContains(m_elementsList, pElement))
            return nullptr;
        return pElement;
    }

    virtual void Remove(T pElement, bool deleteFromList = true) = 0;

    void RemoveAll(CResource* pResource)
    {
        std::vector<T> elementsToRemove;

        for (auto it = m_elementsList.begin(); it != m_elementsList.end(); ++it)
        {
            if ((*it)->GetOwnedResource() == pResource)
            {
                Remove(*it, false);
                elementsToRemove.push_back(*it);
            }
        }

        for (auto it : elementsToRemove)
            ListRemove(m_elementsList, it);
    }

    void Add(T pElement) { m_elementsList.push_back(pElement); }

    unsigned long Count() const { return m_elementsList.size(); }
    auto          IterBegin() { return m_elementsList.begin(); }
    auto          IterEnd() { return m_elementsList.end(); }

protected:
    std::vector<T> m_elementsList;

private:
    EIdClassType m_IdClass;
};
