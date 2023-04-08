/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CVehicleColorConfig.cpp
 *  PURPOSE:     Vehicle colors loader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVehicleColorConfig.h"
#include "CVehicleDefaultColors.h"
#include "models/CModelManager.h"
#include "CGame.h"

bool CVehicleColorConfig::Load(const char* szFilename)
{
    // Load vehiclecolors.conf
    FILE* pFile = File::Fopen(szFilename, "r");
    if (pFile)
    {
        // Read each line of it
        char szBuffer[256];
        while (!feof(pFile))
        {
            // Grab the current line
            fgets(szBuffer, 256, pFile);

            // Is this a comment?
            if (szBuffer[0] != '#')
            {
                // Split it up in vehicle id, color1, color2, color3 and color4
                char* szModel = strtok(szBuffer, " ");
                char* szColor1 = strtok(NULL, " ");
                char* szColor2 = strtok(NULL, " ");
                char* szColor3 = strtok(NULL, " ");
                char* szColor4 = strtok(NULL, " ");

                // Set the colors that exist
                unsigned short usModel = 0;
                unsigned char  ucColor1 = 0;
                unsigned char  ucColor2 = 0;
                unsigned char  ucColor3 = 0;
                unsigned char  ucColor4 = 0;
                if (szModel)
                {
                    usModel = static_cast<unsigned short>(atol(szModel));

                    if (szColor1)
                    {
                        ucColor1 = static_cast<unsigned char>(atol(szColor1));

                        if (szColor2)
                        {
                            ucColor2 = static_cast<unsigned char>(atol(szColor2));

                            if (szColor3)
                            {
                                ucColor3 = static_cast<unsigned char>(atol(szColor3));

                                if (szColor4)
                                {
                                    ucColor4 = static_cast<unsigned char>(atol(szColor4));
                                }
                            }
                        }
                    }

                    // Can be zero for invalid config string
                    if (usModel)
                    {
                        // Add it to the list
                        CVehicleColor color;
                        color.SetPaletteColors(ucColor1, ucColor2, ucColor3, ucColor4);
                        g_pGame->GetModelManager()->GetVehicleModel(usModel)->AddColor(color);
                    }
                }
            }
        }

        // Close it
        fclose(pFile);
        return true;
    }

    // Couldn't load the file
    return false;
}

bool CVehicleColorConfig::Generate(const char* szFilename)
{
    // Try to open the file
    FILE* pFile = File::Fopen(szFilename, "w+");
    if (pFile)
    {
        // Write the default colorfile to it
        fwrite(&szDefaultVehicleColors, 1, strlen(szDefaultVehicleColors), pFile);

        // Close the file and return success
        fclose(pFile);
        return true;
    }

    // Failed
    return false;
}
