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
    // loadDefaultData();
}

CModelLoader::~CModelLoader()
{
}

void CModelLoader::loadDefaultData()
{
    loadObjectDefs("objects.conf");
    loadVehicleHandling("handling.conf");
}


void CModelLoader::loadVehicleHandling(const char* strPath)
{
}

void CModelLoader::loadObjectDefs(const char* strPath)
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
}

void CModelLoader::loadVehicleHandling(const char* strPath)
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
}

void CModelLoader::loadVehicleDefs(const char* strPath)
{

}
