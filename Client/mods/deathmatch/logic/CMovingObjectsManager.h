/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMovingObjectsManager.h
 *  PURPOSE:     Header for moving objects manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CDeathmatchObject.h"
#include <list>

class CMovingObjectsManager
{
public:
    void Add(CDeathmatchObject* pObject) { m_List.push_back(pObject); };
    void Remove(CDeathmatchObject* pObject)
    {
        if (!m_List.empty())
            m_List.remove(pObject);
    };

    void DoPulse(void);

private:
    std::list<CDeathmatchObject*> m_List;
};
