/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConsoleHistory.cpp
*  PURPOSE:     Maintaining the history of the in-game console
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CConsoleHistory::CConsoleHistory ( unsigned int uiHistoryLength )
{
    m_uiHistoryLength = uiHistoryLength;
}


void CConsoleHistory::LoadFromFile ( void )
{
    // Copy and clean console input history
    SString strLogDir = CalcMTASAPath( "MTA" );
    SString strPathFilenameSource = PathJoin( strLogDir, "console.log" );
    SString strPathFilenameDest = PathJoin( strLogDir, "console-input.log" );
    CConsoleLogger::CleanFile( strPathFilenameSource, strPathFilenameDest );
    strPathFilenameSource = PathJoin( strLogDir, "console_input.log" );
    CConsoleLogger::CleanFile( strPathFilenameSource, strPathFilenameDest );

    m_strFilename = strPathFilenameDest;

    // Load the history lines
    char szBuffer [256];
    std::ifstream infile ( FromUTF8( m_strFilename ) );
    while( infile.getline ( szBuffer, 256 ) )
        if ( szBuffer[0] && !m_History.Contains ( szBuffer ) )
        {
            m_History.push_back ( szBuffer );
            m_HistoryNotSaved.push_back ( szBuffer );
        }
    infile.close ();
}


void CConsoleHistory::Add ( const char* szLine )
{
    if ( !szLine[0] )
        return;

    SString strLineClean = szLine;
    CConsoleLogger::CleanLine( strLineClean );

    // Remove any matching line
    m_History.remove ( strLineClean );
    m_HistoryNotSaved.remove ( szLine );

    // Push to front
    m_History.push_front ( strLineClean );
    m_HistoryNotSaved.push_front ( szLine );

    // Is the list too big? Remove the last items
    while ( m_History.size () > m_uiHistoryLength )
        m_History.pop_back ();

    // Is the list too big? Remove the last items
    while ( m_HistoryNotSaved.size () > m_uiHistoryLength )
        m_HistoryNotSaved.pop_back ();

    // Write the history, one per line
    std::ofstream outfile ( FromUTF8( m_strFilename ) );
    for ( std::list < SString > ::iterator iter = m_History.begin () ; iter != m_History.end (); iter++ )
        outfile << *iter << std::endl;
    outfile.close ();
}

const char* CConsoleHistory::Get ( unsigned int uiIndex )
{
    // Grab the item with the chosen index
    uint i = 0;
    for ( std::list < SString > ::iterator iter = m_HistoryNotSaved.begin (); iter != m_HistoryNotSaved.end (); iter++, i++ )
        if ( i == uiIndex )
            return *iter;

    return NULL;
}
