/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMovingObjectsManager.h
 *  PURPOSE:     Manager for moving objects
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <list>

class CDeathmatchObject;

class CMovingObjectsManager
{
public:
    void Add(CDeathmatchObject* pObject) { m_List.push_back(pObject); };
    void Remove(CDeathmatchObject* pObject)
    {
        if (!m_List.empty())
            m_List.remove(pObject);
    };

    void DoPulse();

private:
    std::list<CDeathmatchObject*> m_List;
};
