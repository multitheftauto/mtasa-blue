/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFileSystemMemoryHandleManager.cpp
*  PURPOSE:     Memory buffer filesystem manager
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CFileSystemMemoryHandleManager::CFileSystemMemoryHandleManager ( void )
{

}


CFileSystemMemoryHandleManager::~CFileSystemMemoryHandleManager ( void )
{

}


CFileSystemMemoryHandle* CFileSystemMemoryHandleManager::Add ( CFileSystemMemoryHandle* pFileSystemMemoryHandle )
{
    m_List.push_back ( pFileSystemMemoryHandle );
    return pFileSystemMemoryHandle;
}


CFileSystemMemoryHandle* CFileSystemMemoryHandleManager::Get ( HANDLE hFile )
{
    if ( m_List.size () > 0 )
    {
        list < CFileSystemMemoryHandle* > ::iterator iter;
        for ( iter = m_List.begin (); iter != m_List.end (); iter++ )
        {
            if ( (*iter)->GetHandle () == hFile )
            {
                return *iter;
            }
        }
    }

    return NULL;
}


void CFileSystemMemoryHandleManager::Remove ( CFileSystemMemoryHandle* pFileSystemMemoryHandle )
{
    if ( !m_List.empty() ) m_List.remove ( pFileSystemMemoryHandle );
}


void CFileSystemMemoryHandleManager::Remove ( HANDLE hFile )
{
    CFileSystemMemoryHandle* pHandle = Get ( hFile );
    if ( pHandle )
    {
        Remove ( pHandle );
    }
}
