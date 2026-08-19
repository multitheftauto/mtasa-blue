/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Server/mods/deathmatch/logic/CObjectConfig.h
 *  PURPOSE:     Objects configuration loader and validator
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

class CObjectConfig
{
public:
    explicit CObjectConfig(std::string filePath);

    bool Load();

    bool IsValidModel(std::uint32_t modelId) const noexcept;

private:
    std::string                       m_filePath;
    std::unordered_set<std::uint32_t> m_validModels;
};
