/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CObjectConfig.cpp
 *  PURPOSE:     Objects configuration loader and validator
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectConfig.h"
#include <fstream>

CObjectConfig::CObjectConfig(std::string filePath) : m_filePath(std::move(filePath))
{
}

bool CObjectConfig::Load()
{
    m_validModels.clear();

    std::ifstream file(m_filePath, std::ifstream::in);
    if (!file.is_open())
        return false;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos || line[first] < '0' || line[first] > '9')
            continue;

        std::uint32_t modelId = std::stoul(line.substr(first));
        m_validModels.insert(modelId);
    }

    file.close();
    return !m_validModels.empty();
}

bool CObjectConfig::IsValidModel(std::uint32_t modelId) const noexcept
{
    return m_validModels.find(modelId) != m_validModels.end();
}
