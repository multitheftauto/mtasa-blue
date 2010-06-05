/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CDOMConfig.cpp
*  PURPOSE:     DOM config class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

bool CDOMConfig::LoadFromFile ( const char* szFilename )
{
    // Open the file
    FILE* pFile = fopen ( szFilename, "r" );
    if ( pFile )
    {
        // While we've not reached the end
        char szBuffer [256];
        do
        {
            // Grab the current line
            fgets ( szBuffer, 256, pFile );

            // Get the pointer to the first non-whitespace char
            char* pStart = SkipWhitespace ( szBuffer );
            if ( *pStart == '#' )
            {
                SDOMConfigLine* pLine = new SDOMConfigLine;
                pLine->iType = DOMCONFIG_COMMENT;
                pLine->Entry = szBuffer;
                m_Lines.push_back ( pLine );
            }
            else if ( *pStart == 0 )
            {
                // Empty line
                SDOMConfigLine* pLine = new SDOMConfigLine;
                pLine->iType = DOMCONFIG_EMPTY;
                m_Lines.push_back ( pLine );
            }
            else
            {
                // Split it into key and arguments
                char* szKey = strtok ( pStart, " " );
                char* szArguments = strtok ( NULL, "\0" );

                // Got a key?
                if ( szKey )
                {
                    // Make a new line and set the key/data
                    SDOMConfigLine* pLine = new SDOMConfigLine;
                    pLine->iType = DOMCONFIG_DATA;
                    pLine->Key = szKey;

                    // Eventually set the arguments
                    if ( szArguments )
                    {
                        pLine->Entry = szArguments;
                    }

                    // Add the line to our list
                    m_Lines.push_back ( pLine );
                }
                else
                {
                    // Empty line
                    SDOMConfigLine* pLine = new SDOMConfigLine;
                    pLine->iType = DOMCONFIG_EMPTY;
                    m_Lines.push_back ( pLine );
                }
            }
        }
        while ( !feof ( pFile ) );

        // Close the file
        fclose ( pFile );
        return true;
    }

    return false;
}


bool CDOMConfig::SaveToFile ( const char* szFilename )
{
    // Create the file
    FILE* pFile = fopen ( szFilename, "w+" );
    if ( pFile )
    {
        // Write each entry to the file
        int iType;
        list < SDOMConfigLine* > ::const_iterator iter = m_Lines.begin ();
        for ( ; iter != m_Lines.end (); iter++ )
        {
            iType = (*iter)->iType;
            switch ( iType )
            {
                case DOMCONFIG_EMPTY:
                    fprintf ( pFile, "\n" );
                    break;

                case DOMCONFIG_DATA:
                    fprintf ( pFile, "%s %s", (*iter)->Key.c_str (), (*iter)->Entry.c_str () );
                    break;

                case DOMCONFIG_COMMENT:
                    fprintf ( pFile, "#%s", (*iter)->Entry.c_str () );
                    break;
            }
        }

        // Close the file
        fclose ( pFile );
        return true;
    }

    return false;
}


void CDOMConfig::Clear ( void )
{
    // Delete each item
    list < SDOMConfigLine* > ::const_iterator iter = m_Lines.begin ();
    for ( ; iter != m_Lines.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Lines.clear ();
}


SDOMConfigLine* CDOMConfig::FindEntry ( const char* szKey, unsigned int uiInstance )
{
    // Try to find the specified key in our list
    unsigned int uiCurrentInstance = 1;
    list < SDOMConfigLine* > ::const_iterator iter = m_Lines.begin ();
    for ( ; iter != m_Lines.end (); iter++ )
    {
        // Current key and the given key matches?
        if ( stricmp ( (*iter)->Key.c_str (), szKey ) == 0 )
        {
            // This is the instance of it we want?
            if ( uiInstance == uiCurrentInstance++ )
            {
                return *iter;
            }
        }
    }

    // Doesn't exist
    return NULL;
}


bool CDOMConfig::GetEntry ( const char* szKey, char* szEntry, unsigned int uiBufferSize, unsigned int uiInstance )
{
    // Try to find the entry
    SDOMConfigLine* pLine = FindEntry ( szKey, uiInstance );
    if ( pLine )
    {
        // Copy out the entry and return true
        strncpy ( szEntry, pLine->Entry.c_str (),uiBufferSize );
        return true;
    }

    return false;
}


unsigned int CDOMConfig::GetEntryCount ( const char* szKey )
{
    // Count the specified key instances in the list
    unsigned int uiCurrentCount = 0;
    list < SDOMConfigLine* > ::const_iterator iter = m_Lines.begin ();
    for ( ; iter != m_Lines.end (); iter++ )
    {
        // Current key and the given key matches?
        if ( stricmp ( (*iter)->Key.c_str (), szKey ) == 0 )
        {
            // Increment the count
            ++uiCurrentCount;
        }
    }

    // Return the count
    return uiCurrentCount;
}


void CDOMConfig::SetEntry ( const char* szKey, const char* szEntry, unsigned int uiInstance )
{
    // Grab the entry
    SDOMConfigLine* pLine = FindEntry ( szKey, uiInstance );
    if ( pLine )
    {
        // Set the new text
        pLine->Entry = szEntry;
    }
    else
    {
        // Add a new entry with the given key and content
        pLine = new SDOMConfigLine;
        pLine->iType = DOMCONFIG_DATA;
        pLine->Key = szKey;
        pLine->Entry = szEntry;
        m_Lines.push_back ( pLine );
    }
}


char* CDOMConfig::SkipWhitespace ( char* szString )
{
    while ( *szString && ( *szString == ' ' || *szString == 9 ) )
    {
        ++szString;
    }

    return szString;
}
