/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientFBXManager.h
 *  PURPOSE:     PointLights entity manager class header
 *
 *****************************************************************************/

#pragma once

class CClientFBXManager : CFBXManager
{
    friend class CClientManager;
    friend class CClientFBX;

public:
    CClientFBXManager(CClientManager* pManager);
    ~CClientFBXManager(void);


    std::list<CClientFBX*>::const_iterator IterBegin(void) { return m_List.begin(); };
    std::list<CClientFBX*>::const_iterator IterEnd(void) { return m_List.end(); };

private:
    void AddToList(CClientFBX* pFBX) { m_List.push_back(pFBX); };
    void RemoveFromList(CClientFBX* pFBX);
    void DeleteAll();

private:
    CClientManager* m_pManager;

    std::list<CClientFBX*> m_List;
};
