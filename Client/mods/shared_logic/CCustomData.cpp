/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CCustomData.cpp
*  PURPOSE:     Custom data storage class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_CCustomData
#include "profiler/SharedUtil.Profiler.h"


void CCustomData::Copy ( CCustomData* pCustomData )
{
    std::map < std::string, SCustomData > :: const_iterator iter = pCustomData->IterBegin ();
    for ( ; iter != pCustomData->IterEnd (); iter++ )
    {
        Set ( iter->first.c_str (), iter->second.Variable );
    }
}

SCustomData* CCustomData::Get ( const char* szName )
{
    assert ( szName );

    std::map < std::string, SCustomData > :: iterator it = m_Data.find ( szName );
    if ( it != m_Data.end () )
        return &it->second;

    return NULL;
}


void CCustomData::Set ( const char* szName, const CLuaArgument& Variable )
{
    assert ( szName );

    // Grab the item with the given name
    SCustomData* pData = Get ( szName );
    if ( pData )
    {
        // Update existing
        pData->Variable = Variable;
    }
    else
    {
        // Add new
        SCustomData newData;
        newData.Variable = Variable;
        m_Data [ szName ] = newData;
    }
}


bool CCustomData::Delete ( const char* szName )
{
    // Find the item and delete it
    std::map < std::string, SCustomData > :: iterator it = m_Data.find ( szName );
    if ( it != m_Data.end () )
    {
        m_Data.erase ( it );
        return true;
    }

    // Didn't exist
    return false;
}
