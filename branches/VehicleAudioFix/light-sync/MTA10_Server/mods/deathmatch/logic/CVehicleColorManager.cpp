/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVehicleColorManager.cpp
*  PURPOSE:     Vehicle entity color manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVehicleColor CVehicleColors::GetRandomColor ( void )
{
    // Grab a random index
    unsigned int uiSize = CountColors ();
    if ( uiSize > 0 )
    {
        // Create a random index
        unsigned int uiRandomIndex = GetRandom ( 0, uiSize - 1 );

        // Grab the random color we got off the list
        unsigned int uiIndex = 0;
        list < CVehicleColor > ::iterator iter = m_Colors.begin ();
        for ( ; iter != m_Colors.end (); iter++ )
        {
            if ( uiIndex == uiRandomIndex )
            {
                return *iter;
            }

            ++uiIndex;
        }
    }

    // No items, return default color (black)
    return CVehicleColor ();
}

bool CVehicleColorManager::Load ( const char* szFilename )
{
    // Make sure we're cleared
    Reset ();

    // Load vehiclecolors.conf
    FILE* pFile = fopen ( szFilename, "r" );
    if ( pFile )
    {
        // Read each line of it
        char szBuffer [256];
        while ( !feof ( pFile ) )
        {
            // Grab the current line
            fgets ( szBuffer, 256, pFile );

            // Is this a comment?
            if ( szBuffer [0] != '#' )
            {
                // Split it up in vehicle id, color1, color2, color3 and color4
                char* szModel = strtok ( szBuffer, " " );
                char* szColor1 = strtok ( NULL, " " );
                char* szColor2 = strtok ( NULL, " " );
                char* szColor3 = strtok ( NULL, " " );
                char* szColor4 = strtok ( NULL, " " );

                // Set the colors that exist
                unsigned short usModel = 0;
                unsigned char ucColor1 = 0;
                unsigned char ucColor2 = 0;
                unsigned char ucColor3 = 0;
                unsigned char ucColor4 = 0;
                if ( szModel )
                {
                    usModel = static_cast < unsigned short > ( atol ( szModel ) );

                    if ( szColor1 )
                    {
                        ucColor1 = static_cast < unsigned char > ( atol ( szColor1 ) );

                        if ( szColor2 )
                        {
                            ucColor2 = static_cast < unsigned char > ( atol ( szColor2 ) );

                            if ( szColor3 )
                            {
                                ucColor3 = static_cast < unsigned char > ( atol ( szColor3 ) );

                                if ( szColor4 )
                                {
                                    ucColor4 = static_cast < unsigned char > ( atol ( szColor4 ) );
                                }
                            }
                        }
                    }

                    // Add it to the list
                    CVehicleColor color;
                    color.SetPaletteColors ( ucColor1, ucColor2, ucColor3, ucColor4 );
                    AddColor ( usModel, color );
                }
            }
        }

        // Close it
        fclose ( pFile );
        return true;
    }

    // Couldn't load the file
    return false;
}


bool CVehicleColorManager::Generate ( const char* szFilename )
{
    // Try to open the file
    FILE* pFile = fopen ( szFilename, "w+" );
    if ( pFile )
    {
        // Write the default colorfile to it
        fwrite ( &szDefaultVehicleColors, 1, strlen ( szDefaultVehicleColors ), pFile );

        // Close the file and return success
        fclose ( pFile );
        return true;
    }
    
    // Failed
    return false;
}


void CVehicleColorManager::Reset ( void )
{
    // Remove all colors from all vehicles
    for ( int i = 0; i < 212; i++ )
    {
        m_Colors [i].RemoveAllColors ();
    }
}


void CVehicleColorManager::AddColor ( unsigned short usModel, const CVehicleColor& colVehicle )
{
    if ( usModel >= 400 && usModel <= 611 )
    {
        m_Colors [ usModel - 400 ].AddColor ( colVehicle );
    }
}


CVehicleColor CVehicleColorManager::GetRandomColor ( unsigned short usModel )
{
    if ( usModel >= 400 && usModel <= 611 )
    {
        return m_Colors [ usModel - 400 ].GetRandomColor ();
    }
    else
    {
        return CVehicleColor ();
    }
}
