/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CElementGroup.h
 *  PURPOSE:     Element group class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CElementGroup;

#pragma once

#include "CElement.h"

class CElementGroup
{
private:
    CFastList<CElement*> m_elements;

public:
    ~CElementGroup();
    void         Add(CElement* element);
    void         Remove(CElement* element);
    unsigned int GetCount();

    CFastList<CElement*>::const_iterator IterBegin() { return m_elements.begin(); }
    CFastList<CElement*>::const_iterator IterEnd() { return m_elements.end(); }
};
