/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        utils/map2xml/Main.cpp
*  PURPOSE:     .map (race) to .xml (new) converter utility
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
    CConfig* pInput = new CConfig ( szInputFile );
    if ( pInput->DoesFileExist () )
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


int Convert ( CConfig* pInput, FILE* pOutput )
{
    // Grab vehicle ID
    char szVehicleID [64];
    pInput->GetEntry ( "Vehicle", szVehicleID, 1 );

    // Write the start
    fprintf ( pOutput, "<map mod=\"race\" version=\"1.0\">\r\n" );
    fprintf ( pOutput, "    <name>Insert name here</name>\r\n" );
    fprintf ( pOutput, "    <author>Insert author here</author>\r\n" );
    fprintf ( pOutput, "    <options>\r\n" );
    fprintf ( pOutput, "        <starttime>12:00</starttime>\r\n" );
    fprintf ( pOutput, "        <weather>0</weather>\r\n" );
    fprintf ( pOutput, "    </options>\r\n" );

    // Write the spawnpoints
    char szBuffer [256];
    int iCount = pInput->GetNumberOfSpecificEntries ( "Spawnpoint" );
    for ( int i = 0; i < iCount; i++ )
    {
        if ( pInput->GetEntry ( "Spawnpoint", szBuffer, i + 1 ) )
        {
            // Extract the position
            // Syntax: Checkpoint [x] [y] [z] [rotation]
            char* szX = strtok ( szBuffer, " " );
            char* szY = strtok ( NULL, " " );
            char* szZ = strtok ( NULL, " " );
            char* szRotation = strtok ( NULL, " " );

            // Got X Y Z?
            if ( szX && szY && szZ )
            {
                fprintf ( pOutput, "    <spawnpoint name=\"spawnpoint %i\">\r\n", i+1 );
                fprintf ( pOutput, "        <position>%s %s %s</position>\r\n", szX, szY, szZ );

                if ( szRotation )
                {
                    fprintf ( pOutput, "        <rotation>%s</rotation>\r\n", szRotation );
                }

                // Vehicle ID
                fprintf ( pOutput, "        <vehicle>%s</vehicle>\r\n", szVehicleID );
            }

            // Write the end
            fprintf ( pOutput, "    </spawnpoint>\r\n", i+1 );
        }
    }

    // Write the checkpoints
    iCount = pInput->GetNumberOfSpecificEntries ( "Checkpoint" );
    for ( int i = 0; i < iCount; i++ )
    {
        if ( pInput->GetEntry ( "Checkpoint", szBuffer, i + 1 ) )
        {
            // Extract the position
            // Syntax: Checkpoint [x] [y] [z] [type] [vehicle id] [red] [green] [blue]
            char* szX = strtok ( szBuffer, " " );
            char* szY = strtok ( NULL, " " );
            char* szZ = strtok ( NULL, " " );
            char* szType = strtok ( NULL, " " );
            char* szVehicle = strtok ( NULL, " " );
            char* szRed = strtok ( NULL, " " );
            char* szGreen = strtok ( NULL, " " );
            char* szBlue = strtok ( NULL, " " );

            // Got X Y Z?
            if ( szX && szY && szZ )
            {
                fprintf ( pOutput, "    <checkpoint name=\"checkpoint %i\">\r\n", i+1 );
                fprintf ( pOutput, "        <position>%s %s %s</position>\r\n", szX, szY, szZ );

                if ( szType )
                {
                    fprintf ( pOutput, "        <type>%s</type>\r\n", szType );

                    if ( szVehicle )
                    {
                        fprintf ( pOutput, "        <vehicle>%s</vehicle>\r\n", szVehicle );

                        if ( szRed && szGreen && szBlue )
                        {
                            fprintf ( pOutput, "        <color>%s %s %s</color>\r\n", szRed, szGreen, szBlue );
                        }
                    }
                }
            }

            // Write the end
            fprintf ( pOutput, "    </checkpoint>\r\n", i+1 );
        }
    }

    // Objects
    iCount = pInput->GetNumberOfSpecificEntries ( "Object" );
    for ( int i = 0; i < iCount; i++ )
    {
        if ( pInput->GetEntry ( "Object", szBuffer, i + 1 ) )
        {
            // Extract the position
            // Syntax: Object [x] [y] [z] [rx] [ry] [rz] [id]
            char* szX = strtok ( szBuffer, " " );
            char* szY = strtok ( NULL, " " );
            char* szZ = strtok ( NULL, " " );
            char* szRX = strtok ( NULL, " " );
            char* szRY = strtok ( NULL, " " );
            char* szRZ = strtok ( NULL, " " );
            char* szID = strtok ( NULL, " " );

            // Got X Y Z RX RY RZ?
            if ( szX && szY && szZ && szID )
            {
                fprintf ( pOutput, "    <object name=\"object %i\">\r\n", i+1 );
                fprintf ( pOutput, "        <position>%s %s %s</position>\r\n", szX, szY, szZ );
                fprintf ( pOutput, "        <rotation>%s %s %s</rotation>\r\n", szRX, szRY, szRZ );
                fprintf ( pOutput, "        <model>%s</model>\r\n", szID );
            }

            // Write the end
            fprintf ( pOutput, "    </object>\r\n", i+1 );
        }
    }

    // Vehicle-change pickups
    iCount = pInput->GetNumberOfSpecificEntries ( "VehicleChange" );
    for ( int i = 0; i < iCount; i++ )
    {
        if ( pInput->GetEntry ( "VehicleChange", szBuffer, i + 1 ) )
        {
            // Extract the stuff
            char* szX = strtok ( szBuffer, " " );
            char* szY = strtok ( NULL, " " );
            char* szZ = strtok ( NULL, " " );
            char* szVehicleModel = strtok ( NULL, " " );
            char* szRespawnInterval = strtok ( NULL, " " );

            // Got X Y Z ID?
            if ( szX && szY && szZ && szVehicleModel )
            {
                fprintf ( pOutput, "    <pickup name=\"vehiclechange %i\">\r\n", i+1 );
                fprintf ( pOutput, "        <type>vehiclechange</type>\r\n" );
                fprintf ( pOutput, "        <position>%s %s %s</position>\r\n", szX, szY, szZ );
                fprintf ( pOutput, "        <vehicle>%s</vehicle>\r\n", szVehicleModel );

                // Got respawn interval?
                if ( szRespawnInterval )
                {
                    fprintf ( pOutput, "        <respawn>%s</respawn>\r\n", szRespawnInterval );
                }
            }

            // Write the end
            fprintf ( pOutput, "    </pickup>\r\n", i+1 );
        }
    }

    // Nitro pickups
    iCount = pInput->GetNumberOfSpecificEntries ( "NitroPickup" );
    for ( int i = 0; i < iCount; i++ )
    {
        if ( pInput->GetEntry ( "NitroPickup", szBuffer, i + 1 ) )
        {
            // Extract the stuff
            char* szX = strtok ( szBuffer, " " );
            char* szY = strtok ( NULL, " " );
            char* szZ = strtok ( NULL, " " );
            char* szRespawnInterval = strtok ( NULL, " " );

            // Got X Y Z?
            if ( szX && szY && szZ )
            {
                fprintf ( pOutput, "    <pickup name=\"nitropickup %i\">\r\n", i+1 );
                fprintf ( pOutput, "        <type>nitro</type>\r\n" );
                fprintf ( pOutput, "        <position>%s %s %s</position>\r\n", szX, szY, szZ );

                // Got respawn interval?
                if ( szRespawnInterval )
                {
                    fprintf ( pOutput, "        <respawn>%s</respawn>\r\n", szRespawnInterval );
                }
            }

            // Write the end
            fprintf ( pOutput, "    </pickup>\r\n", i+1 );
        }
    }

    // Repair pickups
    iCount = pInput->GetNumberOfSpecificEntries ( "RepairPickup" );
    for ( int i = 0; i < iCount; i++ )
    {
        if ( pInput->GetEntry ( "RepairPickup", szBuffer, i + 1 ) )
        {
            // Extract the stuff
            char* szX = strtok ( szBuffer, " " );
            char* szY = strtok ( NULL, " " );
            char* szZ = strtok ( NULL, " " );
            char* szRespawnInterval = strtok ( NULL, " " );

            // Got X Y Z?
            if ( szX && szY && szZ )
            {
                fprintf ( pOutput, "    <pickup name=\"repairpickup %i\">\r\n", i+1 );
                fprintf ( pOutput, "        <type>repair</type>\r\n" );
                fprintf ( pOutput, "        <position>%s %s %s</position>\r\n", szX, szY, szZ );

                // Got respawn interval?
                if ( szRespawnInterval )
                {
                    fprintf ( pOutput, "        <respawn>%s</respawn>\r\n", szRespawnInterval );
                }
            }

            // Write the end
            fprintf ( pOutput, "    </pickup>\r\n", i+1 );
        }
    }

    // Write the end
    fprintf ( pOutput, "</map>" );

    // Pickups
    return 0;

    /*
<pickup>
<location>1231.0 4432.1 15.23</location>
<type>nitro</type>
</pickup>
</map>
*/
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
