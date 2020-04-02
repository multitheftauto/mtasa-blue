/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPlayerStats.h
 *  PURPOSE:     Player statistics class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CPlayerStats;

#pragma once

#include <vector>
using namespace std;

struct sStat
{
    unsigned short id;
    float          value;
};

class CPlayerStats
{
public:
    ~CPlayerStats();

    bool                           GetStat(unsigned short usID, float& fValue);
    void                           SetStat(unsigned short usID, float fValue);
    vector<sStat*>::const_iterator IterBegin() { return m_List.begin(); }
    vector<sStat*>::const_iterator IterEnd() { return m_List.end(); }
    unsigned short                 GetSize() { return static_cast<unsigned short>(m_List.size()); }

private:
    vector<sStat*> m_List;
};
