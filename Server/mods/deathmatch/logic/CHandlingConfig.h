/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CHandlingConfig.h
 *  PURPOSE:     Vehicle handling configuration loader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <vector>
#include "CHandlingEntry.h"
#include "CCommon.h"

class CHandlingManager;

class CHandlingConfig
{
public:
    explicit CHandlingConfig(std::string filePath);

    bool Load(const CHandlingManager* manager, tHandlingData originalHandlingData[HT_MAX]);

private:
    void LoadHandlingParams(const std::vector<std::string>& parameters, tHandlingData& handling);

    std::string m_filePath;
};
