/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CHandlingConfig.cpp
 *  PURPOSE:     Vehicle handling configuration loader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CHandlingConfig.h"
#include "CHandlingManager.h"
#include <fstream>
#include <sstream>

CHandlingConfig::CHandlingConfig(std::string filePath) : m_filePath(std::move(filePath))
{
}

bool CHandlingConfig::Load(const CHandlingManager* manager, tHandlingData originalHandlingData[HT_MAX])
{
    std::ifstream file(m_filePath, std::ifstream::in);
    if (!file.is_open())
        return false;

    std::string line;
    while (std::getline(file, line))
    {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> splittedParameters;
        std::stringstream        stream(line);
        std::string              token;
        while (std::getline(stream, token, ','))
        {
            size_t first = token.find_first_not_of(" \t\r\n");
            size_t last = token.find_last_not_of(" \t\r\n");
            if (first != std::string::npos && last != std::string::npos)
                splittedParameters.push_back(token.substr(first, (last - first + 1)));
            else
                splittedParameters.push_back("");
        }

        if (splittedParameters.size() >= 37)
        {
            std::uint32_t modelId = std::stoul(splittedParameters[0]);
            if (manager && modelId >= 400 && modelId <= 611)
            {
                eHandlingTypes handlingType = manager->GetHandlingID(modelId);
                if (handlingType < HT_MAX)
                {
                    LoadHandlingParams(splittedParameters, originalHandlingData[handlingType]);
                }
            }
        }
    }
    file.close();
    return true;
}

void CHandlingConfig::LoadHandlingParams(const std::vector<std::string>& parameters, tHandlingData& handling)
{
    handling.iVehicleID = std::stoi(parameters[0]);
    handling.fMass = std::stof(parameters[1]);
    handling.fTurnMass = std::stof(parameters[2]);
    handling.fDragCoeff = std::stof(parameters[3]);
    handling.vecCenterOfMass = CVector(std::stof(parameters[4]), std::stof(parameters[5]), std::stof(parameters[6]));
    handling.uiPercentSubmerged = std::stoul(parameters[7]);
    handling.fTractionMultiplier = std::stof(parameters[8]);
    handling.Transmission.ucDriveType = parameters[9].empty() ? 'F' : parameters[9][0];
    handling.Transmission.ucEngineType = parameters[10].empty() ? 'P' : parameters[10][0];
    handling.Transmission.ucNumberOfGears = static_cast<unsigned char>(std::stoul(parameters[11]));
    handling.Transmission.uiHandlingFlags = std::stoul(parameters[12]);
    handling.Transmission.fEngineAcceleration = std::stof(parameters[13]);
    handling.Transmission.fEngineInertia = std::stof(parameters[14]);
    handling.Transmission.fMaxVelocity = std::stof(parameters[15]);
    handling.fBrakeDeceleration = std::stof(parameters[16]);
    handling.fBrakeBias = std::stof(parameters[17]);
    handling.bABS = !parameters[18].empty() && (parameters[18][0] == '0' || parameters[18] == "false");
    handling.fSteeringLock = std::stof(parameters[19]);
    handling.fTractionLoss = std::stof(parameters[20]);
    handling.fTractionBias = std::stof(parameters[21]);
    handling.fSuspensionForceLevel = std::stof(parameters[22]);
    handling.fSuspensionDamping = std::stof(parameters[23]);
    handling.fSuspensionHighSpdDamping = std::stof(parameters[24]);
    handling.fSuspensionUpperLimit = std::stof(parameters[25]);
    handling.fSuspensionLowerLimit = std::stof(parameters[26]);
    handling.fSuspensionFrontRearBias = std::stof(parameters[27]);
    handling.fSuspensionAntiDiveMultiplier = std::stof(parameters[28]);
    handling.fCollisionDamageMultiplier = std::stof(parameters[29]);
    handling.uiModelFlags = std::stoul(parameters[30]);
    handling.uiHandlingFlags = std::stoul(parameters[31]);
    handling.fSeatOffsetDistance = std::stof(parameters[32]);
    handling.uiMonetary = std::stoul(parameters[33]);
    handling.ucHeadLight = static_cast<unsigned char>(std::stoul(parameters[34]));
    handling.ucTailLight = static_cast<unsigned char>(std::stoul(parameters[35]));
    handling.ucAnimGroup = static_cast<unsigned char>(std::stoul(parameters[36]));
}
