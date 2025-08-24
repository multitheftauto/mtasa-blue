/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CTextDisplay.h
 *  PURPOSE:     Text display class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CTextDisplay;

#pragma once

#include <list>
#include "CPlayer.h"
#include "CTextItem.h"

class CTextDisplay
{
public:
    CTextDisplay();
    ~CTextDisplay();

    void AddObserver(CPlayer* pPlayer);
    void RemoveObserver(CPlayer* pPlayer);
    bool IsObserver(CPlayer* pPlayer);
    void GetObservers(lua_State* pLua);
    void AddObserver(CPlayerTextManager* pTextManager);
    void RemoveObserver(CPlayerTextManager* pTextManager);
    void Update(CTextItem* pTextItem, bool bRemovedFromDisplay = false);

    void Add(CTextItem* pTextItem);
    void Remove(CTextItem* pTextItem, bool bRemoveFromList = true);
    uint GetScriptID() { return m_uiScriptID; }

private:
    std::list<CPlayerTextManager*> m_observers;
    std::list<CTextItem*>          m_contents;
    uint                           m_uiScriptID;
};
