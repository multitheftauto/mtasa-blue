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
    while ( !m_resources.empty () )
    {
        CResource* pResource = m_resources.back ();

        CLuaArguments Arguments;
        Arguments.PushUserData ( pResource );
        pResource->GetResourceEntity ()->CallEvent ( "onClientResourceStop", Arguments, true );
        delete pResource;

        m_resources.pop_back ();
    }
}

CResource* CResourceManager::Add ( unsigned short usID, char* szResourceName, CClientEntity* pResourceEntity, CClientEntity* pResourceDynamicEntity )
{
    CResource* pResource = new CResource ( usID, szResourceName, pResourceEntity, pResourceDynamicEntity );
    if ( pResource )
    {
        m_resources.push_back ( pResource );
        return pResource;
    }
    return NULL;
}


CResource* CResourceManager::GetResource ( unsigned short usID )
{
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( ( *iter )->GetID() == usID )
            return ( *iter );
    }
    return NULL;
}


CResource* CResourceManager::GetResource ( const char* szResourceName )
{
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( strcmp ( ( *iter )->GetName(), szResourceName ) == 0 )
            return ( *iter );
    }
    return NULL;
}


void CResourceManager::LoadUnavailableResources ( CClientEntity *pRootEntity )
{
    list < CResource* > ::const_iterator iter = m_resources.begin ();
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
    return m_resources.Contains ( pResource );
}


void CResourceManager::StopAll ( void )
{
    while ( m_resources.size () > 0 )
    {
        Remove ( m_resources.front () );
    }
}

bool CResourceManager::ParseResourcePathInput ( std::string strInput, CResource* &pResource, std::string &strPath )
{
	std::string dummy;
	return ParseResourcePathInput ( strInput, pResource, strPath, dummy );
}

// pResource may be changed on return, and it could be NULL if the function returns false.
bool CResourceManager::ParseResourcePathInput ( std::string strInput, CResource* &pResource, std::string &strPath, std::string &strMetaPath )
{
    ReplaceOccurrencesInString ( strInput, "\\", "/" );
    eAccessType accessType = ACCESS_PUBLIC;

    if ( strInput[0] == '@' )
    {
        accessType = ACCESS_PRIVATE;
        strInput = strInput.substr ( 1 );
    }

    if ( strInput[0] == ':' )
    {
        unsigned int iEnd = strInput.find_first_of("/");
        if ( iEnd )
        {
            std::string strResourceName = strInput.substr(1,iEnd-1);
            pResource = g_pClientGame->GetResourceManager()->GetResource ( strResourceName.c_str() );
            if ( pResource && strInput[iEnd+1] )
            {
                strMetaPath = strInput.substr(iEnd+1);
                if ( IsValidFilePath ( strMetaPath.c_str() ) )
                {
                    strPath = PathJoin ( pResource->GetResourceDirectoryPath ( accessType ), strMetaPath );
                    return true;
                }
            }
        }
    }
    else if ( pResource && IsValidFilePath ( strInput.c_str() ) )
    {
        strPath = PathJoin ( pResource->GetResourceDirectoryPath ( accessType ), strInput );
        strMetaPath = strInput;
        return true;
    }
    return false;
}
