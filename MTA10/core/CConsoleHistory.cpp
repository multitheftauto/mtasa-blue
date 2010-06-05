/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		core/CConsoleHistory.cpp
*  PURPOSE:		Maintaining the history of the in-game console
*  DEVELOPERS:	Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

CConsoleHistory::CConsoleHistory ( unsigned int uiHistoryLength )
{
    m_uiHistoryLength = uiHistoryLength;
    m_pFile = NULL;
}


CConsoleHistory::~CConsoleHistory ( void )
{
    // Unload an eventual file
    if ( m_pFile )
    {
        fclose ( m_pFile );
        m_pFile = NULL;
    }

    // Clear our history
    Clear ();
}


bool CConsoleHistory::LoadFromFile ( const char* szFilename, bool bAutosave )
{
    // Close the previous file if any
    if ( m_pFile )
    {
        fclose ( m_pFile );
        m_pFile = NULL;
    }

    // Open the file
    m_pFile = fopen ( szFilename, "a+" );
    if ( !m_pFile )
    {
        return false;
    }

    // Rewind
    rewind ( m_pFile );

    // Read all the lines
    char szBuffer [256];
    while ( fgets ( szBuffer, 256, m_pFile ) )
    {
        // Remove the \n from it if it's worth adding at all
        size_t sizeBuffer = strlen ( szBuffer );
        if ( sizeBuffer > 1 )
        {
            szBuffer [ sizeBuffer - 1 ] = 0;

            // Put it in our list
            char* szTemp = new char [ sizeBuffer + 1 ];
            strcpy ( szTemp, szBuffer );
            m_History.push_back ( szTemp );
        }
    }

    // Close the file if we're not supposed to autosave to it
    if ( !bAutosave )
    {
        fclose ( m_pFile );
        m_pFile = NULL;
    }
    else
        m_strFilename = szFilename ? szFilename : "";

    // Success
    return true;
}


void CConsoleHistory::Add ( const char* szLine )
{
    // Get the last typed string
    char* szLast = NULL;
    if ( m_History.size () > 0 )
    {
        szLast = m_History.front ();
    }

    // If the string is not empty and they differ
    if ( szLine[0] != 0 && ( !szLast || strcmp ( szLast, szLine ) != 0 ) )
    {
        // Allocate and add the line to the list, gets deleted in destructor
        char* szTemp = new char [ strlen ( szLine ) + 1 ];
        strcpy ( szTemp, szLine );
        m_History.push_front ( szTemp );

        // Is the list too big? Remove the last item
        if ( m_History.size () > m_uiHistoryLength )
        {
            DeleteLastEntry ();
        }

        // Write to the file history if any
        if ( m_pFile )
        {
            // Clear it
            m_pFile = freopen ( m_strFilename.c_str (), "w+", m_pFile );
            if ( m_pFile )
            {
                // Write the history, one per line
                std::list < char* > ::iterator iter = m_History.begin ();
                for ( ; iter != m_History.end (); iter++ )
                {
                    fprintf ( m_pFile, "%s\n", *iter );
                }

                // Flush it into the file
                fflush ( m_pFile );
            }
        }
    }
}


void CConsoleHistory::Clear ( void )
{
    // Delete all the history strings
    list < char* > ::iterator iter;
    for ( iter = m_History.begin (); iter != m_History.end (); iter++ )
    {
        delete [] *iter;
    }

    // Clear the list
    m_History.clear ();
}


const char* CConsoleHistory::Get ( unsigned int uiIndex )
{
    // Grab the item with the chosen index
    unsigned int uiTemp = 0;
    list < char* > ::iterator iter;
    for ( iter = m_History.begin (); iter != m_History.end (); iter++ )
    {
        if ( uiTemp == uiIndex )
        {
            return *iter;
        }

        ++uiTemp;
    }

    // Couldn't find it, return NULL
    return NULL;
}


void CConsoleHistory::DeleteLastEntry ( void )
{
    // We got entries?
    if ( !m_History.empty () )
    {
        // Grab it and remove it from the list
        char* szTemp = m_History.back ();
        m_History.remove ( szTemp );

        // Delete the string
        delete [] szTemp;
    }
}
