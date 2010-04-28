/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerStats.cpp
*  PURPOSE:     Player statistics class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerStats::~CPlayerStats ( void )
{
    vector < sStat* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        delete (*iter);
    }
    m_List.clear ();
}


bool CPlayerStats::GetStat ( unsigned short usID, float& fValue )
{
    vector < sStat* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        if ( (*iter)->id == usID )
        {
            fValue = (*iter)->value;
            return true;
        }
    }

    return false;
}


void CPlayerStats::SetStat ( unsigned short usID, float fValue )
{
    vector < sStat* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        if ( (*iter)->id == usID )
        {
            (*iter)->value = fValue;
            return;
        }
    }
    sStat* stat = new sStat;
    stat->id = usID;
    stat->value = fValue;
    m_List.push_back ( stat );
}
