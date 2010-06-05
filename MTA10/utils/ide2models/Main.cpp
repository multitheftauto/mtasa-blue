/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        utils/ide2models/Main.cpp
*  PURPOSE:     IDE to models converter
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "Main.h"
#include <cstring>

int main ( int iArgumentCount, char* szArgs [] )
{
    char* szInputFile = NULL;
    char* szOutputFile = NULL;
    bool bError = false;
    char szError [32];

    // Load the input file
    int i = 0;
    while ( i < iArgumentCount )
    {
        // -i
        if ( stricmp ( szArgs [i], "-i" ) == 0 )
        {
            ++i;
            if ( i < iArgumentCount )
            {
                if ( !szInputFile )
                {
                    szInputFile = new char [ strlen ( szArgs [i] ) + 1 ];
                    strcpy ( szInputFile, szArgs [i] );
                }
                else
                {
                    bError = true;
                    strcpy ( szError, "-i defined more than once" );
                }
            }
        }

        // -o
        if ( stricmp ( szArgs [i], "-o" ) == 0 )
        {
            ++i;
            if ( i < iArgumentCount )
            {
                if ( !szOutputFile )
                {
                    szOutputFile = new char [ strlen ( szArgs [i] ) + 1 ];
                    strcpy ( szOutputFile, szArgs [i] );
                }
                else
                {
                    bError = true;
                    strcpy ( szError, "-o defined more than once" );
                }
            }
        }

        // Increase index
        ++i;
    }

    // Error?
    if ( bError )
    {
        printf ( "ERROR: %s\n", szError );
        return 1;
    }

    // User specified an input/output file?
    if ( !szInputFile )
    {
        printf ( "ERROR: input file not specified (use -i)\n" );
        return 1;
    }

    if ( !szOutputFile )
    {
        printf ( "ERROR: output file not specified (use -o)\n" );
        return 1;
    }


    // Try to open the input file
    FILE* pInput = fopen ( szInputFile, "r" );
    if ( pInput )
    {
        // Try to open the output file
        FILE* pOutput = fopen ( szOutputFile, "w+" );
        if ( pOutput )
        {
            // Start converting
            return Convert ( pInput, pOutput );
        }
        else
        {
            printf ( "ERROR: couldn't open output file '%s'\n", szInputFile );
            return 1;
        }
    }
    else
    {
        printf ( "ERROR: couldn't open input file '%s'\n", szInputFile );
        return 1;
    }


    return 0;
}


int Convert ( FILE* pInput, FILE* pOutput )
{
    char szBuffer [256];
    memset ( szBuffer, 0, 256 );

    while ( !feof ( pInput ) )
    {
        // Read the a line
        if ( fgets ( szBuffer, 256, pInput ) )
        {
            // Longer than 0 chars?
            if ( strlen ( szBuffer ) > 0 )
            {
                // Try to parse it
                if ( !Parse ( szBuffer, pOutput ) )
                {
                    return 1;
                }
            }
        }
        else
        {
            // End of file or error
            if ( ferror ( pInput ) )
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

    // Succeeded
    return 0;
}


bool Parse ( const char* szLine, FILE* pOutput )
{
    // Copy the buffer
    char szBuffer [256];
    strcpy ( szBuffer, szLine );

    // Split it
    char* szID = strtok ( szBuffer, "," );
    char* szName = strtok ( NULL, " " );

    // Succeeded?
    if ( szID && szName )
    {
        // Strip off the last , in the name if it's there
        size_t sizeName = strlen ( szName );
        if ( szName [ sizeName -1 ] == ',' )
        {
            szName [ sizeName - 1] = 0;
        }

        // Write to output file
        fprintf ( pOutput, "%s %s [%s]\n", szID, szName, szID );
        return true;
    }

    return true;
}
