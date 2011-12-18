/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        utils/map2xml/CConfig.cpp
*  PURPOSE:     Config reader class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "CConfig.h"

CConfig::CConfig ( const char* szFileName )
{
    // Set the file name
    m_szFileName = new char [ strlen ( szFileName ) + 1 ];
    strcpy ( m_szFileName, szFileName );

    // Set the pointer to the file
    m_fp = fopen ( szFileName, "r" );
}


CConfig::~CConfig ()
{
    // Close the pointer to the file
    if ( m_fp )
    {
        fclose ( m_fp );
    }

    // Delete the filename
    delete [] m_szFileName;
}


bool CConfig::GetEntry ( char* szEntry, char* szReturnText, int iInstance )
{
    char szTemp[256];
    memset ( szTemp, '\0', 256 );
    
    // If the file exists
    if ( m_fp ) {
        char szInput[256];
        memset ( szInput, '\0', 256 );
        int iHits = 1;
        // Itterate the lines of the file
        while ( !feof ( m_fp ) ) {
            // Grab the current line
            if ( fgets ( szInput, 256, m_fp ) ) {
                // Check for a comment
                if ( szInput[0] == '#' ) continue;
                // Check for the new line character
                else if ( szInput[0] == '\n' ) continue;
                else {
                    char szCurrentEntry[256];
                    memset ( szCurrentEntry, '\0', 256 );
                    int i = 0;
                    // Itterate the characters of the line till the end of the line or a comment and store it in a temp variable
                    while ( szInput[i] != ' ' && szInput[i] != '\0' && szInput[i] != '\n' && szInput[i] != '#' ) {
                        if ( szInput[i] != ' ' && szInput[i] != '\0' && szInput[i] != '\n' && szInput[i] != '#' ) {
                            szCurrentEntry[i] = szInput[i];
                        }
                        else {
                            szCurrentEntry[i] = '\0';
                        }
                        i++;
                    }
                    // If this is one of the entries we are looking for
                    if ( stricmp ( szCurrentEntry, szEntry ) == 0 ) {
                        // If it is the specific entry we are looking for
                        if (iHits == iInstance) {
                            // Check for comments or end of line
                            if ( ( szInput[i] == '\0' ) || ( szInput[i] == '\n' ) || ( szInput[i] == '#' ) ) {
                                // Reset the file pointer to the start of the file
                                rewind ( m_fp );
                                return false;
                            }
                            i++;
                            if ( ( szInput[i] == '\0' ) || ( szInput[i] == '\n' ) || ( szInput[i] == '#' ) ) {
                                // Reset the file pointer to the start of the file
                                rewind ( m_fp );
                                return false;
                            }
                            else {
                                int j = 0;
                                // Copy the temp variable to our buffer
                                while ( szInput[i] != '#' && szInput[i] != '\n' && szInput[i] != '\0' ) {
                                    szTemp[j] = szInput[i];
                                    i++;
                                    j++;
                                }
                                // Clear the temp variable
                                szTemp[strlen ( szTemp )] = '\0';
                                // Copy the buffer to the return buffer
                                strcpy ( szReturnText, szTemp );
                                // Reset the file pointer to the start of the file
                                rewind ( m_fp );
                                return true;
                            }
                        }
                        else {
                            // Increment the counter for the amount of correct entry hits
                            iHits++;
                        }
                    }
                }
            }
        }
        // Reset the file pointer to the start of the file
        rewind ( m_fp );
    }
    else
    {
        // Print the file error
        perror ( m_szFileName );
    }
    return false;
}


void CConfig::GetPreviousEntry ( char* szEntry, char* szReturnText, int iInstance )
{
    char szTemp[256];
    memset ( szTemp, '\0', 256 );
    // If the config file exists
    if ( m_fp ) {
        char szInput[256];
        memset ( szInput, '\0', 256 );
        int iHits = 1;
        // Itterate the lines of the config file
        while ( !feof ( m_fp ) ) {
            // Grab the current line of the file
            if ( fgets ( szInput, 256, m_fp ) ) {
                // Check for comments
                if ( szInput[0] == '#' ) continue;
                // Check for new line
                else if ( szInput[0] == '\n' ) continue;
                else {
                    char szCurrentEntry[256];
                    memset ( szCurrentEntry, '\0', 256 );
                    int i = 0;
                    // Itterate the characters of the file
                    while ( szInput[i] != ' ' && szInput[i] != '\0' && szInput[i] != '\n' && szInput[i] != '#' ) {
                        // Check for comments and end of line
                        if ( szInput[i] != ' ' && szInput[i] != '\0' && szInput[i] != '\n' && szInput[i] != '#' ) {
                            szCurrentEntry[i] = szInput[i];
                        }
                        else {
                            szCurrentEntry[i] = '\0';
                        }
                        i++;
                    }
                    // If this an entry we are looking for
                    if ( stricmp ( szCurrentEntry, szEntry ) == 0 ) {
                        // If this is the specific entry we are looking for
                        if ( iHits == iInstance ) {
                            if ( szTemp[0] != '\0' ) {
                                // Copy the result to the return buffer
                                strcpy ( szReturnText, szTemp );
                                break;
                            }
                            else {
                                // Copy Unknown to the return buffer
                                strcpy ( szReturnText, "Unknown" );
                                break;
                            }
                            break;
                        }
                        else {
                            iHits++;
                        }
                    }
                    else {
                        // Clear the temp buffer
                        memset ( szTemp, '\0', 256 );
                        strcpy ( szTemp, szInput );
                    }
                }
            }
        }
        // Reset the file pointer to the start of the file
        rewind ( m_fp );
    }
    else
    {
        // Print the file error
        perror( m_szFileName );
    }
}


int CConfig::GetNumberOfLines ()
{
    // Set the line count to -1
    int iLines = -1;
    // If the file exists
    if ( m_fp ) {
        // Set the line count to 0
        iLines++;
        char szInput[128] = { '\0' };
        // Itterate the lines of the file
        while ( !feof ( m_fp ) ) {
            // Grab the line of the file
            fgets ( szInput, 128, m_fp );
            // Increment the line counter
            iLines++;
        }
        // Reset the file pointer to the start of the file
        rewind ( m_fp );
    }
    else
    {
        // Print the file error
        perror ( m_szFileName );
    }
    // Return the number of lines found
    return iLines;
}


int CConfig::GetNumberOfEntries ()
{
    // Set the number of entries to -1
    int iEntries = -1;
    // If the file exists
    if ( m_fp ) {
        // Set the number of entries to 0
        iEntries++;
        char szInput[128] = { '\0' };
        // Itterate the lines of the file
        while ( !feof ( m_fp ) ) {
            // Grab the current line
            fgets ( szInput, 128, m_fp );
            // If this is not a comment or a new line
            if ( ( szInput[0] != '#' ) && ( szInput[0] != '\n' ) ) {
                // Increment the entry counter
                iEntries++;
            }
        }
        // Reset the file pointer to the start of the file
        rewind ( m_fp );
    }
    else
    {
        // Print the file error
        perror ( m_szFileName );
    }
    // Return the number of entries
    return iEntries;
}


int CConfig::GetNumberOfSpecificEntries ( char* szEntry )
{
    // Set the number of specific entries to -1
    int iEntries = -1;
    // If the file exists
    if ( m_fp ) {
        // Set the number of specific entries to 0
        iEntries++;
        char szInput[128] = { '\0' };
        // Itterate the lines of the file
        while ( !feof ( m_fp ) ) {
            // Grab the current line
            fgets ( szInput, 128, m_fp );
            // Check for comments or new lines
            if ( ( szInput[0] != '#' ) && ( szInput[0] != '\n' ) ) {
                char* szCurrentEntry = strtok ( szInput, " " );
                // If this is an entry we are looking for
                if ( stricmp ( szCurrentEntry, szEntry ) == 0 ) {
                    // Increment the number of specific entries counter
                    iEntries++;
                }
            }
        }
        // Reset the file pointer to the start of the file
        rewind ( m_fp );
    }
    else
    {
        // Print the file error
        perror ( m_szFileName );
    }
    // Return the number of specific entries
    return iEntries;
}


int CConfig::GetNumberOfEntryProperties ( char* szEntry, int iInstance )
{
    // Set the number of specific entry properties to 0
    int iProperties = 0;
    // If the file exists
    if ( m_fp ) {
        char szInput[256] = { '\0' };
        int iHits = 1;
        // Itterate the lines of the file
        while ( !feof ( m_fp ) ) {
            // Grab the current line
            if ( fgets ( szInput, 256, m_fp ) ) {
                // Check for comments
                if ( szInput[0] == '#' ) continue;
                // Check for a new line
                else if ( szInput[0] == '\n' ) continue;
                else {
                    char* szCurrentEntry = strtok(szInput, " ");
                    // If this is an entry we are looking for
                    if ( stricmp ( szCurrentEntry, szEntry ) == 0 ) {
                        // If this is the specific entry we are looking for
                        if ( iHits == iInstance ) {
                            char* szNextToken = strtok(NULL, " ");
                            // Check there are some properties
                            if ( szNextToken != NULL ) {
                                iProperties++;
                                // Itterate the properties
                                while ( szNextToken != NULL ) {
                                    szNextToken = strtok ( NULL, " " );
                                    if ( szNextToken != NULL ) {
                                        // Check for comments
                                        if ( szNextToken[0] == '#' ) break;
                                        else {
                                            // Increment the property counter
                                            iProperties++;
                                        }
                                    }
                                }
                            }
                            break;
                        }
                        else {
                            // Increment the hits counter
                            iHits++;
                        }
                    }
                }
            }
        }
        // Reset the file pointer to the start of the file
        rewind ( m_fp );
    }
    else
    {
        // Print the file error
        perror ( m_szFileName );
    }
    // Return the number of properties
    return iProperties;
}


void CConfig::GetLine ( int iLines, char* szReturnText )
{
    int i = 0;
    // If the file exists
    if ( m_fp ) {
        i++;
        char szInput[256];
        memset ( szInput, '\0', 256 );
        // Itterate the lines of the file
        while ( !feof ( m_fp ) ) {
            // Grab the current line
            if ( fgets ( szInput, 256, m_fp ) ) {
                // If this is the line we are looking for
                if ( i == iLines )
                {
                    // Copy the line to the return buffer
                    strcpy ( szReturnText, szInput );
                    break;
                }
                // Increment the line counter
                i++;
            }
        }
        // Reset the file pointer to the start of the file
        rewind ( m_fp );
    }
    else
    {
        // Print the file error
        perror ( m_szFileName );
    }
}


int CConfig::WriteToConfig ( char* szInput )
{
    // Set the file pointer to write mode
    m_fp = freopen ( m_szFileName, "at", m_fp );
    // If the file exists
    if ( m_fp ) {
        // Print a new line
        fprintf ( m_fp, "\n" );
        // Print the line to the end of the file
        fprintf ( m_fp, szInput );
    }       
    else 
    {
        // Print the file error
        perror ( m_szFileName );
    }
    // Set the file pointer to read mode for the rest of the object
    m_fp = freopen ( m_szFileName, "r", m_fp );
    return 0;
}

void CConfig::SetFileName ( char* szFileName )
{
    strcpy ( m_szFileName, szFileName );
}

void CConfig::GetFileName (char* szFileName )
{
    strcpy ( szFileName, m_szFileName );
}

bool CConfig::DoesFileExist ()
{
    // Check the file exists
    return m_fp != NULL;
}
