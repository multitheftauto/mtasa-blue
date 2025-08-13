/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CBlipManager.h
 *  PURPOSE:     Blip entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <list>

class CBlip;
class CElement;
class CEvents;

class CBlipManager
{
    friend class CBlip;

public:
    CBlipManager();
    ~CBlipManager() { DeleteAll(); };

    CBlip* Create(CElement* pParent);
    CBlip* CreateFromXML(CElement* pParent, CXMLNode& Node, CEvents* pEvents);
    void   DeleteAll();

    unsigned int Count() { return static_cast<unsigned int>(m_List.size()); };
    bool         Exists(CBlip* pBlip);

    std::list<CBlip*>::const_iterator IterBegin() { return m_List.begin(); };
    std::list<CBlip*>::const_iterator IterEnd() { return m_List.end(); };

    static bool IsValidIcon(unsigned long ulIcon) { return ulIcon <= 63; };

private:
    std::list<CBlip*> m_List;
};
