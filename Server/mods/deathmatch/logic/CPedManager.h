/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPedManager.h
 *  PURPOSE:     Ped entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <list>

class CEvents;
class CElement;
class CXMLNode;

class CPedManager
{
    friend class CPed;

public:
    CPedManager();
    ~CPedManager();

    class CPed* Create(unsigned short usModel, CElement* pParent);
    class CPed* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void        DeleteAll();

    unsigned int Count() { return static_cast<unsigned int>(m_List.size()); };
    bool         Exists(class CPed* pPed);

    std::list<class CPed*>::const_iterator IterBegin() { return m_List.begin(); }
    std::list<class CPed*>::const_iterator IterEnd() { return m_List.end(); }

    static bool IsValidModel(unsigned short usModel);

protected:
    void AddToList(class CPed* pPed) { m_List.push_back(pPed); }
    void RemoveFromList(class CPed* pPed);

    std::list<class CPed*> m_List;
};
