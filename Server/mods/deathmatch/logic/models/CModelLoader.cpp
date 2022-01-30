/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/models/CModelLoader.cpp
 *  PURPOSE:     Model info loader class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include <iostream>
#include <fstream>

CModelLoader::CModelLoader()
{
    // Should be replaced
    loadDefaultData();
}

CModelLoader::~CModelLoader()
{
}

void CModelLoader::loadDefaultData()
{
    filesystem::path modPath = filesystem::path(g_pServerInterface->GetServerModPath());

    loadObjectDefs(modPath / "objects.conf");
    loadVehicleDefs(modPath / "vehicles.conf");
}

void CModelLoader::unloadDefaultData()
{

}

void CModelLoader::loadVehicleHandling(std::string_view strData)
{
}

void CModelLoader::loadObjectDefs(const std::filesystem::path path)
{
    auto parseLineFunction = [](std::vector<char*> lineParams)
    {
        uint32_t uiModelID = atoi(lineParams[0]);
        char*    strModelName = lineParams[1];
        char*    strTextureName = lineParams[2];
        uint32_t iDrawDistance = atoi(lineParams[3]);
        uint32_t iFlags = atoi(lineParams[4]);

        //CModelAtomic* pNewObjectModel = new CModelAtomic(*strModelName, *strTextureName, iDrawDistance, iFlags);
        //g_pGame->GetModelManager()->RegisterModel(pNewObjectModel);
    };

    parseConfigCSV(path, parseLineFunction);
}

void CModelLoader::loadVehicleDefs(const std::filesystem::path path)
{
    auto parseLineFunction = [](std::vector<char*> lineParams) {
        uint32_t uiModelID = atoi(lineParams[0]);
        char*    strModelName = lineParams[1];
        char*    strTextureName = lineParams[2];
        char*    strType = lineParams[3];
        uint8_t  cModelVaiants = atoi(lineParams[4]);
        uint8_t  cAtributes = atoi(lineParams[5]);
        uint8_t  cPassengers = atoi(lineParams[6]);
        //char*    strName = lineParams[4];
        //char*    strAnims = lineParams[5];
        //char*    strClass = lineParams[6];
        //uint32_t iFlags = atoi(lineParams[7]);
        //char*    strComprutes = lineParams[8];
        //uint32_t uiWheelsID = atoi(lineParams[9]);
        //float    fWheelSizeF = atof(lineParams[10]);
        //float    fWheelSizeR = atof(lineParams[11]);
        //uint32_t uiWheelUpgradeClass = atoi(lineParams[12]);

        eVehicleType eType;
        if (strcmp(strType, "car") == 0)
        {
            eType = eVehicleType::VEHICLE_CAR;
        }
        else if (strcmp(strType, "mtruck") == 0)
        {
            eType = eVehicleType::VEHICLE_MONSTERTRUCK;
        } 
        else if (strcmp(strType, "heli") == 0)
        {
            eType = eVehicleType::VEHICLE_HELI;
        }
        else if (strcmp(strType, "boat") == 0)
        {
            eType = eVehicleType::VEHICLE_BOAT;
        }
        else if (strcmp(strType, "trailer") == 0)
        {
            eType = eVehicleType::VEHICLE_TRAILER;
        }
        else if (strcmp(strType, "train") == 0)
        {
            eType = eVehicleType::VEHICLE_TRAIN;
        }
        else if (strcmp(strType, "plane") == 0)
        {
            eType = eVehicleType::VEHICLE_PLANE;
        }
        else if (strcmp(strType, "bike") == 0)
        {
            eType = eVehicleType::VEHICLE_BIKE;
        }
        else if (strcmp(strType, "quad") == 0)
        {
            eType = eVehicleType::VEHICLE_QUADBIKE;
        }
        else if (strcmp(strType, "bmx") == 0)
        {
            eType = eVehicleType::VEHICLE_BMX;
        }

        // Handling is still hardcoded
        const CHandlingEntry* pHandling = g_pGame->GetHandlingManager()->GetOriginalHandlingData((eVehicleTypes)uiModelID);

        CModelVehicle* pModelData = new CModelVehicle(uiModelID, pHandling, eType, cModelVaiants, cAtributes, cPassengers);

        g_pGame->GetModelManager()->RegisterModel(pModelData);
    };

    parseConfigCSV(path, parseLineFunction);
}

void CModelLoader::parseConfigCSV(const std::filesystem::path path, std::function<void(std::vector<char*> lineParams)> parseLineFun)
{
    std::ifstream pFile(path.c_str(), std::ifstream::out);
    if (pFile.is_open())
    {
        std::string line;
        while (std::getline(pFile, line))
        {
            std::vector<char*> splittedParams;

            const char* delimiters = ", \t";
            char*       token = std::strtok(line.data(), delimiters);
            while (token)
            {
                splittedParams.push_back(token);
                token = std::strtok(nullptr, delimiters);
            }

            parseLineFun(splittedParams);
        }
        pFile.close();
    }
}
