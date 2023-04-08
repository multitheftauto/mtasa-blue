#include "StdInc.h"
#include "CHandlingConfig.h"
#include <fstream>
#include "CHandlingEntry.h"
#include "models/CModelManager.h"
#include "CGame.h"

CHandlingConfig::CHandlingConfig(std::string_view strPath)
{
    m_strPacth = strPath;
}

bool CHandlingConfig::Load()
{
    std::ifstream pFile(m_strPacth.data(), std::ifstream::in);
    if (pFile.is_open())
    {
        std::string line;
        while (std::getline(pFile, line))
        {
            if (line[0] != '#')
            {
                std::vector<char*> splittedParams;

                const char* delimiters = ",";
                char*       token = std::strtok(line.data(), delimiters);
                while (token)
                {
                    splittedParams.push_back(token);
                    token = std::strtok(nullptr, delimiters);
                }

                LoadHandlingParams(splittedParams);
            }
        }
        pFile.close();
        return true;
    }
    return false;
}

void CHandlingConfig::LoadHandlingParams(std::vector<char*> lineParams)
{
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

    auto pVehicleModel = g_pGame->GetModelManager()->GetVehicleModel(uiModelID);

    if (pVehicleModel)
    {
        CHandlingEntry handlingEntry(&handling);
        pVehicleModel->SetVehicleDefaultHandling(handlingEntry);
    }
}
