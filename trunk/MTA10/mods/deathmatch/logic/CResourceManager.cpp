/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceManager.cpp
*  PURPOSE:     Resource manager
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CResourceManager::CResourceManager ( void )
{
}


CResourceManager::~CResourceManager ( void )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        delete (*iter);
    }
    m_resources.clear ();
}

CResource* CResourceManager::Add ( unsigned short usID, char* szResourceName )
{
    CResource* pResource = new CResource ( usID, szResourceName );
    if ( pResource )
    {
        m_resources.push_back ( pResource );
        return pResource;
    }
    return NULL;
}


CResource* CResourceManager::GetResource ( unsigned short usID )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( ( *iter )->GetID() == usID )
            return ( *iter );
    }
    return NULL;
}


CResource* CResourceManager::GetResource ( const char* szResourceName )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( strcmp ( ( *iter )->GetName(), szResourceName ) == 0 )
            return ( *iter );
    }
    return NULL;
}


void CResourceManager::LoadUnavailableResources ( CClientEntity *pRootEntity )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( !( ( *iter )->GetActive () ) )
			( *iter )->Load ( pRootEntity );
    }
}


bool CResourceManager::RemoveResource ( unsigned short usID )
{
    CResource* pResource = GetResource ( usID );
    if ( pResource )
    {
        Remove ( pResource );
        return true;
    }
    return false;
}

void CResourceManager::Remove ( CResource* pResource )
{
	// Delete all the resource's locally created children (the server won't do that)
	pResource->DeleteClientChildren ();

	// Delete the resource
    if ( !m_resources.empty() ) m_resources.remove ( pResource );
    delete pResource;
}


bool CResourceManager::Exists ( CResource* pResource )
{
    list < CResource* > ::iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( *iter == pResource )
        {
            return true;
        }
    }
    return false;
}


void CResourceManager::StopAll ( void )
{
    while ( m_resources.size () > 0 )
    {
        Remove ( m_resources.front () );
    }
}
