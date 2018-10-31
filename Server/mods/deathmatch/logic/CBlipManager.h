/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBlipManager.h
 *  PURPOSE:     Blip entity manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CBlipManager;

#pragma once

#include "CBlip.h"
#include <list>

class CBlipManager
{
    friend class CBlip;

public:
    CBlipManager(void);
    ~CBlipManager(void) { DeleteAll(); };

    CBlip* Create(CElement* pParent);
    CBlip* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void   DeleteAll(void);

    unsigned int Count(void) { return static_cast<unsigned int>(m_List.size()); };
    bool         Exists(CBlip* pBlip);

    list<CBlip*>::const_iterator IterBegin(void) { return m_List.begin(); };
    list<CBlip*>::const_iterator IterEnd(void) { return m_List.end(); };

    static bool IsValidIcon(unsigned long ulIcon) { return ulIcon <= 63; };

private:
    list<CBlip*> m_List;
};
