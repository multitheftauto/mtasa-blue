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
    loadVehicleHandling(modPath / "handling.conf");
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

void CModelLoader::loadVehicleHandling(const std::filesystem::path path)
{
    auto parseLineFunction = [](std::vector<char*> lineParams) {
        tHandlingData handling;
        uint32_t      uiModelID = atoi(lineParams[0]);
        handling.iVehicleID = uiModelID;
        handling.fMass = atof(lineParams[1]);
        handling.fTurnMass = atof(lineParams[2]);
        handling.fDragCoeff = atof(lineParams[3]);
        handling.vecCenterOfMass = CVector(atof(lineParams[4]), atof(lineParams[5]), atof(lineParams[6]));
        handling.uiPercentSubmerged = atoi(lineParams[7]);
        handling.fTractionMultiplier = atof(lineParams[8]);
        handling.Transmission.ucDriveType = lineParams[9][0];
        handling.Transmission.ucEngineType = lineParams[10][0];
        handling.Transmission.ucNumberOfGears = atoi(lineParams[11]);
        handling.Transmission.uiHandlingFlags = atoi(lineParams[12]);
        handling.Transmission.fEngineAcceleration = atof(lineParams[13]);
        handling.Transmission.fEngineInertia = atof(lineParams[14]);
        handling.Transmission.fMaxVelocity = atof(lineParams[15]);
        handling.fBrakeDeceleration = atof(lineParams[16]);
        handling.fBrakeBias = atof(lineParams[17]);
        handling.bABS = lineParams[18][0] == '0';
        handling.fSteeringLock = atof(lineParams[19]);
        handling.fTractionLoss = atof(lineParams[20]);
        handling.fTractionBias = atof(lineParams[21]);
        handling.fSuspensionForceLevel = atof(lineParams[22]);
        handling.fSuspensionDamping = atof(lineParams[23]);
        handling.fSuspensionHighSpdDamping = atof(lineParams[24]);
        handling.fSuspensionUpperLimit = atof(lineParams[25]);
        handling.fSuspensionLowerLimit = atof(lineParams[26]);
        handling.fSuspensionFrontRearBias = atof(lineParams[27]);
        handling.fSuspensionAntiDiveMultiplier = atof(lineParams[28]);
        handling.fCollisionDamageMultiplier = atof(lineParams[29]);
        handling.uiModelFlags = atoi(lineParams[30]);
        handling.uiHandlingFlags = atoi(lineParams[31]);
        handling.fSeatOffsetDistance = atof(lineParams[32]);
        handling.uiMonetary = atoi(lineParams[33]);
        handling.ucHeadLight = atoi(lineParams[34]);
        handling.ucTailLight = atoi(lineParams[35]);
        handling.ucAnimGroup = atoi(lineParams[36]);

        g_pGame->GetHandlingManager()->RehisterHandling(uiModelID, handling);
    };
    parseConfigCSV(path, parseLineFunction);
}

void CModelLoader::loadVehicleDefs(const std::filesystem::path path)
{
    auto parseLineFunction = [](std::vector<char*> lineParams) {
        SModelVehicleDefs* sModelData = new SModelVehicleDefs();

        uint32_t uiModelID = atoi(lineParams[0]);
        char*    strModelName = lineParams[1];
        char*    strTextureName = lineParams[2];
        char*    strType = lineParams[3];
        sModelData->uiVariantsCount = atoi(lineParams[4]);
        sModelData->cAttributes = atoi(lineParams[5]);
        sModelData->uiMaxPassengers = atoi(lineParams[6]);
        //char*    strName = lineParams[4];
        //char*    strAnims = lineParams[5];
        //char*    strClass = lineParams[6];
        //uint32_t iFlags = atoi(lineParams[7]);
        //char*    strComprutes = lineParams[8];
        //uint32_t uiWheelsID = atoi(lineParams[9]);
        //float    fWheelSizeF = atof(lineParams[10]);
        //float    fWheelSizeR = atof(lineParams[11]);
        //uint32_t uiWheelUpgradeClass = atoi(lineParams[12]);

        if (strcmp(strType, "car") == 0)
        {
            sModelData->eVehicleType = eVehicleType::CAR;
        }
        else if (strcmp(strType, "mtruck") == 0)
        {
            sModelData->eVehicleType = eVehicleType::MONSTERTRUCK;
        } 
        else if (strcmp(strType, "heli") == 0)
        {
            sModelData->eVehicleType = eVehicleType::HELI;
        }
        else if (strcmp(strType, "boat") == 0)
        {
            sModelData->eVehicleType = eVehicleType::BOAT;
        }
        else if (strcmp(strType, "trailer") == 0)
        {
            sModelData->eVehicleType = eVehicleType::TRAILER;
        }
        else if (strcmp(strType, "train") == 0)
        {
            sModelData->eVehicleType = eVehicleType::TRAIN;
        }
        else if (strcmp(strType, "plane") == 0)
        {
            sModelData->eVehicleType = eVehicleType::PLANE;
        }
        else if (strcmp(strType, "bike") == 0)
        {
            sModelData->eVehicleType = eVehicleType::BIKE;
        }
        else if (strcmp(strType, "quad") == 0)
        {
            sModelData->eVehicleType = eVehicleType::QUADBIKE;
        }
        else if (strcmp(strType, "bmx") == 0)
        {
            sModelData->eVehicleType = eVehicleType::BMX;
        }

        const CHandlingEntry* pHandling = g_pGame->GetHandlingManager()->GetOriginalHandlingData((eVehicleTypes)uiModelID);

        sModelData->pHandling = pHandling;

        CModelVehicle* pModelData = new CModelVehicle(uiModelID, sModelData);

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
