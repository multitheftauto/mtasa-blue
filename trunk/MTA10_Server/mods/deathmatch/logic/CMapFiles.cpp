/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapFiles.cpp
*  PURPOSE:     Map file manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CMapFiles::CMapFiles ( void )
{
    m_uiLastCount = 0;
    m_llNewestTime = 0;
	m_uiLastRandomIndex = 0;
}


CMapFiles::~CMapFiles ( void )
{
    Reset ();
}


bool CMapFiles::Refresh ( void )
{
    char szBuffer [MAX_PATH];

    unsigned int uiRaces = 0;
    // Delete our previous list
    Reset ();

    #ifdef WIN32

        // Find all .map files in the maps folder
        WIN32_FIND_DATA FindData;
        HANDLE hFind = FindFirstFile ( g_pServerInterface->GetModManager ()->GetAbsolutePath ( "maps/*.map", szBuffer, MAX_PATH ), &FindData );
        if ( hFind != INVALID_HANDLE_VALUE )
        {
            // Remove the extension and store the time
            FindData.cFileName [ strlen ( FindData.cFileName ) - 4 ] = 0;
            unsigned int uiCount = 1;
            long long llHighestTime;
            memcpy ( &llHighestTime, &FindData.ftLastWriteTime, sizeof ( long long ) );

            // Add the first .map file
            Add ( FindData.cFileName );

            // Add each file from then..
            while ( FindNextFile ( hFind, &FindData ) && uiRaces < 256 )
            {
                // Remove the extension and grab the write-time
                FindData.cFileName [ strlen ( FindData.cFileName ) - 4 ] = 0;
                ++uiCount;
                long long llTempTime;
                memcpy ( &llTempTime, &FindData.ftLastWriteTime, sizeof ( long long ) );

                // If it's bigger than the last time, overwrite it
                if ( llTempTime > llHighestTime )
                {
                    llHighestTime = llTempTime;
                }

                // Add the .map file
                Add ( FindData.cFileName );

                // Increment the counter
                uiRaces++;
            }

            // Close the search
            FindClose ( hFind );

            // Bigger than the last time?
            if ( llHighestTime > m_llNewestTime || uiCount > m_uiLastCount )
            {
                m_uiLastCount = uiCount;
                m_llNewestTime = llHighestTime;
                return true;
            }
        }
        else
        {
            // If there were items in the folder last time, it has changed
            if ( m_uiLastCount != 0 )
            {
                m_uiLastCount = 0;
                return true;
            }
        }
    #else
        DIR *RaceDirectory;
		struct dirent *RaceDirectoryEntry;
		unsigned int uiCount = 0;
		time_t llHighestTime = 0;

		if ( ( RaceDirectory = opendir ( g_pServerInterface->GetModManager ()->GetAbsolutePath ( "maps/", szBuffer, MAX_PATH ) ) ) == NULL )
		{
			CLogger::ErrorPrintf ( "Could not open maps/ directory.\n" );
		} else {
			while ( ( RaceDirectoryEntry = readdir ( RaceDirectory ) ) != NULL )
			{
				if ( strstr ( RaceDirectoryEntry->d_name, ".map" ) != NULL ) {
					time_t llTempTime = 0;
					struct stat EntryStat;
					char FullPath[255] = { '\0' };
					++uiCount;

					// Check file times
					sprintf ( FullPath, g_pServerInterface->GetModManager ()->GetAbsolutePath ( "maps/%s", szBuffer, MAX_PATH ), RaceDirectoryEntry->d_name );
					if ( stat ( FullPath, &EntryStat ) == 0 ) {
						llTempTime = EntryStat.st_mtime;

						// If it's bigger than the last time, overwrite it
						if( llTempTime > llHighestTime )
						{
							llHighestTime = llTempTime;
						}

						// Remove the extension
						RaceDirectoryEntry->d_name [ strlen ( RaceDirectoryEntry->d_name ) - 4 ] = 0;
						Add ( RaceDirectoryEntry->d_name );

                        uiRaces++;
					} else {
						CLogger::ErrorPrintf ( "Could not stat %s\n", FullPath );
					}
				}
			}
		// Close the directory handle
		closedir ( RaceDirectory );
		}

		if ( uiCount == 0 ) {
			// If there were items in the folder last time, it has changed
			if ( m_uiLastCount != 0 )
			{
				m_uiLastCount = 0;
				return true;
			}
		} else {
            // Bigger than the last time?
            if ( llHighestTime > m_llNewestTime || uiCount > m_uiLastCount )
            {
                m_uiLastCount = uiCount;
                m_llNewestTime = llHighestTime;
                return true;
            }
		}
    #endif

    // The list hasn't changed
    return false;
}


void CMapFiles::Reset ( void )
{
    // Delete each item
    list < CMapFile* > ::iterator iter = m_Files.begin ();
    for ( ; iter != m_Files.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Files.clear ();
}


unsigned int CMapFiles::Count ( void )
{
    return static_cast < unsigned int > ( m_Files.size () );
}


bool CMapFiles::Exists ( const char* szFilename )
{
    return Get ( szFilename ) != NULL;
}


CMapFile* CMapFiles::Get ( const char* szFilename )
{
    if ( szFilename )
    {
        // Find the filename in our list
        list < CMapFile* > ::iterator iter = m_Files.begin ();
        for ( ; iter != m_Files.end (); iter++ )
        {
            if ( stricmp ( (*iter)->GetFilenamePointer (), szFilename ) == 0 )
            {
                return *iter;
            }
        }
    }

    // We couldn't find it
    return NULL;
}


CMapFile* CMapFiles::Get ( unsigned int uiID )
{
    list < CMapFile* > ::iterator iter = m_Files.begin ();
    for ( ; iter != m_Files.end (); iter++ )
    {
        if ( (*iter)->GetID () == uiID )
        {
            return *iter;
        }
    }
    return NULL;
}


CMapFile* CMapFiles::GetRandom ( void )
{
    // Grab number of files
    unsigned int uiCount = Count ();
    if ( uiCount > 0 )
    {
        // Create a random index
        unsigned int uiRandomIndex = rand () % uiCount;

        // If the randomly chosen race equals the last executed race, choose a new one
        if ( uiCount >= 2 && m_uiLastRandomIndex == uiRandomIndex )
        {
            if ( uiRandomIndex == 0 )
            {
                uiRandomIndex = 1;
            }
            else
            {
                --uiRandomIndex;
            }
        }

        // Set the last random index
        m_uiLastRandomIndex = uiRandomIndex;

        // Grab that index from the list
        unsigned int uiIndex = 0;
        list < CMapFile* > ::iterator iter = m_Files.begin ();
        for ( ; iter != m_Files.end (); iter++ )
        {
            if ( uiIndex == uiRandomIndex )
            {
                return *iter;
            }

            ++uiIndex;
        }
    }

    // No items to pick from
    return NULL;
}


void CMapFiles::Remove ( CMapFile* pFile )
{
    if ( !m_Files.empty() ) m_Files.remove ( pFile );
    delete pFile;
}


void CMapFiles::Remove ( const char* szFilename )
{
    CMapFile* pMapFile = Get ( szFilename );
    if ( pMapFile )
    {
        Remove ( pMapFile );
    }
}


void CMapFiles::Remove ( unsigned int uiID )
{
    CMapFile* pMapFile = Get ( uiID );
    if ( pMapFile )
    {
        Remove ( pMapFile );
    }
}


void CMapFiles::RemoveAll ( void )
{
    Reset ();
}


list < CMapFile* > ::iterator CMapFiles::IterBegin ( void )
{
    return m_Files.begin ();
}


list < CMapFile* > ::iterator CMapFiles::IterEnd ( void )
{
    return m_Files.end ();
}


CMapFile* CMapFiles::Add ( const char* szFilename )
{
    unsigned int uiID = Count ();
    CMapFile* pMapFile = new CMapFile ( uiID, szFilename );
    m_Files.push_back ( pMapFile );
    return pMapFile;
}
