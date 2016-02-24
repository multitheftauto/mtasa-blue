/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CCustomData.cpp
*  PURPOSE:     Custom entity data class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CCustomData::Copy ( CCustomData* pCustomData )
{
    map < std::string, SCustomData > :: const_iterator iter = pCustomData->IterBegin ();
    for ( ; iter != pCustomData->IterEnd (); iter++ )
    {
        Set ( iter->first.c_str (), iter->second.Variable );
    }
}

SCustomData* CCustomData::Get ( const char* szName )
{
    assert ( szName );

    std::map < std::string, SCustomData > :: const_iterator it = m_Data.find ( szName );
    if ( it != m_Data.end () )
        return (SCustomData *)&it->second;

    return NULL;
}

SCustomData* CCustomData::GetSynced ( const char* szName )
{
    assert ( szName );

    std::map < std::string, SCustomData > :: const_iterator it = m_SyncedData.find ( szName );
    if ( it != m_SyncedData.end () )
        return (SCustomData *)&it->second;

    return NULL;
}


bool CCustomData::DeleteSynced ( const char* szName )
{
    // Find the item and delete it
    std::map < std::string, SCustomData > :: iterator iter = m_SyncedData.find ( szName );
    if ( iter != m_SyncedData.end () )
    {
        m_SyncedData.erase ( iter );
        return true;
    }

    // Didn't exist
    return false;
}

void CCustomData::UpdateSynced ( const char* szName, const CLuaArgument& Variable, bool bSynchronized )
{
    if ( bSynchronized )
    {
        SCustomData* pDataSynced = GetSynced ( szName );
        if ( pDataSynced )
        {
            pDataSynced->Variable = Variable;
            pDataSynced->bSynchronized = bSynchronized;
        }
        else
        {
            SCustomData newData;
            newData.Variable = Variable;
            newData.bSynchronized = bSynchronized;
            m_SyncedData [ szName ] = newData;
        }
    }
    else
    {
        DeleteSynced ( szName );
    }
}


void CCustomData::Set ( const char* szName, const CLuaArgument& Variable, bool bSynchronized )
{
    assert ( szName );

    // Grab the item with the given name
    SCustomData* pData = Get ( szName );
    if ( pData )
    {
        // Update existing
        pData->Variable = Variable;
        pData->bSynchronized = bSynchronized;
        UpdateSynced ( szName, Variable, bSynchronized );
    }
    else
    {
        // Add new
        SCustomData newData;
        newData.Variable = Variable;
        newData.bSynchronized = bSynchronized;
        m_Data [ szName ] = newData;
        UpdateSynced ( szName, Variable, bSynchronized );
    }
}


bool CCustomData::Delete ( const char* szName )
{
    // Find the item and delete it
    std::map < std::string, SCustomData > :: iterator it = m_Data.find ( szName );
    if ( it != m_Data.end () )
    {
        DeleteSynced ( szName );
        m_Data.erase ( it );
        return true;
    }

    // Didn't exist
    return false;
}

CXMLNode * CCustomData::OutputToXML ( CXMLNode * pNode )
{
    std::map < std::string, SCustomData > :: const_iterator iter = m_Data.begin ();
    for ( ; iter != m_Data.end (); iter++ )
    {
        CLuaArgument* arg = (CLuaArgument *)&iter->second.Variable;
        
        switch ( arg->GetType() )
        {
        case LUA_TSTRING:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create( iter->first.c_str () );
                attr->SetValue ( arg->GetString ().c_str () );
                break;
            }
        case LUA_TNUMBER:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create( iter->first.c_str () );
                attr->SetValue ( (float)arg->GetNumber () );
                break;
            }
        case LUA_TBOOLEAN:
            {
                CXMLAttribute* attr = pNode->GetAttributes().Create( iter->first.c_str () );
                attr->SetValue ( arg->GetBoolean () );
                break;
            }
        }
    }
    return pNode;   
}

unsigned short CCustomData::CountOnlySynchronized ( void )
{
    return m_SyncedData.size ( ); 
}
